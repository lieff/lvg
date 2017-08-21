#include "render.h"

static int nvpr_init(void **render)
{
    return 1;
}

static void nvpr_release(void *render)
{
}

static void nvpr_begin_frame(void *render, LVGMovieClip *clip, int winWidth, int winHeight, int width, int height)
{

}

static void nvpr_end_frame(void *render)
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

static void nvpr_render_image(void *render, int image)
{
}

static void nvpr_set_transform(void *render, float *t, int reset)
{
}

static void nvpr_get_transform(void *render, float *t)
{
}


const render nvpr_render =
{
    nvpr_init,
    nvpr_release,
    nvpr_begin_frame,
    nvpr_end_frame,
    nvpr_cache_shape,
    nvpr_cache_image,
    nvpr_update_image,
    nvpr_render_shape,
    nvpr_render_image,
    nvpr_set_transform,
    nvpr_get_transform
};
