#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "render/render.h"
#include <math.h>

extern render *g_render;
extern void *g_render_obj;

void identity(Transform3x2 dst)
{
    dst[0][0] = 1;
    dst[0][1] = 0;
    dst[0][2] = 0;
    dst[1][0] = 0;
    dst[1][1] = 1;
    dst[1][2] = 0;
}

void mul(Transform3x2 dst, Transform3x2 a, Transform3x2 b)
{
    Transform3x2 result;
    result[0][0] = a[0][0]*b[0][0] + a[0][1]*b[1][0];
    result[0][1] = a[0][0]*b[0][1] + a[0][1]*b[1][1];
    result[0][2] = a[0][0]*b[0][2] + a[0][1]*b[1][2] + a[0][2];

    result[1][0] = a[1][0]*b[0][0] + a[1][1]*b[1][0];
    result[1][1] = a[1][0]*b[0][1] + a[1][1]*b[1][1];
    result[1][2] = a[1][0]*b[0][2] + a[1][1]*b[1][2] + a[1][2];

    dst[0][0] = result[0][0];
    dst[0][1] = result[0][1];
    dst[0][2] = result[0][2];
    dst[1][0] = result[1][0];
    dst[1][1] = result[1][1];
    dst[1][2] = result[1][2];
}

void translate(Transform3x2 dst, float x, float y)
{
    dst[0][0] = 1;
    dst[0][1] = 0;
    dst[0][2] = x;
    dst[1][0] = 0;
    dst[1][1] = 1;
    dst[1][2] = y;
}

void scale(Transform3x2 dst, float x, float y)
{
    dst[0][0] = x;
    dst[0][1] = 0;
    dst[0][2] = 0;

    dst[1][0] = 0;
    dst[1][1] = y;
    dst[1][2] = 0;
}

void rotate(Transform3x2 dst, float angle)
{
    float radians = angle*3.14159f/180.0f,
          s = (float)sin(radians),
          c = (float)cos(radians);
    dst[0][0] = c;
    dst[0][1] = -s;
    dst[0][2] = 0;
    dst[1][0] = s;
    dst[1][1] = c;
    dst[1][2] = 0;
}

/*void ortho(Transform3x2 dst, float l, float r, float b, float t)
{
    dst[0][0] = 2/(r-l);
    dst[0][1] = 0;
    dst[0][2] = -(r+l)/(r-l);
    dst[1][0] = 0;
    dst[1][1] = 2/(t-b);
    dst[1][2] = -(t+b)/(t-b);
}

void inverse_ortho(Transform3x2 dst, float l, float r, float b, float t)
{
    dst[0][0] = (r-l)/2;
    dst[0][1] = 0;
    dst[0][2] = (r+l)/2;
    dst[1][0] = 0;
    dst[1][1] = (t-b)/2;
    dst[1][2] = (t+b)/2;
}*/

void xform(float dst[2], Transform3x2 a, const float v[2])
{
    float result[2];
    result[0] = a[0][0]*v[0] + a[0][1]*v[1] + a[0][2];
    result[1] = a[1][0]*v[0] + a[1][1]*v[1] + a[1][2];
    dst[0] = result[0];
    dst[1] = result[1];
}

void inverse(Transform3x2 dst, Transform3x2 data)
{
    double det = 1.0/(data[0][0]*data[1][1] - data[0][1]*data[1][0]);
    Transform3x2 result;
    result[0][0] = + (data[1][1]) * det;
    result[1][0] = - (data[1][0]) * det;
    result[0][1] = - (data[0][1]) * det;
    result[1][1] = + (data[0][0]) * det;
    result[0][2] = + (data[0][1] * data[1][2] - data[1][1] * data[0][2]) * det;
    result[1][2] = - (data[0][0] * data[1][2] - data[1][0] * data[0][2]) * det;

    dst[0][0] = result[0][0];
    dst[0][1] = result[0][1];
    dst[0][2] = result[0][2];
    dst[1][0] = result[1][0];
    dst[1][1] = result[1][1];
    dst[1][2] = result[1][2];
}

