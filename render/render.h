#pragma once
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "../nanovg/nanosvg.h"
#include "../nanovg/nanovg.h"
#include "lvg_header.h"

#define IMAGE_REPEAT 1

typedef struct render
{
    int (*init)(void **render);
    void (*release)(void *render);
    void (*begin_frame)(void *render, LVGMovieClip *clip, int winWidth, int winHeight, int width, int height);
    void (*end_frame)(void *render);
    int (*cache_shape)(void *render, NSVGshape *shape);
    int (*cache_image)(void *render, int width, int height, int flags, const void *rgba);
    void (*update_image)(void *render, int image, const void *rgba);
    void (*render_shape)(void *render, NSVGshape *shape, LVGObject *o);
    void (*render_image)(void *render, int image);
    void (*set_transform)(void *render, float *t, int reset);
    void (*get_transform)(void *render, float *t);
} render;

NVGcolor nvgColorU32(uint32_t c);
NVGcolor transformColor(NVGcolor color, LVGObject *o);
int LinearGradientStops(struct NVGcontext *vg, NSVGshape *shape, LVGObject *o);
