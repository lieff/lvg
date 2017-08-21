#pragma once
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "../nanovg/nanosvg.h"
#include "../nanovg/nanovg.h"
#include "lvg_header.h"

typedef struct render
{
    int (*init)(void **render);
    void (*release)(void *render);
    int (*cache_shape)(void *render, NSVGshape *shape);
    int (*cache_image)(void *render, int width, int height, const void *rgba);
    void (*update_image)(void *render, int image, const void *rgba);
    void (*render_shape)(void *render, NSVGshape *shape, LVGObject *o);
} render;
