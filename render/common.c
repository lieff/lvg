#include "render/render.h"
#include <math.h>

extern render *g_render;
extern void *g_render_obj;

#define GRADIENT_SAMPLES 1024

static float clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }

NVGcolor nvgColorU32(uint32_t c)
{
    return nvgRGBA(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff, c >> 24);
}

NVGcolor transformColor(NVGcolor color, LVGObject *o)
{
    if (!o)
        return color;
    color = nvgRGBAf(color.r*o->color_mul[0], color.g*o->color_mul[1], color.b*o->color_mul[2], color.a*o->color_mul[3]);
    color = nvgRGBAf(color.r + o->color_add[0], color.g + o->color_add[1], color.b + o->color_add[2], color.a + o->color_add[3]);
    color = nvgRGBAf(fmax(0.0f, fmin(color.r, 1.0f)), fmax(0.0f, fmin(color.g, 1.0f)), fmax(0.0f, fmin(color.b, 1.0f)), fmax(0.0f, fmin(color.a, 1.0f)));
    return color;
}

static void gradientSpan(uint32_t * dst, NVGcolor color0, NVGcolor color1, float offset0, float offset1)
{
    float s0o = clampf(offset0, 0.0f, 1.0f);
    float s1o = clampf(offset1, 0.0f, 1.0f);
    unsigned s = s0o * GRADIENT_SAMPLES;
    unsigned e = s1o * GRADIENT_SAMPLES;
    unsigned sc = 0xffffffff;
    unsigned sh = 24;
    unsigned r = color0.rgba[0] * sc;
    unsigned g = color0.rgba[1] * sc;
    unsigned b = color0.rgba[2] * sc;
    unsigned a = color0.rgba[3] * sc;
    unsigned dr = (color1.rgba[0] * sc - r) / (e-s);
    unsigned dg = (color1.rgba[1] * sc - g) / (e-s);
    unsigned db = (color1.rgba[2] * sc - b) / (e-s);
    unsigned da = (color1.rgba[3] * sc - a) / (e-s);
    for (unsigned i = s; i < e; i++)
    {
        dst[i] = ((a>>sh)<<24) + ((b>>sh)<<16) + ((g>>sh)<<8) + ((r>>sh)<<0);
        r += dr; g += dg; b += db; a += da;
    }
}

int LinearGradientStops(struct NVGcontext *vg, NSVGshape *shape, LVGObject *o)
{
    int nstops = shape->fill.gradient->nstops;
    uint32_t data[GRADIENT_SAMPLES];
    struct NVGcolor s0 = nvgRGBAf(0.f, 0.f, 0.f, 1.f);
    struct NVGcolor s1 = nvgRGBAf(1.f, 1.f, 1.f, 1.f);
    gradientSpan(data, s0, transformColor(nvgColorU32(shape->fill.gradient->stops[0].color), o), 0, shape->fill.gradient->stops[0].offset);
    for (unsigned i = 0; i < (nstops - 1); i++)
    {
        gradientSpan(data, transformColor(nvgColorU32(shape->fill.gradient->stops[i].color), o),
                     transformColor(nvgColorU32(shape->fill.gradient->stops[i + 1].color), o),
                     shape->fill.gradient->stops[i].offset,
                     shape->fill.gradient->stops[i + 1].offset);
    }
    gradientSpan(data, transformColor(nvgColorU32(shape->fill.gradient->stops[nstops - 1].color), o), s1,
            shape->fill.gradient->stops[nstops - 1].offset, 1);
    return g_render->cache_image(g_render_obj, GRADIENT_SAMPLES, 1, IMAGE_REPEAT, (unsigned char*)data);
}
