#include <config.h>
#if ENABLE_SCRIPT && SCRIPT_TCC
#include <libtcc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include "all.h"
#include "all_lib.h"

#define GL_GLEXT_PROTOTYPES
#ifdef __MINGW32__
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include "render/render.h"

static int tcc_buf_pos;
static size_t tcc_buf_size;
static char *tcc_buf;

extern void (*onInit)();
extern void (*onFrame)();

int open_wrapper(const char *pathname, int flags)
{
    if (!strcmp(pathname, "./lib/libtcc1.a"))
    {
        tcc_buf_pos = 0;
        tcc_buf_size = sizeof(lib_libtcc1_a);
        tcc_buf = (char *)lib_libtcc1_a;
        return INT_MAX;
    }
    char *e = strrchr(pathname, '.');
    if (e && !strcmp(e, ".h"))
    {
        uint32_t size;
        char *file = lvgGetFileContents(pathname, &size);
        if (!file)
            return -1;
        tcc_buf_pos = 0;
        tcc_buf_size = size;
        tcc_buf = file;
        return INT_MAX - 1;
    }
    return open(pathname, flags);
}

void close_wrapper(int fd)
{
    if (INT_MAX == fd)
        return;
    if ((INT_MAX - 1) == fd)
    {
        free(tcc_buf);
        tcc_buf = 0;
        return;
    }
    close(fd);
}

ssize_t read_wrapper(int fd, void *buf, size_t count)
{
    if (INT_MAX != fd && (INT_MAX - 1) != fd)
        return read(fd, buf, count);
    size_t rest = tcc_buf_size - tcc_buf_pos;
    size_t to_read = count < rest ? count : rest;
    memcpy(buf, tcc_buf + tcc_buf_pos, to_read);
    tcc_buf_pos += to_read;
    return to_read;
}

off_t lseek_wrapper(int fd, off_t offset, int whence)
{
    if (INT_MAX != fd && (INT_MAX - 1) != fd)
        return lseek(fd, offset, whence);
    switch (whence)
    {
    case SEEK_SET: tcc_buf_pos = offset; break;
    case SEEK_CUR: tcc_buf_pos += offset; break;
    case SEEK_END: tcc_buf_pos = tcc_buf_size + offset; break;
    }
    return tcc_buf_pos;
}

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
    { "lvgLoadClip", lvgLoadClip },
    { "lvgGetFileContents", lvgGetFileContents },
    { "lvgFree", lvgFree },
    //{ "lvgStartAudio", lvgStartAudio },
    { "lvgPlaySound", lvgPlaySound },
    { "lvgLoadMP3", lvgLoadMP3 },
    { "lvgLoadMP3Buf", lvgLoadMP3Buf },
    { "printf", printf },

    { "malloc", malloc },
    { "calloc", calloc },
    { "realloc", realloc },
    { "free", free },

    { "atof", atof },

    { "time", time },
    { "localtime", localtime },

    { "sin", sin },
    { "cos", cos },
    { "tan", tan },
    { "sinh", sinh },
    { "cosh", cosh },
    { "tanh", tanh },
    { "asin", asin },
    { "acos", acos },
    { "atan", atan },
    { "atan2", atan2 },
    { "exp", exp },
    { "log", log },
    { "log10", log10 },
    { "pow", pow },
    { "sqrt", sqrt },
    { "ceil", ceil },
    { "floor", floor },
    { "fabs", fabs },
    { "ldexp", ldexp },
    { "frexp", frexp },
    { "modf", modf },
    { "fmod", fmod },

    { "sinf", sinf },
    { "cosf", cosf },
    { "tanf", tanf },
    { "sinhf", sinhf },
    { "coshf", coshf },
    { "tanhf", tanhf },
    { "asinf", asinf },
    { "acosf", acosf },
    { "atanf", atanf },
    { "atan2f", atan2f },
    { "expf", expf },
    { "logf", logf },
    { "log10f", log10f },
    { "powf", powf },
    { "sqrtf", sqrtf },
    { "ceilf", ceilf },
    { "floorf", floorf },
    { "fabsf", fabsf },
    { "ldexpf", ldexpf },
    { "frexpf", frexpf },
    { "modff", modff },
    { "fmodf", fmodf },

    { "nvgScale", nvgScale },
    { "nvgSave", nvgSave },
    { "nvgBeginPath", nvgBeginPath },
    { "nvgRoundedRect", nvgRoundedRect },
    { "nvgRGB", nvgRGB },
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

    { "glCreateProgram", glCreateProgram },
    { "glCreateShader", glCreateShader },
    { "glShaderSource", glShaderSource },
    { "glCompileShader", glCompileShader },
    { "glGetShaderiv", glGetShaderiv },
    { "glGetShaderInfoLog", glGetShaderInfoLog },
    { "glAttachShader", glAttachShader },
    { "glLinkProgram", glLinkProgram },
    { "glGetProgramiv", glGetProgramiv },
    { "glGetUniformLocation", glGetUniformLocation },
    { "glGetAttribLocation", glGetAttribLocation },
    { "glGenBuffers", glGenBuffers },
#ifndef __APPLE__
    { "glGenVertexArrays", glGenVertexArrays },
    { "glBindVertexArray", glBindVertexArray },
#endif
    { "glBindBuffer", glBindBuffer },
    { "glEnableVertexAttribArray", glEnableVertexAttribArray },
    { "glVertexAttribPointer", glVertexAttribPointer },
    { "glBindTexture", glBindTexture },
    { "glGenTextures", glGenTextures },
    { "glTexParameteri", glTexParameteri },
    { "glTexImage2D", glTexImage2D },
    { "glDetachShader", glDetachShader },
    { "glDeleteShader", glDeleteShader },
    { "glDeleteProgram", glDeleteProgram },
    { "glDeleteTextures", glDeleteTextures },
    { "glDeleteBuffers", glDeleteBuffers },
    { "glEnable", glEnable },
    { "glBlendEquation", glBlendEquation },
    { "glBlendFunc", glBlendFunc },
    { "glDisable", glDisable },
    { "glActiveTexture", glActiveTexture },
    { "glUseProgram", glUseProgram },
    { "glUniform1i", glUniform1i },
    { "glUniformMatrix4fv", glUniformMatrix4fv },
    { "glViewport", glViewport },
    { "glBufferData", glBufferData },
    { "glMapBuffer", glMapBuffer },
#ifndef __APPLE__
    { "glMapBufferRange", glMapBufferRange },
#endif
    { "glUnmapBuffer", glUnmapBuffer },
    { "glScissor", glScissor },
    { "glDrawElements", glDrawElements },
    { "glPushAttrib", glPushAttrib },
    { "glViewport", glViewport },
    { "glMatrixMode", glMatrixMode },
    { "glPushMatrix", glPushMatrix },
    { "glLoadIdentity", glLoadIdentity },
    { "glOrtho", glOrtho },
    { "glEnableClientState", glEnableClientState },

    //{ "vg", &vg },
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
    source = malloc(strlen((char *)allh_h) + strlen(buf) + 1);
    source[0] = 0;
    strcat(source, (char *)allh_h);
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
