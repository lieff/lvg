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

static int nvpr_init(void **render)
{
    int fail = 0;
    if (glfwExtensionSupported("GL_NV_path_rendering"))
    {
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
    }
    if (fail)
        printf("failed to load NVPR extensions\n");
    return 1;
}

static void nvpr_release(void *render)
{
}

static void nvpr_begin_frame(void *render, LVGMovieClip *clip, int winWidth, int winHeight, int width, int height)
{
    //glMatrixLoadIdentityEXT(GL_PROJECTION);
    //glMatrixOrtho(GL_PROJECTION, 0, 640, 480, 0, -1, 1);
    //glMatrixLoadIdentity(GL_MODELVIEW);
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

static inline NVGcolor nvgColorU32(uint32_t c)
{
    return nvgRGBA(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff, 255);
}

static void nvpr_render_shape(void *render, NSVGshape *shape, LVGObject *o)
{
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
        glColor3f(c.r,c.g,c.b);
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
        int w = shape->bounds[2] - shape->bounds[0], h = shape->bounds[3] - shape->bounds[1];
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
            glColor3f(c.r,c.g,c.b);
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
