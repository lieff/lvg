#define NANOSVG_IMPLEMENTATION
#ifdef EMSCRIPTEN
#define NANOVG_GLES2_IMPLEMENTATION
#else
#define NANOVG_GL2_IMPLEMENTATION
#endif
#include "gl.h"
#include "render.h"
#include "nanovg_gl.h"
#include <assert.h>

static void nvgSVGLinearGrad(struct NVGcontext *vg, struct NSVGshape *shape, LVGColorTransform *cxform, int is_fill)
{
    NSVGgradient *gradient = is_fill ? shape->fill.gradient : shape->stroke.gradient;
    float *xf = gradient->xform;
    Transform3x2 data = { { xf[0], xf[2], 0.f },
                          { xf[1], xf[3], 0.f } };
    Transform3x2 tr;
    inverse(data, data);
    scale(tr, 20.0/32768.0, 20.0/32768.0); // swf gradients -16384..16384 square in twips
    mul(data, tr, data);
    translate(tr, 128, 128);
    mul(data, tr, data);
    inverse(data, data);
    scale(tr, 1.0/256, 1.0/256);
    mul(data, tr, data);

    NVGpaint p;
    memset(&p, 0, sizeof(p));
    p.xform[0] = data[0][0];
    p.xform[1] = data[1][0];
    p.xform[2] = data[0][1];
    p.xform[3] = data[1][1];
    p.xform[4] = data[0][2] + xf[4];
    p.xform[5] = data[1][2] + xf[5];
    p.image = gradient->cache;
    p.innerColor = p.outerColor = transformColor(nvgRGBAf(1,1,1,1), cxform);
    p.extent[0] = 256;
    p.extent[1] = 256;
    p.feather = 0;

    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

static void nvgSVGRadialGrad(struct NVGcontext *vg, struct NSVGshape *shape, LVGColorTransform *cxform, int is_fill)
{
    NSVGgradient *gradient = is_fill ? shape->fill.gradient : shape->stroke.gradient;
    float *xf = gradient->xform;
    Transform3x2 data = { { xf[0], xf[2], 0.f },
                          { xf[1], xf[3], 0.f } };
    Transform3x2 tr;
    inverse(data, data);
    scale(tr, 20.0/32768.0, 20.0/32768.0); // swf gradients -16384..16384 square in twips
    mul(data, tr, data);
    translate(tr, 128, 128);
    mul(data, tr, data);
    inverse(data, data);
    scale(tr, 1.0/256, 1.0/256);
    mul(data, tr, data);

    NVGpaint p;
    memset(&p, 0, sizeof(p));
    p.xform[0] = data[0][0];
    p.xform[1] = data[1][0];
    p.xform[2] = data[0][1];
    p.xform[3] = data[1][1];
    p.xform[4] = data[0][2] + xf[4];
    p.xform[5] = data[1][2] + xf[5];
    p.image = gradient->cache;
    p.innerColor = p.outerColor = transformColor(nvgRGBAf(1,1,1,1), cxform);
    p.extent[0] = 256;
    p.extent[1] = 256;
    p.feather = 0;

    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

static void ImagePaint(struct NVGcontext *vg, struct NSVGshape *shape, LVGColorTransform *cxform, int is_fill)
{
    NSVGpaint *sp = is_fill ? &shape->fill : &shape->stroke;
    GLNVGcontext* gl = (GLNVGcontext*)nvgInternalParams(vg)->userPtr;
    GLNVGtexture* tex = glnvg__findTexture(gl, sp->color);
    if (NSVG_SPREAD_PAD == sp->spread)
        tex->flags &= ~(NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
    else
        tex->flags |= NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY;
    //if (sp->filtered) TODO
    int tw = tex->width, th = tex->height;
    //glBindTexture(GL_TEXTURE_2D, tex->tex);
    //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
    //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
    //glBindTexture(GL_TEXTURE_2D, 0);
    float *xf = sp->xform;
    GLfloat data[2][3] = { { xf[0], xf[2], xf[4] },
                           { xf[1], xf[3], xf[5] } };
    Transform3x2 tr;
    inverse(data, data);
    scale(tr, 20.0/tw, 20.0/th);
    mul(data, tr, data);
    inverse(data, data);
    scale(tr, 1.0/256, 1.0/256);
    mul(data, tr, data);

    NVGpaint p;
    memset(&p, 0, sizeof(p));
    p.xform[0] = data[0][0];
    p.xform[1] = data[1][0];
    p.xform[2] = data[0][1];
    p.xform[3] = data[1][1];
    p.xform[4] = xf[4];
    p.xform[5] = xf[5];
    p.image = sp->color;
    p.innerColor = p.outerColor = transformColor(nvgRGBAf(1,1,1,1), cxform);
    p.extent[0] = 256;
    p.extent[1] = 256;
    p.feather = 0;

    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

static void nvgDrawShape(NVGcontext *vg, LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode)
{
    int i, j;
    NSVGpath *path, *path2;
    float om_ratio = 1.0f - ratio;
    for (j = 0; j < shapecol->num_shapes; j++)
    {
        NSVGshape *shape = shapecol->shapes + j;
        NSVGshape *shape2 = shapecol->morph ? shapecol->morph->shapes + j : 0;
        nvgBeginPath(vg);
        path2 = shape2 ? shape2->paths : 0;
        for (path = shape->paths; path != NULL; path = path->next)
        {
            if (NSVG_PAINT_NONE == shape->stroke.type && (NSVG_PAINT_NONE == shape->fill.type || (NSVG_PAINT_NONE != shape->fill.type && !path->closed)))
            {
                if (path2)
                    path2 = path2->next;
                continue;
            }
            int l = path->npts - 1;
            //l = (int)(l*g_time*0.4) % l;
            if (path2)
            {
                assert(path->npts == path2->npts);
                nvgMoveTo(vg, path->pts[0]*om_ratio + path2->pts[0]*ratio, path->pts[1]*om_ratio + path2->pts[1]*ratio);
                for (i = 0; i < l; i += 3)
                {
                    float *p  = &path->pts[i*2];
                    float *p2 = &path2->pts[i*2];
                    nvgBezierTo(vg, p[2]*om_ratio + p2[2]*ratio, p[3]*om_ratio + p2[3]*ratio, p[4]*om_ratio + p2[4]*ratio,
                            p[5]*om_ratio + p2[5]*ratio, p[6]*om_ratio + p2[6]*ratio, p[7]*om_ratio + p2[7]*ratio);
                }
                if (path->closed)
                    nvgClosePath(vg);
                path2 = path2->next;
            } else
            {
                nvgMoveTo(vg, path->pts[0], path->pts[1]);
                for (i = 0; i < l; i += 3)
                {
                    float *p = &path->pts[i*2];
                    nvgBezierTo(vg, p[2], p[3], p[4], p[5], p[6], p[7]);
                }
                if (path->closed)
                    nvgClosePath(vg);
            }
        }
        if (NSVG_PAINT_NONE != shape->fill.type)
        {
            if (NSVG_PAINT_COLOR == shape->fill.type)
                nvgFillColor(vg, transformColor(nvgColorU32(shape->fill.color), cxform));
            else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
                nvgSVGLinearGrad(vg, shape, cxform, 1);
            else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
                nvgSVGRadialGrad(vg, shape, cxform, 1);
            else if (NSVG_PAINT_IMAGE == shape->fill.type)
                ImagePaint(vg, shape, cxform, 1);
            nvgFill(vg);
        }
        if (NSVG_PAINT_NONE != shape->stroke.type)
        {
            if (NSVG_PAINT_COLOR == shape->stroke.type)
                nvgStrokeColor(vg, transformColor(nvgColorU32(shape->stroke.color), cxform));
            else if (NSVG_PAINT_LINEAR_GRADIENT == shape->stroke.type)
                nvgSVGLinearGrad(vg, shape, cxform, 0);
            else if (NSVG_PAINT_RADIAL_GRADIENT == shape->stroke.type)
                nvgSVGRadialGrad(vg, shape, cxform, 0);
            nvgStrokeWidth(vg, shape->strokeWidth);
            nvgLineJoin(vg, shape->strokeLineJoin);
            nvgLineCap(vg, shape->strokeLineCap);
            nvgMiterLimit(vg, shape->miterLimit);
            nvgStroke(vg);
        }
    }
}


static int nvg_init(void **render, const platform *platform)
{
#ifdef EMSCRIPTEN
    *render = nvgCreateGLES2(0);
#else
    *render = nvgCreateGL2(/*NVG_ANTIALIAS | NVG_STENCIL_STROKES*/0
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

static void nvg_begin_frame(void *render, int viewportWidth, int viewportHeight, int winWidth, int winHeight, int width, int height)
{
    NVGcontext *vg = render;
    nvgBeginFrame(vg, winWidth, winHeight, (float)width / (float)winWidth);
    float scalex = (float)width/viewportWidth;
    float scaley = (float)height/viewportHeight;
    float scale = scalex < scaley ? scalex : scaley;

    nvgTranslate(vg, -(viewportWidth*scale - width)/2, -(viewportHeight*scale - height)/2);
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
    return nvgCreateImageRGBA(vg, width, height,
#ifndef EMSCRIPTEN
        NVG_IMAGE_GENERATE_MIPMAPS |
#endif
        ((flags & IMAGE_REPEAT) ? (NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY) : 0), (const unsigned char *)rgba);
}

static int nvg_cache_gradient(void *render, NSVGpaint *fill)
{
    int img = (NSVG_PAINT_LINEAR_GRADIENT == fill->type) ? LinearGradientStops(fill->gradient, 0) : RadialGradientStops(fill->gradient, 0);
    fill->gradient->cache = img;
    return img;
}

static void nvg_update_image(void *render, int image, const void *rgba)
{
    NVGcontext *vg = render;
    nvgUpdateImage(vg, image, rgba);
}

static void nvg_render_shape(void *render, LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode)
{
    NVGcontext *vg = render;
    nvgDrawShape(vg, shapecol, cxform, ratio, blend_mode);
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
    gl_free_image,
    nvg_update_image,
    nvg_render_shape,
    nvg_render_image,
    nvg_set_transform,
    nvg_get_transform,
    0
};
