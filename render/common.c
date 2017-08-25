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
    if (offset0 > offset1)
    {
        float tmp = offset0;
        offset0 = offset1;
        offset1 = tmp;
        NVGcolor ctmp = color0;
        color0 = color1;
        color1 = ctmp;
    }
    float s0o = clampf(offset0, 0.0f, 1.0f);
    float s1o = clampf(offset1, 0.0f, 1.0f);
    unsigned s = s0o * GRADIENT_SAMPLES;
    unsigned e = s1o * GRADIENT_SAMPLES;
    float r = color0.rgba[0];
    float g = color0.rgba[1];
    float b = color0.rgba[2];
    float a = color0.rgba[3];
    float dr = (color1.rgba[0] - r) / (e-s);
    float dg = (color1.rgba[1] - g) / (e-s);
    float db = (color1.rgba[2] - b) / (e-s);
    float da = (color1.rgba[3] - a) / (e-s);
    for (unsigned i = s; i < e; i++)
    {
        unsigned ur = (unsigned)(r*255); unsigned ug = (unsigned)(g*255); unsigned ub = (unsigned)(b*255); unsigned ua = (unsigned)(a*255);
        dst[i] = (ua << 24) | (ub << 16) | (ug << 8) | ur;
        r += dr; g += dg; b += db; a += da;
    }
}

int LinearGradientStops(NSVGshape *shape, LVGObject *o)
{
    int nstops = shape->fill.gradient->nstops;
    uint32_t data[GRADIENT_SAMPLES];
    float max_offset = shape->fill.gradient->stops[0].offset;
    struct NVGcolor s0 = transformColor(nvgColorU32(shape->fill.gradient->stops[0].color), o);
    gradientSpan(data, nvgRGBAf(0.f, 0.f, 0.f, 1.f), s0, 0, max_offset);
    for (unsigned i = 0; i < (nstops - 1); i++)
    {
        if (max_offset < shape->fill.gradient->stops[i + 1].offset)
        {
            max_offset = shape->fill.gradient->stops[i + 1].offset;
            s0 = nvgColorU32(shape->fill.gradient->stops[i + 1].color);
        }
        gradientSpan(data, transformColor(nvgColorU32(shape->fill.gradient->stops[i].color), o),
                     transformColor(nvgColorU32(shape->fill.gradient->stops[i + 1].color), o),
                     shape->fill.gradient->stops[i].offset,
                     shape->fill.gradient->stops[i + 1].offset);
    }
    if (max_offset < 1.0f)
        gradientSpan(data, s0, s0, max_offset, 1.0f);
    return g_render->cache_image(g_render_obj, GRADIENT_SAMPLES, 1, 0, (unsigned char*)data);
}
