//#define GL_GLEXT_PROTOTYPES
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "render.h"

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

#if defined(_WIN32)
# define GET_PROC_ADDRESS(name)  wglGetProcAddress(#name)
#else
# define GET_PROC_ADDRESS(name)  glXGetProcAddress((const GLubyte *) #name)
#endif

#define LOAD_PROC(type, name) \
  FUNC(name) = (type) GET_PROC_ADDRESS(name); \
  if (!FUNC(name)) { \
    fail = 1; \
  }

typedef float Transform3x2[2][3];

typedef struct render_ctx
{
    Transform3x2 transform;
} render_ctx;

void identity(Transform3x2 dst)
{
    dst[0][0] = 1;
    dst[0][1] = 0;
    dst[0][2] = 0;
    dst[1][0] = 0;
    dst[1][1] = 1;
    dst[1][2] = 0;
}

void mul(Transform3x2 dst, Transform3x2 a, Transform3x2 b)
{
    Transform3x2 result;
    result[0][0] = a[0][0]*b[0][0] + a[0][1]*b[1][0];
    result[0][1] = a[0][0]*b[0][1] + a[0][1]*b[1][1];
    result[0][2] = a[0][0]*b[0][2] + a[0][1]*b[1][2] + a[0][2];

    result[1][0] = a[1][0]*b[0][0] + a[1][1]*b[1][0];
    result[1][1] = a[1][0]*b[0][1] + a[1][1]*b[1][1];
    result[1][2] = a[1][0]*b[0][2] + a[1][1]*b[1][2] + a[1][2];

    dst[0][0] = result[0][0];
    dst[0][1] = result[0][1];
    dst[0][2] = result[0][2];
    dst[1][0] = result[1][0];
    dst[1][1] = result[1][1];
    dst[1][2] = result[1][2];
}

void translate(Transform3x2 dst, float x, float y)
{
    dst[0][0] = 1;
    dst[0][1] = 0;
    dst[0][2] = x;
    dst[1][0] = 0;
    dst[1][1] = 1;
    dst[1][2] = y;
}

void scale(Transform3x2 dst, float x, float y)
{
    dst[0][0] = x;
    dst[0][1] = 0;
    dst[0][2] = 0;

    dst[1][0] = 0;
    dst[1][1] = y;
    dst[1][2] = 0;
}

void rotate(Transform3x2 dst, float angle)
{
    float radians = angle*3.14159f/180.0f,
          s = (float)sin(radians),
          c = (float)cos(radians);
    dst[0][0] = c;
    dst[0][1] = -s;
    dst[0][2] = 0;
    dst[1][0] = s;
    dst[1][1] = c;
    dst[1][2] = 0;
}

void ortho(Transform3x2 dst, float l, float r, float b, float t)
{
    dst[0][0] = 2/(r-l);
    dst[0][1] = 0;
    dst[0][2] = -(r+l)/(r-l);
    dst[1][0] = 0;
    dst[1][1] = 2/(t-b);
    dst[1][2] = -(t+b)/(t-b);
}

void inverse_ortho(Transform3x2 dst, float l, float r, float b, float t)
{
    dst[0][0] = (r-l)/2;
    dst[0][1] = 0;
    dst[0][2] = (r+l)/2;
    dst[1][0] = 0;
    dst[1][1] = (t-b)/2;
    dst[1][2] = (t+b)/2;
}

void xform(float dst[2], Transform3x2 a, const float v[2])
{
    float result[2];
    result[0] = a[0][0]*v[0] + a[0][1]*v[1] + a[0][2];
    result[1] = a[1][0]*v[0] + a[1][1]*v[1] + a[1][2];
    dst[0] = result[0];
    dst[1] = result[1];
}

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

static int nvpr_init(void **render)
{
    render_ctx *ctx = calloc(1, sizeof(render_ctx));
    *render = ctx;
    if (!ctx)
        return 0;
    int fail = 0;
    if (!glfwExtensionSupported("GL_NV_path_rendering"))
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
    return 1;
}

