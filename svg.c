#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#define GL_GLEXT_PROTOTYPES
#ifdef EMSCRIPTEN
#include <emscripten.h>
#define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#if !defined(EMSCRIPTEN) && defined(DEBUG)
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

#include "nanovg.h"
#ifdef EMSCRIPTEN
#define NANOVG_GLES2_IMPLEMENTATION
#else
#include <libtcc.h>
#define NANOVG_GL2_IMPLEMENTATION
#endif
#include "nanovg_gl.h"

#include "lunzip.h"
#include "all.h"
#include "lvg_header.h"

zip_t g_zip;
NVGcontext *vg = NULL;
NVGcolor g_bgColor;
GLFWwindow *window;
int winWidth, winHeight;
int width = 0, height = 0;
double mx = 0, my = 0;
double g_time;
int mkeys = 0;
const char *g_main_script;

#ifndef EMSCRIPTEN
void (*onInit)();
void (*onFrame)();
#else
extern void onInit();
extern void onFrame();
#endif

NVGpaint nvgLinearGradientTCC(NVGcontext* ctx,
    float sx, float sy, float ex, float ey,
    float ir, float ig, float ib, float ia, float or, float og, float ob, float oa)
{
    float m = 1.0/255.0;
    return nvgLinearGradient(ctx, sx, sy, ex, ey, nvgRGBAf(ir*m, ig*m, ib*m, ia*m), nvgRGBAf(or*m, og*m, ob*m, oa*m));
}

char *lvgGetFileContents(const char *fname, uint32_t *size)
{
    uint32_t idx;
    if ((idx = lvgZipNameLocate(&g_zip, fname)) == (int32_t)-1)
        return 0;
    return lvgZipDecompress(&g_zip, idx, size);
}

void lvgFree(void *buf)
{
    free(buf);
}

NSVGimage *lvgLoadSVG(const char *file)
{
    char *buf;
    double time = glfwGetTime();
    if (!(buf = lvgGetFileContents(file, 0)))
    {
        printf("error: could not open SVG image.\n");
        return 0;
    }
    double time2 = glfwGetTime();
    printf("zip time: %fs\n", time2 - time);
    NSVGimage *image = nsvgParse(buf, "px", 96.0f);
    free(buf);
    time = glfwGetTime();
    printf("svg load time: %fs\n", time - time2);
#if !defined(EMSCRIPTEN) && defined(DEBUG)
    if (0)
    {
        int w = (int)image->width;
        int h = (int)image->height;
        NSVGrasterizer *rast = nsvgCreateRasterizer();
        if (rast == NULL) {
            printf("Could not init rasterizer.\n");
        }
        unsigned char *img = malloc(w*h*4);
        if (img == NULL) {
            printf("Could not alloc image buffer.\n");
        }
        nsvgRasterize(rast, image, 0, 0, 1, img, w, h, w*4);
        stbi_write_png("svg.png", w, h, 4, img, w*4);
        nsvgDeleteRasterizer(rast);
    }
#endif
    return image;
}

#define READ(p, n) { memcpy(p, buf, n); buf += n; }

char *load_paint(char *buf, NSVGpaint *p)
{
    READ(&p->type, 1);
    if (NSVG_PAINT_COLOR == p->type)
    {
        READ(&p->color, 4);
    } else if (NSVG_PAINT_LINEAR_GRADIENT == p->type || NSVG_PAINT_RADIAL_GRADIENT == p->type)
    {
        int nstops = 0;
        READ(&nstops, 4);
        NSVGgradient *g = p->gradient = (NSVGgradient*)malloc(sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*nstops);
        g->nstops = nstops;
        READ(&g->xform, 4*6);
        READ(&g->fx, 4*2);
        READ(&g->spread, 1);
        READ(g->stops, g->nstops*8);
    }
    return buf;
}

