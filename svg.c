#include <stdio.h>
#include <string.h>
#include <float.h>
#include <zip.h>
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

struct zip *g_zip;
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

char *lvgGetFileContents(const char *fname, int *size)
{
    struct zip_stat st;
    struct zip_file *zf;
    zip_int64_t idx;
    if ((idx = zip_name_locate(g_zip, fname, 0)) < 0)
    return 0;
    if (!(zf = zip_fopen_index(g_zip, idx, 0)))
        return 0;
    zip_stat_index(g_zip, idx, 0, &st);
    char *buf = malloc(st.size + 1);
    buf[st.size] = 0;
    zip_fread(zf, buf, st.size);
    zip_fclose(zf);
    if (size)
        *size = st.size;
    return buf;
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

    NVGpaint p = nvgLinearGradient(vg, sx, sy, ex, ey, nvgColorU32(grad->stops[0].color), ce);
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

void lvgDrawSVG(NSVGimage *image)
{
    NSVGshape *shape;
    NSVGpath *path;

    int i;
    for (shape = image->shapes; shape != NULL; shape = shape->next)
    {
        if (!(shape->flags & NSVG_FLAGS_VISIBLE))
            continue;

        for (path = shape->paths; path != NULL; path = path->next)
        {
            nvgBeginPath(vg);
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
            if (NSVG_PAINT_COLOR == shape->fill.type)
                nvgFillColor(vg, nvgColorU32(shape->fill.color));
            else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
                nvgSVGLinearGrad(vg, shape, 1);
            else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
                nvgSVGRadialGrad(vg, shape, 1);
            if (NSVG_PAINT_NONE != shape->fill.type)
                nvgFill(vg);
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
    }
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
const char g_header[] =
    "#include <math.h>\n"
    "#include \"nanovg.h\"\n"
    "#define NANOSVG_ALL_COLOR_KEYWORDS\n"
    "#include \"nanosvg.h\"\n"
    "\n"
    "char *lvgGetFileContents(const char *fname, int *size);\n"
    "void lvgFree(void *buf);\n"
    "void lvgDrawSVG(NSVGimage *image);\n"
    "NSVGimage *lvgLoadSVG(const char *file);\n"
    "\n"
    "extern NVGcontext *vg;\n"
    "extern NVGcolor g_bgColor;\n"
    "extern int winWidth;\n"
    "extern int winHeight;\n"
    "extern int width;\n"
    "extern int height;\n"
    "extern int mkeys;\n"
    "extern double mx;\n"
    "extern double my;\n"
    "extern double g_time;\n"
    "\n";

struct SYM
{
    const char *m_name;
    void *m_sym;
};

const struct SYM g_syms[] = {
    { "lvgDrawSVG", lvgDrawSVG },
    { "lvgLoadSVG", lvgLoadSVG },
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
    source = malloc(strlen(g_header) + strlen(buf) + 1);
    source[0] = 0;
    strcat(source, g_header);
    strcat(source, buf);
    free(buf);

    s = tcc_new();
    tcc_set_error_func(s, 0, tcc_error_func);
    //tcc_set_options(s, "-g");
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
    tcc_add_include_path(s, ".");
    tcc_add_include_path(s, "./include");
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
    int err;
    if ((g_zip = zip_open(file, 0, &err)) == NULL)
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
        printf("error");
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
        printf("eror: could not open window\n");
        glfwTerminate();
        return -1;
    }

#ifndef EMSCRIPTEN
    glfwSwapInterval(1);
#endif
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

#ifdef EMSCRIPTEN
    vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#else
    vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
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
    zip_close(g_zip);

    glfwTerminate();
    return 0;
}
