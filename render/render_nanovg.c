#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#define NANOSVG_IMPLEMENTATION
#ifdef EMSCRIPTEN
#define NANOVG_GLES2_IMPLEMENTATION
#else
#define NANOVG_GL2_IMPLEMENTATION
#endif
#include "render.h"
#include "nanovg_gl.h"

static int nvgSVGLinearGrad(struct NVGcontext *vg, struct NSVGshape *shape, LVGObject *o, int is_fill)
{
    NSVGgradient *gradient = is_fill ? shape->fill.gradient : shape->stroke.gradient;
    /*float sx = shape->bounds[0];
    float sy = shape->bounds[1];
    float ex = shape->bounds[0];
    float ey = shape->bounds[3];*/
    float w = shape->bounds[2] - shape->bounds[0];
    float h = shape->bounds[3] - shape->bounds[1];
    /*NVGcolor cs = transformColor(nvgColorU32(grad->stops[0].color), o);
    NVGcolor ce = transformColor(nvgColorU32(grad->stops[grad->nstops - 1].color), o);

    NVGpaint p = nvgLinearGradient(vg, sx, sy, ex, ey, cs, ce);*/

    float *xf = gradient->xform;
    Transform3x2 data = { { xf[0], xf[2], xf[4] },
                          { xf[1], xf[3], xf[5] } };
    //float p1[2] = { shape->bounds[0], shape->bounds[1] };
    //float p2[2] = { shape->bounds[2], shape->bounds[3] };
    Transform3x2 tr;
    inverse(data, data);
    //rotate(tr, mx);
    //mul(data, tr, data);
    scale(tr, 20.0/32768.0, 20.0/32768.0); // swf gradients -16384..16384 square in twips
    mul(data, tr, data);
    translate(tr, 128, 128);
    mul(data, tr, data);
    //xform(p1, data, p1);
    //xform(p2, data, p2);
    inverse(data, data);
    scale(tr, 1.0/256, 1.0/256);
    mul(data, tr, data);

    NVGpaint p;
    memset(&p, 0, sizeof(p));
    p.xform[0] = data[0][0];
    p.xform[1] = data[1][0];
    p.xform[2] = data[0][1];
    p.xform[3] = data[1][1];
    p.xform[4] = data[0][2] + shape->bounds[0] + w*0.5;
    p.xform[5] = data[1][2] + shape->bounds[1] + h*0.5;
    p.image = gradient->cache;
    p.innerColor = p.outerColor = nvgRGBAf(1,1,1,1);
    p.extent[0] = 256;
    p.extent[1] = 256;
    p.feather = 0;

    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
    return gradient->cache;
}

static void nvgSVGRadialGrad(struct NVGcontext *vg, struct NSVGshape *shape, LVGObject *o, int is_fill)
{
    NSVGgradient *grad = is_fill ? shape->fill.gradient : shape->stroke.gradient;
    float cx = (shape->bounds[0] + shape->bounds[2])/2.0;
    float cy = (shape->bounds[1] + shape->bounds[3])/2.0;
    NVGcolor cs = transformColor(nvgColorU32(grad->stops[0].color), o);
    NVGcolor ce = transformColor(nvgColorU32(grad->stops[grad->nstops - 1].color), o);

    NVGpaint p = nvgRadialGradient(vg, cx, cy, 0, cx, cs, ce);
    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

static void nvgDrawShape(NVGcontext *vg, NSVGshape *shape, LVGObject *o)
{
    int i;
    NSVGpath *path;
    nvgBeginPath(vg);
    for (path = shape->paths; path != NULL; path = path->next)
    {
        nvgMoveTo(vg, path->pts[0], path->pts[1]);
        int l = path->npts - 1;
        //l = (int)(l*g_time*0.4) % l;
        for (i = 0; i < l; i += 3)
        {
            float *p = &path->pts[i*2];
            nvgBezierTo(vg, p[2], p[3], p[4], p[5], p[6], p[7]);
        }
        if (path->closed)
            nvgLineTo(vg, path->pts[0], path->pts[1]);
    }
    if (NSVG_PAINT_NONE != shape->fill.type)
    {
        GLuint tex = 0;
        if (NSVG_PAINT_COLOR == shape->fill.type)
            nvgFillColor(vg, transformColor(nvgColorU32(shape->fill.color), o));
        else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
            tex = nvgSVGLinearGrad(vg, shape, o, 1);
        else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
            nvgSVGRadialGrad(vg, shape, o, 1);
        else if (NSVG_PAINT_IMAGE == shape->fill.type)
        {
            int w = shape->bounds[2] - shape->bounds[0], h = shape->bounds[3] - shape->bounds[1];
            NVGpaint imgPaint = nvgImagePattern(vg, shape->bounds[0], shape->bounds[1], w, h, 0, shape->fill.color, 1.0f);
            nvgFillPaint(vg, imgPaint);
        }
        if (NSVG_FILLRULE_EVENODD == shape->fillRule)
            nvgPathWinding(vg, NVG_HOLE);
        nvgFill(vg);
        glFlush();
        if (tex)
            glDeleteTextures(1, &tex);
    }
    if (NSVG_PAINT_NONE != shape->stroke.type)
    {
        if (NSVG_PAINT_COLOR == shape->stroke.type)
            nvgStrokeColor(vg, transformColor(nvgColorU32(shape->stroke.color), o));
        else if (NSVG_PAINT_LINEAR_GRADIENT == shape->stroke.type)
            nvgSVGLinearGrad(vg, shape, o, 0);
        else if (NSVG_PAINT_RADIAL_GRADIENT == shape->stroke.type)
            nvgSVGRadialGrad(vg, shape, o, 0);
        nvgStrokeWidth(vg, shape->strokeWidth);
        nvgStroke(vg);
    }
}


static int nvg_init(void **render)
{
#ifdef EMSCRIPTEN
    *render = nvgCreateGLES2(/*NVG_ANTIALIAS | NVG_STENCIL_STROKES*/0);
#else
    *render = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES
#ifdef DEBUG
        | NVG_DEBUG
#endif
        );
#endif
    return 1;
}