NSVGimage *lvgLoadSVGB(const char *file)
{
    char *buf, *save_buf;
    double time = glfwGetTime();
    if (!(buf = save_buf = lvgGetFileContents(file, 0)))
    {
        printf("error: could not open SVG image.\n");
        return 0;
    }
    double time2 = glfwGetTime();
    printf("zip time: %fs\n", time2 - time);

    NSVGimage *image = (NSVGimage*)malloc(sizeof(NSVGimage));
    memset(image, 0, sizeof(NSVGimage));

    int nshapes = 0;
    READ(&nshapes, 4);
    READ(&image->width, 4);
    READ(&image->height, 4);
    image->shapes = (NSVGshape*)malloc(nshapes*sizeof(NSVGshape));
    memset(image->shapes, 0, nshapes*sizeof(NSVGshape));
    NSVGshape *shape = 0;
    for (int i = 0; i < nshapes; i++)
    {
        if (shape)
            shape->next = &image->shapes[i];
        shape = &image->shapes[i];
        int npaths = 0;
        READ(&npaths, 4);
        buf = load_paint(buf, &shape->fill);
        buf = load_paint(buf, &shape->stroke);
        READ(&shape->strokeWidth, 4);
        READ(shape->bounds, 4*4);
        shape->flags |= NSVG_FLAGS_VISIBLE;

        shape->paths = (NSVGpath*)malloc(npaths*sizeof(NSVGpath));
        memset(shape->paths, 0, npaths*sizeof(NSVGpath));
        NSVGpath *path = 0;
        for (int j = 0; j < npaths; j++)
        {
            if (path)
                path->next = &shape->paths[j];
            path = &shape->paths[j];
            READ(&path->npts, 4);
            READ(&path->closed, 1);
            path->pts = (float*)malloc(sizeof(path->pts[0])*path->npts*2);
            READ(path->pts, sizeof(path->pts[0])*path->npts*2);
        }
    }

    free(save_buf);
    time = glfwGetTime();
    printf("svg load time: %fs\n", time - time2);
    return image;
}

static inline NVGcolor nvgColorU32(uint32_t c)
{
    return nvgRGBA(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff, 255);
}

static void nvgSVGLinearGrad(struct NVGcontext *vg, struct NSVGshape *shape, int is_fill)
{
    struct NSVGgradient *grad = shape->fill.gradient;
    float sx = shape->bounds[0];
    float sy = shape->bounds[1];
    float ex = shape->bounds[2];
    float ey = shape->bounds[3];
    NVGcolor cs = nvgColorU32(grad->stops[0].color);
    NVGcolor ce = nvgColorU32(grad->stops[1].color);

    NVGpaint p = nvgLinearGradient(vg, sx, sy, ex, ey, cs, ce);
    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

static void nvgSVGRadialGrad(struct NVGcontext *vg, struct NSVGshape *shape, int is_fill)
{
    struct NSVGgradient *grad = shape->fill.gradient;
    float cx = (shape->bounds[0] + shape->bounds[2])/2.0;
    float cy = (shape->bounds[1] + shape->bounds[3])/2.0;
    NVGcolor cs = nvgColorU32(grad->stops[0].color);
    NVGcolor ce = nvgColorU32(grad->stops[1].color);

    NVGpaint p = nvgRadialGradient(vg, cx, cy, 0, cx, cs, ce);
    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

void lvgDrawShape(NSVGshape *shape)
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
    if (NSVG_PAINT_COLOR == shape->fill.type)
        nvgFillColor(vg, nvgColorU32(shape->fill.color));
    else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
        nvgSVGLinearGrad(vg, shape, 1);
    else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
        nvgSVGRadialGrad(vg, shape, 1);
    else if (NSVG_PAINT_IMAGE == shape->fill.type)
    {
        int w = shape->bounds[2] - shape->bounds[0], h = shape->bounds[3] - shape->bounds[1];
        NVGpaint imgPaint = nvgImagePattern(vg, shape->bounds[0], shape->bounds[1], w, h, 0, shape->fill.color, 1.0f);
        nvgFillPaint(vg, imgPaint);
    }
    if (NSVG_PAINT_NONE != shape->fill.type)
    {
        //if (NSVG_FILLRULE_EVENODD == shape->fillRule)
        //    nvgPathWinding(vg, NVG_HOLE);
        nvgFill(vg);
    }
    if (NSVG_PAINT_NONE != shape->stroke.type)
    {
        if (NSVG_PAINT_COLOR == shape->stroke.type)
            nvgStrokeColor(vg, nvgColorU32(shape->stroke.color));
        else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
            nvgSVGLinearGrad(vg, shape, 0);
        else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
            nvgSVGRadialGrad(vg, shape, 0);
        nvgStrokeWidth(vg, shape->strokeWidth);
        nvgStroke(vg);
    }
}

void lvgDrawSVG(NSVGimage *image)
{
    NSVGshape *shape;
    for (shape = image->shapes; shape != NULL; shape = shape->next)
    {
        if (!(shape->flags & NSVG_FLAGS_VISIBLE))
            continue;
        lvgDrawShape(shape);
    }
}

static void lvgDrawClipGroup(LVGMovieClip *clip, LVGMovieClipGroup *group, int next_frame)
{
    int w, h;
    int frame = group->cur_frame;
    assert(frame < group->num_frames);
    if (next_frame)
        group->cur_frame = (group->cur_frame + 1) % group->num_frames;
    for (int i = 0; i < group->frames[frame].num_objects; i++)
    {
        LVGObject *o = &group->frames[frame].objects[i];
        nvgSave(vg);
        nvgTransform(vg, o->t[0], o->t[1], o->t[2], o->t[3], o->t[4], o->t[5]);
        if (LVG_OBJ_SHAPE == o->type)
        {
            for (int j = 0; j < clip->shapes[o->id].num_shapes; j++)
                lvgDrawShape(&clip->shapes[o->id].shapes[j]);
        } else
        if (LVG_OBJ_IMAGE == o->type)
        {
            int w, h;
            nvgImageSize(vg, clip->images[o->id], &w, &h);
            NVGpaint imgPaint = nvgImagePattern(vg, 0, 0, w, h, 0, clip->images[o->id], 1.0f);
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, w, h);
            nvgFillPaint(vg, imgPaint);
            nvgFill(vg);
        } else
        if (LVG_OBJ_GROUP == o->type)
        {
            lvgDrawClipGroup(clip, clip->groups + o->id, next_frame);
        }
        nvgRestore(vg);
    }
}

