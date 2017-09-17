#pragma once
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "../nanovg/nanosvg.h"
#include "../nanovg/nanovg.h"
#include "lvg_header.h"

#define IMAGE_REPEAT 1
#define GRADIENT_SAMPLES_L 256
#define GRADIENT_SAMPLES_R 256

typedef struct render
{
    int (*init)(void **render);
    void (*release)(void *render);
    void (*begin_frame)(void *render, LVGMovieClip *clip, int winWidth, int winHeight, int width, int height);
    void (*end_frame)(void *render);
    int (*cache_shape)(void *render, NSVGshape *shape);
    int (*cache_image)(void *render, int width, int height, int flags, const void *rgba);
    int (*cache_gradient)(NSVGpaint *fill);
    void (*free_image)(int image);
    void (*update_image)(void *render, int image, const void *rgba);
    void (*render_shape)(void *render, NSVGshape *shape, LVGColorTransform *cxform);
    void (*render_image)(void *render, int image);
    void (*set_transform)(void *render, float *t, int reset);
    void (*get_transform)(void *render, float *t);
} render;

NVGcolor nvgColorU32(uint32_t c);
NVGcolor transformColor(NVGcolor color, LVGColorTransform *x);
int LinearGradientStops(NSVGgradient *gradient, LVGColorTransform *x);
int RadialGradientStops(NSVGgradient *gradient, LVGColorTransform *x);
void gl_free_image(int image);

typedef float Transform3x2[2][3];

void identity(Transform3x2 dst);
void mul(Transform3x2 dst, Transform3x2 a, Transform3x2 b);
void translate(Transform3x2 dst, float x, float y);
void scale(Transform3x2 dst, float x, float y);
void rotate(Transform3x2 dst, float angle);
void xform(float dst[2], Transform3x2 a, const float v[2]);
void inverse(Transform3x2 dst, Transform3x2 data);
