#include "gl.h"
#include "render.h"
#include <stdlib.h>
#include <string.h>

#define FUNC(x) x

PFNGLGENPATHSNVPROC FUNC(glGenPathsNV) = NULL;
PFNGLDELETEPATHSNVPROC FUNC(glDeletePathsNV) = NULL;
PFNGLISPATHNVPROC FUNC(glIsPathNV) = NULL;
PFNGLPATHCOMMANDSNVPROC FUNC(glPathCommandsNV) = NULL;
PFNGLPATHCOORDSNVPROC FUNC(glPathCoordsNV) = NULL;
PFNGLPATHSUBCOMMANDSNVPROC FUNC(glPathSubCommandsNV) = NULL;
PFNGLPATHSUBCOORDSNVPROC FUNC(glPathSubCoordsNV) = NULL;
PFNGLPATHSTRINGNVPROC FUNC(glPathStringNV) = NULL;
PFNGLPATHGLYPHSNVPROC FUNC(glPathGlyphsNV) = NULL;
PFNGLPATHGLYPHRANGENVPROC FUNC(glPathGlyphRangeNV) = NULL;
PFNGLWEIGHTPATHSNVPROC FUNC(glWeightPathsNV) = NULL;
PFNGLCOPYPATHNVPROC FUNC(glCopyPathNV) = NULL;
PFNGLINTERPOLATEPATHSNVPROC FUNC(glInterpolatePathsNV) = NULL;
PFNGLTRANSFORMPATHNVPROC FUNC(glTransformPathNV) = NULL;
PFNGLPATHPARAMETERIVNVPROC FUNC(glPathParameterivNV) = NULL;
PFNGLPATHPARAMETERINVPROC FUNC(glPathParameteriNV) = NULL;
PFNGLPATHPARAMETERFVNVPROC FUNC(glPathParameterfvNV) = NULL;
PFNGLPATHPARAMETERFNVPROC FUNC(glPathParameterfNV) = NULL;
PFNGLPATHDASHARRAYNVPROC FUNC(glPathDashArrayNV) = NULL;
PFNGLSTENCILFILLPATHNVPROC FUNC(glStencilFillPathNV) = NULL;
PFNGLPATHSTENCILDEPTHOFFSETNVPROC FUNC(glPathStencilDepthOffsetNV) = NULL;
PFNGLSTENCILSTROKEPATHNVPROC FUNC(glStencilStrokePathNV) = NULL;
PFNGLSTENCILFILLPATHINSTANCEDNVPROC FUNC(glStencilFillPathInstancedNV) = NULL;
PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC FUNC(glStencilStrokePathInstancedNV) = NULL;
PFNGLPATHCOVERDEPTHFUNCNVPROC FUNC(glPathCoverDepthFuncNV) = NULL;
PFNGLPATHCOLORGENNVPROC FUNC(glPathColorGenNV) = NULL;
PFNGLPATHTEXGENNVPROC FUNC(glPathTexGenNV) = NULL;
PFNGLPATHFOGGENNVPROC FUNC(glPathFogGenNV) = NULL;
PFNGLCOVERFILLPATHNVPROC FUNC(glCoverFillPathNV) = NULL;
PFNGLCOVERSTROKEPATHNVPROC FUNC(glCoverStrokePathNV) = NULL;
PFNGLCOVERFILLPATHINSTANCEDNVPROC FUNC(glCoverFillPathInstancedNV) = NULL;
PFNGLCOVERSTROKEPATHINSTANCEDNVPROC FUNC(glCoverStrokePathInstancedNV) = NULL;
PFNGLGETPATHPARAMETERIVNVPROC FUNC(glGetPathParameterivNV) = NULL;
PFNGLGETPATHPARAMETERFVNVPROC FUNC(glGetPathParameterfvNV) = NULL;
PFNGLGETPATHCOMMANDSNVPROC FUNC(glGetPathCommandsNV) = NULL;
PFNGLGETPATHCOORDSNVPROC FUNC(glGetPathCoordsNV) = NULL;
PFNGLGETPATHDASHARRAYNVPROC FUNC(glGetPathDashArrayNV) = NULL;
PFNGLGETPATHMETRICSNVPROC FUNC(glGetPathMetricsNV) = NULL;
PFNGLGETPATHMETRICRANGENVPROC FUNC(glGetPathMetricRangeNV) = NULL;
PFNGLGETPATHSPACINGNVPROC FUNC(glGetPathSpacingNV) = NULL;
PFNGLGETPATHCOLORGENIVNVPROC FUNC(glGetPathColorGenivNV) = NULL;
PFNGLGETPATHCOLORGENFVNVPROC FUNC(glGetPathColorGenfvNV) = NULL;
PFNGLGETPATHTEXGENIVNVPROC FUNC(glGetPathTexGenivNV) = NULL;
PFNGLGETPATHTEXGENFVNVPROC FUNC(glGetPathTexGenfvNV) = NULL;
PFNGLISPOINTINFILLPATHNVPROC FUNC(glIsPointInFillPathNV) = NULL;
PFNGLISPOINTINSTROKEPATHNVPROC FUNC(glIsPointInStrokePathNV) = NULL;
PFNGLGETPATHLENGTHNVPROC FUNC(glGetPathLengthNV) = NULL;
PFNGLPOINTALONGPATHNVPROC FUNC(glPointAlongPathNV) = NULL;
PFNGLPATHSTENCILFUNCNVPROC FUNC(glPathStencilFuncNV) = NULL;