void lvgDrawClip(LVGMovieClip *clip)
{
    int next_frame = 0;
    if ((g_time - clip->last_time) > (1.0/clip->fps))
    {
        next_frame = 1;
        clip->last_time += (1.0/clip->fps);
    }
    lvgDrawClipGroup(clip, clip->groups, next_frame);
#ifdef DEBUG
    int w, h;
    nvgImageSize(vg, clip->images[clip->num_images - 1], &w, &h);
    NVGpaint imgPaint = nvgImagePattern(vg, 600, 0, w, h, 0, clip->images[clip->num_images - 1], 1.0f);
    nvgBeginPath(vg);
    nvgRect(vg, 600, 0, w, h);
    nvgFillPaint(vg, imgPaint);
    nvgFill(vg);
#endif
}

void drawframe()
{
    glfwPollEvents();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &width, &height);
    glfwGetCursorPos(window, &mx, &my);
    mkeys = 0;
    mkeys |= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? 1 : 0;
    mkeys |= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ? 2 : 0;
    mkeys |= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ? 4 : 0;

    glViewport(0, 0, width, height);
    glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(vg, winWidth, winHeight, (float)width / (float)winWidth);
    //nvgSave(vg);

    g_time = glfwGetTime();
#ifdef EMSCRIPTEN
    if (g_main_script)
#else
    if (onFrame)
#endif
        onFrame();

    //nvgRestore(vg);
    nvgEndFrame(vg);
    glfwSwapBuffers(window);
}

#ifndef EMSCRIPTEN

struct SYM
{
    const char *m_name;
    void *m_sym;
};

const struct SYM g_syms[] = {
    { "lvgDrawSVG", lvgDrawSVG },
    { "lvgDrawClip", lvgDrawClip },
    { "lvgLoadSVG", lvgLoadSVG },
    { "lvgLoadSVGB", lvgLoadSVGB },
    { "lvgLoadSWF", lvgLoadSWF },
    { "lvgGetFileContents", lvgGetFileContents },
    { "lvgFree", lvgFree },
    { "sin", sin },
    { "floor", floor },

    { "nvgScale", nvgScale },
    { "nvgSave", nvgSave },
    { "nvgBeginPath", nvgBeginPath },
    { "nvgRoundedRect", nvgRoundedRect },
    { "nvgRGBA", nvgRGBA },
    { "nvgFillColor", nvgFillColor },
    { "nvgFill", nvgFill },
    { "nvgBoxGradient", nvgBoxGradient },
    { "nvgRect", nvgRect },
    { "nvgPathWinding", nvgPathWinding },
    { "nvgFillPaint", nvgFillPaint },
    { "nvgLinearGradient", nvgLinearGradient },
    { "nvgLinearGradientTCC", nvgLinearGradientTCC },
    { "nvgMoveTo", nvgMoveTo },
    { "nvgLineTo", nvgLineTo },
    { "nvgStrokeColor", nvgStrokeColor },
    { "nvgStroke", nvgStroke },
    { "nvgFontSize", nvgFontSize },
    { "nvgFontFace", nvgFontFace },
    { "nvgTextAlign", nvgTextAlign },
    { "nvgFontBlur", nvgFontBlur },
    { "nvgText", nvgText },
    { "nvgRestore", nvgRestore },
    { "nvgTextBounds", nvgTextBounds },
    { "nvgRadialGradient", nvgRadialGradient },
    { "nvgCircle", nvgCircle },
    { "nvgEllipse", nvgEllipse },
    { "nvgBezierTo", nvgBezierTo },
    { "nvgStrokeWidth", nvgStrokeWidth },
    { "nvgArc", nvgArc },
    { "nvgClosePath", nvgClosePath },
    { "nvgScissor", nvgScissor },
    { "nvgTranslate", nvgTranslate },
    { "nvgImageSize", nvgImageSize },
    { "nvgImagePattern", nvgImagePattern },
    { "nvgHSLA", nvgHSLA },
    { "nvgRotate", nvgRotate },
    { "nvgLineCap", nvgLineCap },
    { "nvgLineJoin", nvgLineJoin },
    { "nvgCreateImage", nvgCreateImage },
    { "nvgCreateImageMem", nvgCreateImageMem },
    { "nvgCreateFont", nvgCreateFont },
    { "nvgCreateFontMem", nvgCreateFontMem },
    { "nvgDeleteImage", nvgDeleteImage },
    { "nvgTextMetrics", nvgTextMetrics },
    { "nvgTextBreakLines", nvgTextBreakLines },
    { "nvgTextGlyphPositions", nvgTextGlyphPositions },
    { "nvgTextLineHeight", nvgTextLineHeight },
    { "nvgTextBoxBounds", nvgTextBoxBounds },
    { "nvgGlobalAlpha", nvgGlobalAlpha },
    { "nvgTextBox", nvgTextBox },
    { "nvgDegToRad", nvgDegToRad },
    { "nvgResetScissor", nvgResetScissor },
    { "nvgIntersectScissor", nvgIntersectScissor },

    { "vg", &vg },
    { "g_bgColor", &g_bgColor },
    { "winWidth", &winWidth },
    { "winHeight", &winHeight },
    { "width", &width },
    { "height", &height },
    { "mkeys", &mkeys },
    { "mx", &mx },
    { "my", &my },
    { "g_time", &g_time }
};

