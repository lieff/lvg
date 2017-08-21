#include "render.h"

static int nvpr_init(void **render)
{
    return 1;
}

static void nvpr_release(void *render)
{
}

static int nvpr_cache_shape(void *render, NSVGshape *shape)
{
    return 1;
}

static int nvpr_cache_image(void *render, int width, int height, const void *rgba)
{
    return 1;
}

static void nvpr_update_image(void *render, int image, const void *rgba)
{
}

static void nvpr_render_shape(void *render, NSVGshape *shape, LVGObject *o)
{
}

const render nvpr_render =
{
    nvpr_init,
    nvpr_release,
    nvpr_cache_shape,
    nvpr_cache_image,
    nvpr_update_image,
    nvpr_render_shape
};
