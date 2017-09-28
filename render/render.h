#pragma once
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "../nanovg/nanosvg.h"
#include "../nanovg/nanovg.h"
#include "lvg_header.h"

#define IMAGE_REPEAT 1
#define GRADIENT_SAMPLES_L 256
#define GRADIENT_SAMPLES_R 256

#define BLEND_REPLACE    0
#define BLEND_layer      1
#define BLEND_multiply   2
#define BLEND_screen     3
#define BLEND_lighten    4
#define BLEND_darken     5
#define BLEND_difference 6
#define BLEND_add        7
#define BLEND_subtract   8
#define BLEND_invert     9
#define BLEND_alpha      10
#define BLEND_erase      11
#define BLEND_overlay    12
#define BLEND_hardlight  13


typedef struct render
{
    int (*init)(void **render);
    void (*release)(void *render);
    void (*begin_frame)(void *render, LVGMovieClip *clip, int winWidth, int winHeight, int width, int height);
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
int LinearGradientStops(NSVGgradient *gradient, LVGColorTransform *x);
int RadialGradientStops(NSVGgradient *gradient, LVGColorTransform *x);
void gl_free_image(void *render, int image);

typedef float Transform3x2[2][3];

void identity(Transform3x2 dst);
void mul(Transform3x2 dst, Transform3x2 a, Transform3x2 b);
void translate(Transform3x2 dst, float x, float y);
void scale(Transform3x2 dst, float x, float y);
void rotate(Transform3x2 dst, float angle);
void xform(float dst[2], Transform3x2 a, const float v[2]);
void inverse(Transform3x2 dst, Transform3x2 data);