typedef void (APIENTRYP PFNGLMATRIXLOADIDENTITYEXTPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLMATRIXORTHOEXTPROC) (GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void (APIENTRYP PFNGLMATRIXLOADFEXTPROC) (GLenum mode, const GLfloat *m);
PFNGLMATRIXLOADIDENTITYEXTPROC glMatrixLoadIdentityEXT = NULL;
PFNGLMATRIXORTHOEXTPROC glMatrixOrthoEXT = NULL;
PFNGLMATRIXLOADFEXTPROC glMatrixLoadfEXT = NULL;

#define LOAD_PROC(type, name) \
  FUNC(name) = (type) platform->get_proc_address(#name); \
  if (!FUNC(name)) { \
    fail = 1; \
  }

typedef struct render_ctx
{
    Transform3x2 transform;
    int winWidth, winHeight, width, height;
} render_ctx;

void MatrixLoadToGL(Transform3x2 m)
{
    GLfloat mm[16];
    mm[0] = m[0][0];
    mm[1] = m[1][0];
    mm[2] = 0;
    mm[3] = 0;
    mm[4] = m[0][1];
    mm[5] = m[1][1];
    mm[6] = 0;
    mm[7] = 0;
    mm[8] = 0;
    mm[9] = 0;
    mm[10] = 1;
    mm[11] = 0;
    mm[12] = m[0][2];
    mm[13] = m[1][2];
    mm[14] = 0;
    mm[15] = 1;
    glMatrixLoadfEXT(GL_MODELVIEW, &mm[0]);
}

static int nvpr_init(void **render, const platform *platform)
{
    render_ctx *ctx = calloc(1, sizeof(render_ctx));
    *render = ctx;
    if (!ctx)
        return 0;
    int fail = 0;
    if (!platform->extension_supported || !platform->extension_supported("GL_NV_path_rendering"))
        goto error;

    LOAD_PROC(PFNGLMATRIXLOADIDENTITYEXTPROC, glMatrixLoadIdentityEXT);
    LOAD_PROC(PFNGLMATRIXORTHOEXTPROC, glMatrixOrthoEXT);
    LOAD_PROC(PFNGLMATRIXLOADFEXTPROC, glMatrixLoadfEXT);

    LOAD_PROC(PFNGLGENPATHSNVPROC, glGenPathsNV);
    LOAD_PROC(PFNGLDELETEPATHSNVPROC, glDeletePathsNV);
    LOAD_PROC(PFNGLISPATHNVPROC, glIsPathNV);
    LOAD_PROC(PFNGLPATHCOMMANDSNVPROC, glPathCommandsNV);
    LOAD_PROC(PFNGLPATHCOORDSNVPROC, glPathCoordsNV);
    LOAD_PROC(PFNGLPATHSUBCOMMANDSNVPROC, glPathSubCommandsNV);
    LOAD_PROC(PFNGLPATHSUBCOORDSNVPROC, glPathSubCoordsNV);
    LOAD_PROC(PFNGLPATHSTRINGNVPROC, glPathStringNV);
    LOAD_PROC(PFNGLPATHGLYPHSNVPROC, glPathGlyphsNV);
    LOAD_PROC(PFNGLPATHGLYPHRANGENVPROC, glPathGlyphRangeNV);
    LOAD_PROC(PFNGLWEIGHTPATHSNVPROC, glWeightPathsNV);
    LOAD_PROC(PFNGLCOPYPATHNVPROC, glCopyPathNV);
    LOAD_PROC(PFNGLINTERPOLATEPATHSNVPROC, glInterpolatePathsNV);
    LOAD_PROC(PFNGLTRANSFORMPATHNVPROC, glTransformPathNV);
    LOAD_PROC(PFNGLPATHPARAMETERIVNVPROC, glPathParameterivNV);
    LOAD_PROC(PFNGLPATHPARAMETERINVPROC, glPathParameteriNV);
    LOAD_PROC(PFNGLPATHPARAMETERFVNVPROC, glPathParameterfvNV);
    LOAD_PROC(PFNGLPATHPARAMETERFNVPROC, glPathParameterfNV);
    LOAD_PROC(PFNGLPATHDASHARRAYNVPROC, glPathDashArrayNV);
    LOAD_PROC(PFNGLSTENCILFILLPATHNVPROC, glStencilFillPathNV);
    LOAD_PROC(PFNGLSTENCILSTROKEPATHNVPROC, glStencilStrokePathNV);
    LOAD_PROC(PFNGLSTENCILFILLPATHINSTANCEDNVPROC, glStencilFillPathInstancedNV);
    LOAD_PROC(PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC, glStencilStrokePathInstancedNV);
    LOAD_PROC(PFNGLPATHCOLORGENNVPROC, glPathColorGenNV);
    LOAD_PROC(PFNGLPATHTEXGENNVPROC, glPathTexGenNV);
    LOAD_PROC(PFNGLPATHFOGGENNVPROC, glPathFogGenNV);
    LOAD_PROC(PFNGLCOVERFILLPATHNVPROC, glCoverFillPathNV);
    LOAD_PROC(PFNGLCOVERSTROKEPATHNVPROC, glCoverStrokePathNV);
    LOAD_PROC(PFNGLCOVERFILLPATHINSTANCEDNVPROC, glCoverFillPathInstancedNV);
    LOAD_PROC(PFNGLCOVERSTROKEPATHINSTANCEDNVPROC, glCoverStrokePathInstancedNV);
    LOAD_PROC(PFNGLGETPATHPARAMETERIVNVPROC, glGetPathParameterivNV);
    LOAD_PROC(PFNGLGETPATHPARAMETERFVNVPROC, glGetPathParameterfvNV);
    LOAD_PROC(PFNGLGETPATHCOMMANDSNVPROC, glGetPathCommandsNV);
    LOAD_PROC(PFNGLGETPATHCOORDSNVPROC, glGetPathCoordsNV);
    LOAD_PROC(PFNGLGETPATHDASHARRAYNVPROC, glGetPathDashArrayNV);
    LOAD_PROC(PFNGLGETPATHMETRICSNVPROC, glGetPathMetricsNV);
    LOAD_PROC(PFNGLGETPATHMETRICRANGENVPROC, glGetPathMetricRangeNV);
    LOAD_PROC(PFNGLGETPATHSPACINGNVPROC, glGetPathSpacingNV);
    LOAD_PROC(PFNGLGETPATHCOLORGENIVNVPROC, glGetPathColorGenivNV);
    LOAD_PROC(PFNGLGETPATHCOLORGENFVNVPROC, glGetPathColorGenfvNV);
    LOAD_PROC(PFNGLGETPATHTEXGENIVNVPROC, glGetPathTexGenivNV);
    LOAD_PROC(PFNGLGETPATHTEXGENFVNVPROC, glGetPathTexGenfvNV);
    LOAD_PROC(PFNGLISPOINTINFILLPATHNVPROC, glIsPointInFillPathNV);
    LOAD_PROC(PFNGLISPOINTINSTROKEPATHNVPROC, glIsPointInStrokePathNV);
    LOAD_PROC(PFNGLGETPATHLENGTHNVPROC, glGetPathLengthNV);
    LOAD_PROC(PFNGLPOINTALONGPATHNVPROC, glPointAlongPathNV);
    LOAD_PROC(PFNGLPATHSTENCILFUNCNVPROC, glPathStencilFuncNV);
    LOAD_PROC(PFNGLPATHSTENCILDEPTHOFFSETNVPROC, glPathStencilDepthOffsetNV);
    LOAD_PROC(PFNGLPATHCOVERDEPTHFUNCNVPROC,  glPathCoverDepthFuncNV);
    if (fail)
        goto error;
    return 1;
error:
    free(ctx);
    return 0;
}

static void nvpr_release(void *render)
{
    render_ctx *ctx = render;
    free(ctx);
}

static void nvpr_begin_frame(void *render, LVGMovieClip *clip, int winWidth, int winHeight, int width, int height)
{
    render_ctx *ctx = render;
    ctx->winWidth = winWidth, ctx->winHeight = winHeight, ctx->width = width, ctx->height = height;
    glMatrixLoadIdentityEXT(GL_PROJECTION);
    glMatrixOrthoEXT(GL_PROJECTION, 0, winWidth, winHeight, 0, -1, 1);
    glMatrixLoadIdentityEXT(GL_MODELVIEW);
    if (!clip)
        return;
    float clip_w = clip->bounds[2] - clip->bounds[0], clip_h = clip->bounds[3] - clip->bounds[1];
    float scalex = width/clip_w;
    float scaley = height/clip_h;
    float best_scale = scalex < scaley ? scalex : scaley;

    identity(ctx->transform);
    translate(ctx->transform, -(clip_w*best_scale - width)/2, -(clip_h*best_scale - height)/2);
    Transform3x2 tr;
    scale(tr, best_scale, best_scale);
    mul(ctx->transform, ctx->transform, tr);
    MatrixLoadToGL(ctx->transform);
}

static void nvpr_end_frame(void *render)
{
    //render_ctx *ctx = render;
}

static int nvpr_cache_shape(void *render, NSVGshape *shape)
{
    //render_ctx *ctx = render;
    int cmds = 0, coords = 0;
    GLuint pathObj = glGenPathsNV(1);
    NSVGpath *path;
    for (path = shape->paths; path != NULL; path = path->next)
    {
        if (path->npts > 2)
        {
            int ncubic = (path->npts - 1)/3;
            cmds += 1 + ncubic;
            coords += 2 + ncubic*6;
            if (path->closed)
                cmds++;
        }
    }
    GLubyte *cmd = (GLubyte *)alloca(cmds*sizeof(GLubyte));
    GLfloat *coord = alloca(coords*sizeof(GLfloat));
    cmds = 0, coords = 0;
    for (path = shape->paths; path != NULL; path = path->next)
    {
        if (path->npts > 2)
        {
            cmd[cmds] = GL_MOVE_TO_NV;
            int ncubic = (path->npts - 1)/3;
            memset(cmd + cmds + 1, GL_CUBIC_CURVE_TO_NV, ncubic);
            cmds += 1 + ncubic;
            memcpy(coord + coords, path->pts, (2 + ncubic*6)*sizeof(float));
            coords += 2 + ncubic*6;
            if (path->closed)
                cmd[cmds++] = GL_CLOSE_PATH_NV;
        }
    }
    glPathCommandsNV(pathObj, cmds, cmd, coords, GL_FLOAT, coord);
    shape->cache = pathObj;
    return pathObj;
}

static int morph_shape(NSVGshape *shape, NSVGshape *shape2, float ratio)
{
    //render_ctx *ctx = render;
    int cmds = 0, coords = 0;
    float om_ratio = 1.0f - ratio;
    NSVGpath *path, *path2;
    for (path = shape->paths; path != NULL; path = path->next)
    {
        if (path->npts > 2)
        {
            int ncubic = (path->npts - 1)/3;
            cmds += 1 + ncubic;
            coords += 2 + ncubic*6;
            if (path->closed)
                cmds++;
        }
    }
    GLubyte *cmd = (GLubyte *)alloca(cmds*sizeof(GLubyte));
    GLfloat *coord = alloca(coords*sizeof(GLfloat));
    cmds = 0, coords = 0;
    path2 = shape2->paths;
    for (path = shape->paths; path != NULL; path = path->next, path2 = path2->next)
    {
        if (path->npts > 2)
        {
            cmd[cmds] = GL_MOVE_TO_NV;
            int ncubic = (path->npts - 1)/3;
            memset(cmd + cmds + 1, GL_CUBIC_CURVE_TO_NV, ncubic);
            cmds += 1 + ncubic;
            for (int i = 0; i < (2 + ncubic*6); i++)
                coord[coords + i] = path->pts[i]*om_ratio + path2->pts[i]*ratio;
            coords += 2 + ncubic*6;
            if (path->closed)
                cmd[cmds++] = GL_CLOSE_PATH_NV;
        }
    }
    GLuint pathObj = glGenPathsNV(1);
    glPathCommandsNV(pathObj, cmds, cmd, coords, GL_FLOAT, coord);
    return pathObj;
}

static int nvpr_cache_image(void *render, int width, int height, int flags, const void *rgba)
{
    //render_ctx *ctx = render;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (flags & IMAGE_REPEAT)
    {
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    return (int)tex;
}

static int nvpr_cache_gradient(void *render, NSVGpaint *fill)
{
    int img = (NSVG_PAINT_LINEAR_GRADIENT == fill->type) ? LinearGradientStops(fill->gradient, 0) : RadialGradientStops(fill->gradient, 0);
    fill->gradient->cache = img;
    return img;
}

static void nvpr_update_image(void *render, int image, const void *rgba)
{
    //render_ctx *ctx = render;
    glBindTexture(GL_TEXTURE_2D, image);
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
}

static void LinearGrad(struct NSVGshape *shape, LVGColorTransform *cxform, int is_fill)
{
    /*struct NSVGgradient *grad = shape->fill.gradient;
    NVGcolor cs = transformColor(nvgColorU32(grad->stops[0].color), o);
    NVGcolor ce = transformColor(nvgColorU32(grad->stops[grad->nstops - 1].color), o);
    GLfloat rgbGen[4][3] = { { (ce.r - cs.r), 0, cs.r },
                             { (ce.g - cs.g), 0, cs.g },
                             { (ce.b - cs.b), 0, cs.b },
                             { (ce.a - cs.a), 0, cs.a } };
    glPathColorGenNV(GL_PRIMARY_COLOR, GL_PATH_OBJECT_BOUNDING_BOX_NV, GL_RGBA, &rgbGen[0][0]);*/
    NSVGgradient *gradient = is_fill ? shape->fill.gradient : shape->stroke.gradient;
    float *xf = gradient->xform;
    GLfloat data[2][3] = { { xf[0], xf[2], xf[4] },
                           { xf[1], xf[3], xf[5] } };
    inverse(data, data);
    /*float p1[2] = { shape->bounds[0], shape->bounds[1] };
    float p2[2] = { shape->bounds[2], shape->bounds[3] };
    xform(p1, data, p1);
    xform(p2, data, p2);*/
    Transform3x2 tr;
    translate(tr, 16384.0/20.0, 16384.0/20.0);
    mul(data, tr, data);
    scale(tr, 20.0/32768.0, 20.0/32768.0); // swf gradients -16384..16384 square in twips
    mul(data, tr, data);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gradient->cache);
    if (cxform->mul[0] != 1.0f || cxform->mul[1] != 1.0f || cxform->mul[2] != 1.0f || cxform->mul[3] != 1.0f)
    {
        glColor4f(cxform->mul[0], cxform->mul[1], cxform->mul[2], cxform->mul[3]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    glPathTexGenNV(GL_TEXTURE0, GL_OBJECT_LINEAR, 2, &data[0][0]);
}

static void RadialGrad(struct NSVGshape *shape, LVGColorTransform *cxform, int is_fill)
{
    NSVGgradient *gradient = is_fill ? shape->fill.gradient : shape->stroke.gradient;
    float *xf = gradient->xform;
    GLfloat data[2][3] = { { xf[0], xf[2], xf[4] },
                           { xf[1], xf[3], xf[5] } };
    inverse(data, data);
    Transform3x2 tr;
    translate(tr, 16384.0/20.0, 16384.0/20.0);
    mul(data, tr, data);
    scale(tr, 20.0/32768.0, 20.0/32768.0); // swf gradients -16384..16384 square in twips
    mul(data, tr, data);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gradient->cache);
    if (cxform->mul[0] != 1.0f || cxform->mul[1] != 1.0f || cxform->mul[2] != 1.0f || cxform->mul[3] != 1.0f)
    {
        glColor4f(cxform->mul[0], cxform->mul[1], cxform->mul[2], cxform->mul[3]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    glPathTexGenNV(GL_TEXTURE0, GL_OBJECT_LINEAR, 2, &data[0][0]);
}

static void ImagePaint(struct NSVGshape *shape, LVGColorTransform *cxform, int is_fill)
{
    NSVGpaint *p = is_fill ? &shape->fill : &shape->stroke;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, p->color);
    if (NSVG_SPREAD_PAD == p->spread)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    if (/*p->filtered*/1) //TODO: make cmdline switch
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    float *xf = p->xform;
    GLfloat data[2][3] = { { xf[0], xf[2], xf[4] },
                           { xf[1], xf[3], xf[5] } };
    inverse(data, data);
    Transform3x2 tr;
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    scale(tr, 20.0/w, 20.0/h);
    mul(data, tr, data);
    if (cxform->mul[0] != 1.0f || cxform->mul[1] != 1.0f || cxform->mul[2] != 1.0f || cxform->mul[3] != 1.0f)
    {
        glColor4f(cxform->mul[0], cxform->mul[1], cxform->mul[2], cxform->mul[3]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    glPathTexGenNV(GL_TEXTURE0, GL_OBJECT_LINEAR, 2, &data[0][0]);
}

static void nvpr_render_shape(void *render, LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode)
{
    //render_ctx *ctx = render;
    for (int j = 0; j < shapecol->num_shapes; j++)
    {
        NSVGshape *shape = shapecol->shapes + j;
        NSVGshape *shape2 = shapecol->morph ? shapecol->morph->shapes + j : 0;
        GLuint pathObj = shape->cache;
        if (shape2)
            pathObj = morph_shape(shape, shape2, ratio);

        /*GLfloat object_bbox[4], fill_bbox[4], stroke_bbox[4];
        glGetPathParameterfvNV(pathObj, GL_PATH_OBJECT_BOUNDING_BOX_NV, object_bbox);
        glGetPathParameterfvNV(pathObj, GL_PATH_FILL_BOUNDING_BOX_NV, fill_bbox);
        glGetPathParameterfvNV(pathObj, GL_PATH_STROKE_BOUNDING_BOX_NV, stroke_bbox);*/

        glEnable(GL_BLEND);
        if (BLEND_overlay == blend_mode || BLEND_multiply == blend_mode)
        {
            glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
        } if (BLEND_screen == blend_mode)
        {
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        } else
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
        glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
        glEnable(GL_STENCIL_TEST);
        if (NSVG_PAINT_NONE != shape->fill.type)
        {
            if (NSVG_PAINT_COLOR == shape->fill.type)
            {
                NVGcolor c = transformColor(nvgColorU32(shape->fill.color), cxform);
                glColor4f(c.r, c.g, c.b, c.a);
            } else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
                LinearGrad(shape, cxform, 1);
            else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
                RadialGrad(shape, cxform, 1);
            else if (NSVG_PAINT_IMAGE == shape->fill.type)
                ImagePaint(shape, cxform, 1);
            glStencilFillPathNV(pathObj, (NSVG_FILLRULE_EVENODD == shape->fillRule) ? GL_INVERT : GL_COUNT_UP_NV, 0x1F);
            glCoverFillPathNV(pathObj, GL_BOUNDING_BOX_NV);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glPathTexGenNV(GL_TEXTURE0, GL_NONE, 0, NULL);
            glPathColorGenNV(GL_PRIMARY_COLOR, GL_NONE, 0, NULL);
            glDisable(GL_TEXTURE_2D);
        }
        if (NSVG_PAINT_NONE != shape->stroke.type)
        {
            if (NSVG_PAINT_COLOR == shape->stroke.type)
            {
                NVGcolor c = transformColor(nvgColorU32(shape->stroke.color), cxform);
                glColor4f(c.r, c.g, c.b, c.a);
            } else if (NSVG_PAINT_LINEAR_GRADIENT == shape->stroke.type)
            {
                //LinearGrad(shape, o, 0);
                NVGcolor c = nvgColorU32(shape->stroke.gradient->stops[0].color);
                glColor4f(c.r, c.g, c.b, c.a);
            } else if (NSVG_PAINT_RADIAL_GRADIENT == shape->stroke.type)
            {
                NVGcolor c = nvgColorU32(shape->stroke.gradient->stops[0].color);
                glColor4f(c.r, c.g, c.b, c.a);
            }
            glPathParameterfNV(pathObj, GL_PATH_STROKE_WIDTH_NV, shape->strokeWidth);
            if (NSVG_JOIN_ROUND == shape->strokeLineJoin)
                glPathParameteriNV(pathObj, GL_PATH_JOIN_STYLE_NV, GL_ROUND_NV);
            else if (NSVG_JOIN_BEVEL == shape->strokeLineJoin)
                glPathParameteriNV(pathObj, GL_PATH_JOIN_STYLE_NV, GL_BEVEL_NV);
            else if (NSVG_JOIN_MITER == shape->strokeLineJoin)
                glPathParameteriNV(pathObj, GL_PATH_JOIN_STYLE_NV, GL_MITER_TRUNCATE_NV);
            glPathParameterfNV(pathObj, GL_PATH_MITER_LIMIT_NV, shape->miterLimit);
            if (NSVG_CAP_ROUND == shape->strokeLineCap)
                glPathParameteriNV(pathObj, GL_PATH_END_CAPS_NV, GL_ROUND_NV);
            else if (NSVG_CAP_BUTT == shape->strokeLineCap)
                glPathParameteriNV(pathObj, GL_PATH_END_CAPS_NV, GL_FLAT);
            else if (NSVG_CAP_SQUARE == shape->strokeLineCap)
                glPathParameteriNV(pathObj, GL_PATH_END_CAPS_NV, GL_SQUARE_NV);
            GLint reference = 0x1;
            glStencilStrokePathNV(pathObj, reference, 0x1F);
            glCoverStrokePathNV(pathObj, GL_BOUNDING_BOX_NV);

            glPathColorGenNV(GL_PRIMARY_COLOR, GL_NONE, 0, NULL);
        }
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        if (shape2)
            glDeletePathsNV(pathObj, 1);
    }
}

static void nvpr_render_image(void *render, int image)
{
    //render_ctx *ctx = render;
    glEnable(GL_TEXTURE_2D);
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    glBindTexture(GL_TEXTURE_2D, image);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, h);
    glTexCoord2f(1, 1); glVertex2f(w, h);
    glTexCoord2f(1, 0); glVertex2f(w, 0);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

static void nvpr_set_transform(void *render, float *t, int reset)
{
    render_ctx *ctx = render;
    if (reset)
        identity(ctx->transform);
    Transform3x2 tr;
    tr[0][0] = t[0]; tr[1][0] = t[1];
    tr[0][1] = t[2]; tr[1][1] = t[3];
    tr[0][2] = t[4]; tr[1][2] = t[5];
    mul(ctx->transform, ctx->transform, tr);
    MatrixLoadToGL(ctx->transform);
}

static void nvpr_get_transform(void *render, float *t)
{
    render_ctx *ctx = render;
    t[0] = ctx->transform[0][0]; t[1] = ctx->transform[1][0];
    t[2] = ctx->transform[0][1]; t[3] = ctx->transform[1][1];
    t[4] = ctx->transform[0][2]; t[5] = ctx->transform[1][2];
}

int nvpr_inside_shape(void *render, NSVGshape *shape, float x, float y)
{
    GLuint pathObj = shape->cache;
    return glIsPointInFillPathNV(pathObj, 0xFF, x, y);
}

const render nvpr_render =
{
    nvpr_init,
    nvpr_release,
    nvpr_begin_frame,
    nvpr_end_frame,
    nvpr_cache_shape,
    nvpr_cache_image,
    nvpr_cache_gradient,
    gl_free_image,
    nvpr_update_image,
    nvpr_render_shape,
    nvpr_render_image,
    nvpr_set_transform,
    nvpr_get_transform,
    nvpr_inside_shape
};
