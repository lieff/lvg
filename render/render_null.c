#include "render.h"

static int null_init(void **render, const platform *platform)
{
    *render = 0;
    return 1;
}

static void null_release(void *render)
{
}

static void null_begin_frame(void *render, int viewportWidth, int viewportHeight, int winWidth, int winHeight, int width, int height)
{
}

static void null_end_frame(void *render)
{
}

static int null_cache_shape(void *render, NSVGshape *shape)
{
    return 1;
}

static int null_cache_image(void *render, int width, int height, int flags, const void *rgba)
{
    return 1;
}

static int null_cache_gradient(void *render, NSVGpaint *fill)
{
    return 1;
}

static void null_free_image(void *render, int image)
{
}

static void null_update_image(void *render, int image, const void *rgba)
{
}

static void null_render_shape(void *render, LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode)
{
}

static void null_render_image(void *render, int image)
{
}

static void null_set_transform(void *render, float *t, int reset)
{
}

static void null_get_transform(void *render, float *t)
{
}

const render null_render =
{
    null_init,
    null_release,
    null_begin_frame,
    null_end_frame,
    null_cache_shape,
    null_cache_image,
    null_cache_gradient,
    null_free_image,
    null_update_image,
    null_render_shape,
    null_render_image,
    null_set_transform,
    null_get_transform,
    0
};