static int nvpr_cache_image(void *render, int width, int height, const void *rgba)
{
    //render_ctx *ctx = render;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    return (int)tex;
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

static inline NVGcolor nvgColorU32(uint32_t c)
{
    return nvgRGBA(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff, 255);
}

static void nvpr_render_shape(void *render, NSVGshape *shape, LVGObject *o)
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
        }
    }
    glPathCommandsNV(pathObj, cmds, cmd, coords, GL_FLOAT, coord);


    GLfloat object_bbox[4],
            fill_bbox[4],
            stroke_bbox[4];

    glGetPathParameterfvNV(pathObj, GL_PATH_OBJECT_BOUNDING_BOX_NV, object_bbox);
    glGetPathParameterfvNV(pathObj, GL_PATH_FILL_BOUNDING_BOX_NV, fill_bbox);
    glGetPathParameterfvNV(pathObj, GL_PATH_STROKE_BOUNDING_BOX_NV, stroke_bbox);

    glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
    glEnable(GL_STENCIL_TEST);
    if (NSVG_PAINT_COLOR == shape->fill.type)
    {
        NVGcolor c = nvgColorU32(shape->fill.color);
        glColor4f(c.r, c.g, c.b, 1);
    } else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
    {
        GLfloat rgbGen[3][3] = { {0,  0, 0},
                                 {0,  1, 0},
                                 {0, -1, 1} };
        glPathColorGenNV(GL_PRIMARY_COLOR, GL_PATH_OBJECT_BOUNDING_BOX_NV, GL_RGB, &rgbGen[0][0]);
    }
    else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
    {
        GLfloat rgbGen[3][3] = { {0,  0, 0},
                                 {0,  1, 0},
                                 {0, -1, 1} };
        glPathColorGenNV(GL_PRIMARY_COLOR, GL_PATH_OBJECT_BOUNDING_BOX_NV, GL_RGB, &rgbGen[0][0]);
    }
    else if (NSVG_PAINT_IMAGE == shape->fill.type)
    {
        //int w = shape->bounds[2] - shape->bounds[0], h = shape->bounds[3] - shape->bounds[1];
        GLfloat rgbGen[3][3] = { {0,  0, 0},
                                 {0,  1, 0},
                                 {0, -1, 1} };
        glPathColorGenNV(GL_PRIMARY_COLOR, GL_PATH_OBJECT_BOUNDING_BOX_NV, GL_RGB, &rgbGen[0][0]);
    }
    if (NSVG_PAINT_NONE != shape->fill.type)
    {
        glStencilFillPathNV(pathObj, GL_COUNT_UP_NV, 0x1F);
        glCoverFillPathNV(pathObj, GL_BOUNDING_BOX_NV);
        glPathColorGenNV(GL_PRIMARY_COLOR, GL_NONE, 0, NULL);
    }
    if (NSVG_PAINT_NONE != shape->stroke.type)
    {
        if (NSVG_PAINT_COLOR == shape->stroke.type)
        {
            NVGcolor c = nvgColorU32(shape->stroke.color);
            glColor4f(c.r, c.g, c.b, 1);
        } else if (NSVG_PAINT_LINEAR_GRADIENT == shape->stroke.type)
        {
        } else if (NSVG_PAINT_RADIAL_GRADIENT == shape->stroke.type)
        {
        }
        glPathParameterfNV(pathObj, GL_PATH_STROKE_WIDTH_NV, shape->strokeWidth);
        GLint reference = 0x1;
        glStencilStrokePathNV(pathObj, reference, 0x1F);
        glCoverStrokePathNV(pathObj, GL_BOUNDING_BOX_NV);
        glPathColorGenNV(GL_PRIMARY_COLOR, GL_NONE, 0, NULL);
    }
    glDeletePathsNV(pathObj, 1);
    glDisable(GL_STENCIL_TEST);
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
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f(w, 0);
    glTexCoord2f(1, 0); glVertex2f(w, h);
    glTexCoord2f(0, 0); glVertex2f(0, h);
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