void (tcc_error_func)(void *opaque, const char *msg)
{
    printf("%s\n", msg);
    fflush(stdout);
}

int loadScript()
{
    TCCState *s;
    char *buf, *source;
    int size, i;

    if (!(buf = lvgGetFileContents("main.c", 0)))
    {
        printf("error: could not open C script.\n");
        return -1;
    }
    source = malloc(strlen(allh_h) + strlen(buf) + 1);
    source[0] = 0;
    strcat(source, allh_h);
    strcat(source, buf);
    free(buf);

    s = tcc_new();
    tcc_set_error_func(s, 0, tcc_error_func);
    //tcc_set_options(s, "-g");
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
    tcc_set_lib_path(s, "./lib");

    if (tcc_compile_string(s, source) == -1)
        goto error;

    for (i = 0; i < sizeof(g_syms)/sizeof(g_syms[0]); i++)
        tcc_add_symbol(s, g_syms[i].m_name, g_syms[i].m_sym);

    size = tcc_relocate(s, TCC_RELOCATE_AUTO);
    if (size == -1)
        goto error;

    onInit  = tcc_get_symbol(s, "onInit");
    onFrame = tcc_get_symbol(s, "onFrame");
    free(source);
    return 0;
error:
    free(source);
    return -1;
}
#endif

int open_lvg(const char *file)
{
    if (lvgZipOpen(file, &g_zip))
        return -1;
    g_main_script = 0;
    char *buf;
#ifdef EMSCRIPTEN
    if (!(buf = lvgGetFileContents("main.js", 0)))
        printf("error: could not open JS script.\n");
    else
        g_main_script = buf;
#else
    loadScript();
#endif
    return 0;
}

int main(int argc, char **argv)
{
    if (!glfwInit() || open_lvg(argc > 1 ? argv[1] : "main.lvg"))
    {
        printf("error: could not open lvg file\n");
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, 1);
#ifdef EMSCRIPTEN
    width = 1024, height = 800;
#else
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    width = mode->width - 40, height = mode->height - 80;
    if (width > 1024)
        width = 1024;
    if (height > 800)
        height = 800;
#endif
    window = glfwCreateWindow(width, height, "LVG Player", NULL, NULL);
    if (!window)
    {
        printf("error: could not open window\n");
        glfwTerminate();
        return -1;
    }

#ifndef EMSCRIPTEN
    glfwSwapInterval(1);
#endif
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

#ifdef EMSCRIPTEN
    vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#else
    vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES
#ifdef DEBUG
        | NVG_DEBUG
#endif
        );
#endif

    /*GLuint texId[1];
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, texId);
    glBindTexture(GL_TEXTURE_2D, texId[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);*/

#ifdef EMSCRIPTEN
    if (g_main_script)
    {
        EM_ASM_({
            var src = Pointer_stringify($0);
            Runtime.loadDynamicLibrarySrc(src);
        }, g_main_script);
    }
    if (g_main_script)
        onInit();
    glfwSetTime(0);
    emscripten_set_main_loop(drawframe, 0, 1);
#else
    if (onInit)
        onInit();
    glfwSetTime(0);
    while (!glfwWindowShouldClose(window))
    {
        drawframe();
    }
#endif
#ifdef EMSCRIPTEN
    nvgDeleteGLES2(vg);
#else
    nvgDeleteGL2(vg);
#endif
    lvgZipClose(&g_zip);

    glfwTerminate();
    return 0;
}