static void nvg_release(void *render)
{
    NVGcontext *vg = render;
#ifdef EMSCRIPTEN
    nvgDeleteGLES2(vg);
#else
    nvgDeleteGL2(vg);
#endif
}

static void nvg_begin_frame(void *render, LVGMovieClip *clip, int winWidth, int winHeight, int width, int height)
{
    NVGcontext *vg = render;
    nvgBeginFrame(vg, winWidth, winHeight, (float)width / (float)winWidth);
    if (!clip)
        return;
    float clip_w = clip->bounds[2] - clip->bounds[0], clip_h = clip->bounds[3] - clip->bounds[1];
    float scalex = width/clip_w;
    float scaley = height/clip_h;
    float scale = scalex < scaley ? scalex : scaley;

    nvgTranslate(vg, -(clip_w*scale - width)/2, -(clip_h*scale - height)/2);
    nvgScale(vg, scale, scale);
}

static void nvg_end_frame(void *render)
{
    NVGcontext *vg = render;
    nvgEndFrame(vg);
}

static int nvg_cache_shape(void *render, NSVGshape *shape)
{
    return 1;
}

static int nvg_cache_image(void *render, int width, int height, int flags, const void *rgba)
{
    NVGcontext *vg = render;
    return nvgCreateImageRGBA(vg, width, height, (flags & IMAGE_REPEAT) ? (NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY) : 0, (const unsigned char *)rgba);
}

static int nvg_cache_gradient(NSVGpaint *fill)
{
    if (NSVG_PAINT_RADIAL_GRADIENT == fill->type)
        return 0;
    int img = (NSVG_PAINT_LINEAR_GRADIENT == fill->type) ? LinearGradientStops(fill->gradient, 0) : RadialGradientStops(fill->gradient, 0);
    fill->gradient->cache = img;
    return img;
}

static void nvg_update_image(void *render, int image, const void *rgba)
{
    NVGcontext *vg = render;
    nvgUpdateImage(vg, image, rgba);
}

static void nvg_render_shape(void *render, NSVGshape *shape, LVGObject *o)
{
    NVGcontext *vg = render;
    nvgDrawShape(vg, shape, o);
}

static void nvg_render_image(void *render, int image)
{
    NVGcontext *vg = render;
    int w, h;
    nvgImageSize(vg, image, &w, &h);
    NVGpaint imgPaint = nvgImagePattern(vg, 0, 0, w, h, 0, image, 1.0f);
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, w, h);
    nvgFillPaint(vg, imgPaint);
    nvgFill(vg);
}

static void nvg_set_transform(void *render, float *t, int reset)
{
    NVGcontext *vg = render;
    if (reset)
        nvgResetTransform(vg);
    nvgTransform(vg, t[0], t[1], t[2], t[3], t[4], t[5]);
}

static void nvg_get_transform(void *render, float *t)
{
    NVGcontext *vg = render;
    nvgCurrentTransform(vg, t);
}

const render nvg_render =
{
    nvg_init,
    nvg_release,
    nvg_begin_frame,
    nvg_end_frame,
    nvg_cache_shape,
    nvg_cache_image,
    nvg_cache_gradient,
    nvg_update_image,
    nvg_render_shape,
    nvg_render_image,
    nvg_set_transform,
    nvg_get_transform
};
