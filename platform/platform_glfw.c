#include <config.h>
#if PLATFORM_GLFW
#include <platform/platform.h>
#include <stdio.h>
#include <stdlib.h>
#define GL_GLEXT_PROTOTYPES
#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>

typedef void* (* GLADloadproc)(const char *name);
int gladLoadGLLoader(GLADloadproc);
void drawframe();

typedef struct platform_ctx
{
    GLFWwindow *window;
    platform_params *params;
    int winX, winY;
    int defWidth, defHeight;
    char keys[256];
} platform_ctx;

/*void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    platform_ctx *platform = (platform_ctx *)glfwGetWindowUserPointer(window);
    platform->keys[scancode & 0xff] = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 1 : 0;
}*/

static int glfw_init(void **ctx, platform_params *params, int audio_only)
{
    *ctx = 0;
    if (!glfwInit())
    {
        printf("error: glfw init failed\n");
        return 0;
    }
    platform_ctx *platform = (platform_ctx *)calloc(1, sizeof(platform_ctx));
    platform->params = params;

    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_SAMPLES, 8);
#ifdef EMSCRIPTEN
    platform->defWidth = 1024, platform->defHeight = 800;
#else
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    platform->defWidth  = mode->width - 40;
    platform->defHeight = mode->height - 80;
    if (platform->defWidth > 1024)
        platform->defWidth = 1024;
    if (platform->defHeight > 800)
        platform->defHeight = 800;
#endif
    platform->window = glfwCreateWindow(platform->defWidth, platform->defHeight, "LVG Player", NULL, NULL);
    if (!platform->window)
    {
        free(platform);
        printf("error: could not open window\n");
        return 0;
    }

#ifndef EMSCRIPTEN
    glfwSwapInterval(1);
#endif
    glfwMakeContextCurrent(platform->window);
    glfwSetInputMode(platform->window, GLFW_STICKY_MOUSE_BUTTONS, 1);
    glfwSetInputMode(platform->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //glfwSetWindowUserPointer(platform->window, (void*)platform);
    //glfwSetKeyCallback(platform->window, key_callback);
#ifndef EMSCRIPTEN
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("error: glad init failed\n");
        return 0;
    }
#endif
    *ctx = platform;
    return 1;
}

static void glfw_release(void *ctx)
{
    glfwTerminate();
}

static void glfw_pull_events(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    platform_params *params = platform->params;
    glfwPollEvents();
    glfwGetWindowSize(platform->window, &params->winWidth, &params->winHeight);
    glfwGetFramebufferSize(platform->window, &params->width, &params->height);
    glfwGetCursorPos(platform->window, &params->mx, &params->my);
    params->mkeys = 0;
    params->mkeys |= glfwGetMouseButton(platform->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? MOUSE_BUTTON_LEFT : 0;
    params->mkeys |= glfwGetMouseButton(platform->window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ? MOUSE_BUTTON_MIDDLE : 0;
    params->mkeys |= glfwGetMouseButton(platform->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ? MOUSE_BUTTON_RIGHT : 0;
    params->frame_time = glfwGetTime();
}

static void glfw_main_loop(void *ctx)
{
#ifdef EMSCRIPTEN
    glfwSetTime(0);
    emscripten_set_main_loop(drawframe, 0, 1);
#else
    platform_ctx *platform = (platform_ctx *)ctx;
    glfwSetTime(0);
    while (!glfwWindowShouldClose(platform->window))
    {
        drawframe();
        if (glfwGetKey(platform->window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(platform->window, 1);
    }
#endif
}

static void glfw_swap_buffers(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    glfwSwapBuffers(platform->window);
}

static void glfw_fullscreen(void *ctx, int b_fullscreen)
{
#ifndef EMSCRIPTEN
    platform_ctx *platform = (platform_ctx *)ctx;
    platform_params *params = platform->params;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (b_fullscreen)
    {
        glfwGetWindowPos(platform->window, &platform->winX, &platform->winY);
        params->winWidth  = mode->width;
        params->winHeight = mode->height;
    } else
    {
        params->winWidth  = platform->defWidth;
        params->winHeight = platform->defHeight;
    }
    glfwSetWindowMonitor(platform->window, b_fullscreen ? monitor : 0, b_fullscreen ? 0 : platform->winX,
        b_fullscreen ? 0 : platform->winY, params->winWidth, params->winHeight, b_fullscreen ? mode->refreshRate : 0);
#endif
}

static double glfw_get_time(void *ctx)
{
    return glfwGetTime();
}

static int glfw_get_key(void *ctx, int key)
{
    static const int key_lookup[] = {
        [KEY_ENTER] =      GLFW_KEY_ENTER,
        [KEY_ESC] =        GLFW_KEY_ESCAPE,
        [KEY_BACKSPACE] =  GLFW_KEY_BACKSPACE,
        [KEY_TAB] =        GLFW_KEY_TAB,
        [KEY_SPACE] =      GLFW_KEY_SPACE,
        [KEY_MINUS] =      GLFW_KEY_MINUS,
        [KEY_EQUAL] =      GLFW_KEY_EQUAL,
        [KEY_LEFTBRACE] =  GLFW_KEY_LEFT_BRACKET,
        [KEY_RIGHTBRACE] = GLFW_KEY_RIGHT_BRACKET,
        [KEY_BACKSLASH] =  GLFW_KEY_BACKSLASH,
        //[KEY_HASHTILDE] =  0x32,
        [KEY_SEMICOLON] =  GLFW_KEY_SEMICOLON,
        [KEY_APOSTROPHE] = GLFW_KEY_APOSTROPHE,
        [KEY_GRAVE] =      GLFW_KEY_GRAVE_ACCENT,
        [KEY_COMMA] =      GLFW_KEY_COMMA,
        [KEY_DOT] =        GLFW_KEY_PERIOD,
        [KEY_SLASH] =      GLFW_KEY_SLASH,
        [KEY_CAPSLOCK] =   GLFW_KEY_CAPS_LOCK,

        [KEY_LEFTCTRL] = GLFW_KEY_LEFT_CONTROL,
        [KEY_LEFTSHIFT] = GLFW_KEY_LEFT_SHIFT,
        [KEY_LEFTALT] = GLFW_KEY_LEFT_ALT,
        [KEY_LEFTMETA] = GLFW_KEY_LEFT_SUPER,
        [KEY_RIGHTCTRL] = GLFW_KEY_RIGHT_CONTROL,
        [KEY_RIGHTSHIFT] = GLFW_KEY_RIGHT_SHIFT,
        [KEY_RIGHTALT] = GLFW_KEY_RIGHT_ALT,
        [KEY_RIGHTMETA] = GLFW_KEY_RIGHT_SUPER
    };
    platform_ctx *platform = (platform_ctx *)ctx;
    if (key < 0 || key > sizeof(key_lookup)/sizeof(key_lookup[0]))
        return 0;
    return glfwGetKey(platform->window, key_lookup[key]);
}

static void *glfw_get_proc_address(const char *procname)
{
    return glfwGetProcAddress(procname);
}

static int glfw_extension_supported(const char *ext)
{
    return glfwExtensionSupported(ext);
}

const platform glfw_platform =
{
    glfw_init,
    glfw_release,
    glfw_pull_events,
    glfw_main_loop,
    glfw_swap_buffers,
    glfw_fullscreen,
    glfw_get_time,
    glfw_get_key,
    glfw_get_proc_address,
    glfw_extension_supported
};
#endif