static float clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }

NVGcolor nvgColorU32(uint32_t c)
{
    return nvgRGBA(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff, c >> 24);
}

NVGcolor transformColor(NVGcolor color, LVGColorTransform *x)
{
    if (!x)
        return color;
    color = nvgRGBAf(color.r*x->mul[0], color.g*x->mul[1], color.b*x->mul[2], color.a*x->mul[3]);
    color = nvgRGBAf(color.r + x->add[0], color.g + x->add[1], color.b + x->add[2], color.a + x->add[3]);
    color = nvgRGBAf(fmax(0.0f, fmin(color.r, 1.0f)), fmax(0.0f, fmin(color.g, 1.0f)), fmax(0.0f, fmin(color.b, 1.0f)), fmax(0.0f, fmin(color.a, 1.0f)));
    return color;
}

static void gradientSpan(uint32_t * dst, NVGcolor color0, NVGcolor color1, float offset0, float offset1)
{
    float s0o = clampf(offset0, 0.0f, 1.0f);
    float s1o = clampf(offset1, 0.0f, 1.0f);
    unsigned s = s0o * GRADIENT_SAMPLES_L;
    unsigned e = s1o * GRADIENT_SAMPLES_L;
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

int LinearGradientStops(NSVGgradient *gradient, LVGColorTransform *x)
{
    int nstops = gradient->nstops;
    uint32_t data[GRADIENT_SAMPLES_L];
    if (gradient->stops[0].offset > 0.0f)
    {
        NVGcolor s0 = transformColor(nvgColorU32(gradient->stops[0].color), x);
        gradientSpan(data, s0, s0, 0.0f, gradient->stops[0].offset);
    }
    for (unsigned i = 0; i < (nstops - 1); i++)
    {
        gradientSpan(data, transformColor(nvgColorU32(gradient->stops[i].color), x),
                     transformColor(nvgColorU32(gradient->stops[i + 1].color), x),
                     gradient->stops[i].offset,
                     gradient->stops[i + 1].offset);
    }
    if (gradient->stops[nstops - 1].offset < 1.0f)
    {
        NVGcolor s0 = transformColor(nvgColorU32(gradient->stops[nstops - 1].color), x);
        gradientSpan(data, s0, s0, gradient->stops[nstops - 1].offset, 1.0f);
    }
    return g_render->cache_image(g_render_obj, GRADIENT_SAMPLES_L, 1, 0, (unsigned char*)data);
}

static NVGcolor lerpColor(NVGcolor color0, NVGcolor color1, float offset0, float offset1, float g)
{
    NVGcolor dst;
    float den = fmax(0.00001f, offset1 - offset0);
    for (int i = 0; i < 4; i++)
        dst.rgba[i] = color0.rgba[i] + (color1.rgba[i] - color0.rgba[i])*(g - offset0)/den;
    dst = nvgRGBAf(fmax(0.0f, fmin(dst.r, 1.0f)), fmax(0.0f, fmin(dst.g, 1.0f)), fmax(0.0f, fmin(dst.b, 1.0f)), fmax(0.0f, fmin(dst.a, 1.0f)));
    return dst;
}

static void calcStops(NSVGgradient *gradient, LVGColorTransform *x, NVGcolor *color0, NVGcolor *color1, float *stop0, float *stop1, float g)
{
    float *s0 = 0;
    float *s1 = 0;
    for (size_t i = 0; i < gradient->nstops && !s1; i++)
    {
        float *curr = &gradient->stops[i].offset;
        if ( g >= curr[0])
        {
            s0 = curr;
            *color0 = transformColor(nvgColorU32(gradient->stops[i].color), x);
        } else if ( s0 && g <= curr[0])
        {
            s1 = curr;
            *color1 = transformColor(nvgColorU32(gradient->stops[i].color), x);
        }
    }
    if (!s0)
    {
        s0 = &gradient->stops[0].offset;
        *color0 = transformColor(nvgColorU32(gradient->stops[0].color), x);
    }
    if (!s1)
    {
        s1 = &gradient->stops[gradient->nstops - 1].offset;
        *color1 = transformColor(nvgColorU32(gradient->stops[gradient->nstops - 1].color), x);
    }
    *stop0 = s0[0];
    *stop1 = s1[0];
}

int RadialGradientStops(NSVGgradient *gradient, LVGColorTransform *cxform)
{
    const int width = GRADIENT_SAMPLES_R, height = GRADIENT_SAMPLES_R;
    uint32_t *image = (unsigned int*)malloc(width*height*sizeof(uint32_t));
#define SPREAD_PAD     0
#define SPREAD_REPEAT  1
#define SPREAD_REFLECT 2

    int nstops = gradient->nstops;
    int spreadMode = 0;

    float fxn = width/2;
    float fyn = height/2;
    float fxp = 0;
    float fyp = 0;
    float rn = width/2 - 1.0001;
    float denominator = (rn*rn) - (fxp*fxp + fyp*fyp);

    for (int x = 0; x < width; x++)
    {
        float dx = x - fxn;
        for (int y = 0; y < height; y++)
        {
            float dy = y - fyn;

            float numerator = (dx*fxp + dy*fyp);
            float df = dx*fyp - dy*fxp;
            numerator += sqrtf((rn*rn) * (dx*dx + dy*dy) - (df*df));
            float g = numerator/denominator;

            // color = c0 + (c1 - c0)(g - x0)/(x1 - x0)
            // where c0 = stop color 0, c1 = stop color 1
            // where x0 = stop offset 0, x1 = stop offset 1
            NVGcolor finalcolor;
            float stop0, stop1;
            NVGcolor color0, color1;

            if (spreadMode == SPREAD_PAD)
            {
                if (g < 0.0f)
                {
                    finalcolor = transformColor(nvgColorU32(gradient->stops[0].color), cxform);
                } else if (g > 1.0f)
                {
                    finalcolor = transformColor(nvgColorU32(gradient->stops[nstops - 1].color), cxform);
                } else
                {
                    calcStops(gradient, cxform, &color0, &color1, &stop0, &stop1, g);
                    finalcolor = lerpColor(color0, color1, stop0, stop1, g);
                }
            } else
            {
                int w = (int)fabsf(g);
                if (spreadMode == SPREAD_REPEAT)
                {
                    if (g < 0)
                    {
                        g = 1 - (fabs(g) - w);
                    } else
                    {
                        g = g - w;
                    }
                } else if (spreadMode == SPREAD_REFLECT)
                {
                    if (g < 0)
                    {
                        if (w % 2 == 0)
                        {   // even
                            g = (fabsf(g) - w);
                        } else
                        {   // odd
                            g = (1 - (fabsf(g) - w));
                        }
                    } else
                    {
                        if (w % 2 == 0)
                        {   // even
                            g = g - w;
                        } else
                        {   // odd
                            g = 1 - (g - w);
                        }
                    }
                }
                // clamp
                if (g > 1)
                    g = 1;
                if (g < 0)
                    g = 0;
                calcStops(gradient, cxform, &color0, &color1, &stop0, &stop1, g);
                finalcolor = lerpColor(color0, color1, stop0, stop1, g);
            }
            uint32_t color = ((uint32_t)(finalcolor.a*255) << 24) | ((uint32_t)(finalcolor.b*255) << 16) |
                    ((uint32_t)(finalcolor.g*255) << 8) | (uint32_t)(finalcolor.r*255);
            image[(y*width) + x] = color;
        }
    }
    int img = g_render->cache_image(g_render_obj, width, height, 0, (unsigned char*)image);
    free(image);
    return img;
}

void gl_free_image(void *render, int image)
{
    GLuint img = image;
    glDeleteTextures(1, &img);
}
