#pragma once
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "../nanovg/nanosvg.h"
#include "../nanovg/nanovg.h"
#include "lvg_header.h"
#include <platform/platform.h>

#define IMAGE_REPEAT 1
#define GRADIENT_SAMPLES_L 256
#define GRADIENT_SAMPLES_R 256

#define BLEND_REPLACE    0
#define BLEND_LAYER      1
#define BLEND_MULTIPLY   2
#define BLEND_SCREEN     3
#define BLEND_LIGHTEN    4
#define BLEND_DARKEN     5
#define BLEND_DIFFERENCE 6
#define BLEND_ADD        7
#define BLEND_SUBTRACT   8
#define BLEND_INVERT     9
#define BLEND_ALPHA      10
#define BLEND_ERASE      11
#define BLEND_OVERLAY    12
#define BLEND_HARDLIGHT  13

typedef struct render
{
    int (*init)(void **render, const platform *platform);
    void (*release)(void *render);
    void (*begin_frame)(void *render, int viewportWidth, int viewportHeight, int winWidth, int winHeight, int width, int height);
    void (*end_frame)(void *render);
    int (*cache_shape)(void *render, NSVGshape *shape);
    int (*cache_image)(void *render, int width, int height, int flags, const void *rgba);
    int (*cache_gradient)(void *render, NSVGpaint *fill);
    void (*free_image)(void *render, int image);
    void (*update_image)(void *render, int image, const void *rgba);
    void (*render_shape)(void *render, LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode);
    void (*render_image)(void *render, int image);
    void (*set_transform)(void *render, float *t, int reset);
    void (*get_transform)(void *render, float *t);
    int (*inside_shape)(void *render, NSVGshape *shape, float x, float y);
} render;

NVGcolor nvgColorU32(uint32_t c);
NVGcolor transformColor(NVGcolor color, LVGColorTransform *x);
int LinearGradientStops(const render *render, void *render_obj, NSVGgradient *gradient, LVGColorTransform *x);
int RadialGradientStops(const render *render, void *render_obj, NSVGgradient *gradient, LVGColorTransform *x);
void gl_free_image(void *render, int image);

typedef float Transform3x2[2][3];

void identity(Transform3x2 dst);
void mul(Transform3x2 dst, Transform3x2 a, Transform3x2 b);
void translate(Transform3x2 dst, float x, float y);
void scale(Transform3x2 dst, float x, float y);
void rotate(Transform3x2 dst, float angle);
void xform(float dst[2], Transform3x2 a, const float v[2]);
void inverse(Transform3x2 dst, Transform3x2 data);
void to_transform3x2(Transform3x2 tr, float *t);
void from_transform3x2(float *t, Transform3x2 tr);
