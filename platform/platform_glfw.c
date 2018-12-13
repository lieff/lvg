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

typedef struct platform_ctx
{
    GLFWwindow *window;
    platform_params *params;
    void (*onFrame)(void *);
    void *user;
    int winX, winY;
    int defWidth, defHeight;
    //char keys[256];
} platform_ctx;

/*void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    platform_ctx *platform = (platform_ctx *)glfwGetWindowUserPointer(window);
    platform->keys[scancode & 0xff] = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 1 : 0;
}*/

static int glfw_init(void **ctx, platform_params *params, void (*onFrame)(void *), void *user, int audio_only)
{
    *ctx = 0;
    if (!glfwInit())
    {
        printf("error: glfw init failed\n");
        return 0;
    }
    platform_ctx *platform = (platform_ctx *)calloc(1, sizeof(platform_ctx));
    platform->onFrame = onFrame;
    platform->user   = user;
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
    params->time = glfwGetTime();
}

static void glfw_main_loop(void *ctx)
{
    glfwSetTime(0);
    platform_ctx *platform = (platform_ctx *)ctx;
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(platform->onFrame, platform->user, 0, 1);
#else
    while (!glfwWindowShouldClose(platform->window))
    {
        platform->onFrame(platform->user);
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
    static const short key_lookup[] = {
        [KEY_A] = GLFW_KEY_A,
        [KEY_B] = GLFW_KEY_B,
        [KEY_C] = GLFW_KEY_C,
        [KEY_D] = GLFW_KEY_D,
        [KEY_E] = GLFW_KEY_E,
        [KEY_F] = GLFW_KEY_F,
        [KEY_G] = GLFW_KEY_G,
        [KEY_H] = GLFW_KEY_H,
        [KEY_I] = GLFW_KEY_I,
        [KEY_J] = GLFW_KEY_J,
        [KEY_K] = GLFW_KEY_K,
        [KEY_L] = GLFW_KEY_L,
        [KEY_M] = GLFW_KEY_M,
        [KEY_N] = GLFW_KEY_N,
        [KEY_O] = GLFW_KEY_O,
        [KEY_P] = GLFW_KEY_P,
        [KEY_Q] = GLFW_KEY_Q,
        [KEY_R] = GLFW_KEY_R,
        [KEY_S] = GLFW_KEY_S,
        [KEY_T] = GLFW_KEY_T,
        [KEY_U] = GLFW_KEY_U,
        [KEY_V] = GLFW_KEY_V,
        [KEY_W] = GLFW_KEY_W,
        [KEY_X] = GLFW_KEY_X,
        [KEY_Y] = GLFW_KEY_Y,
        [KEY_Z] = GLFW_KEY_Z,

        [KEY_1] = GLFW_KEY_1,
        [KEY_2] = GLFW_KEY_2,
        [KEY_3] = GLFW_KEY_3,
        [KEY_4] = GLFW_KEY_4,
        [KEY_5] = GLFW_KEY_5,
        [KEY_6] = GLFW_KEY_6,
        [KEY_7] = GLFW_KEY_7,
        [KEY_8] = GLFW_KEY_8,
        [KEY_9] = GLFW_KEY_9,
        [KEY_0] = GLFW_KEY_0,

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
        [KEY_HASHTILDE] =  GLFW_KEY_GRAVE_ACCENT,
        [KEY_SEMICOLON] =  GLFW_KEY_SEMICOLON,
        [KEY_APOSTROPHE] = GLFW_KEY_APOSTROPHE,
        [KEY_GRAVE] =      GLFW_KEY_GRAVE_ACCENT,
        [KEY_COMMA] =      GLFW_KEY_COMMA,
        [KEY_DOT] =        GLFW_KEY_PERIOD,
        [KEY_SLASH] =      GLFW_KEY_SLASH,
        [KEY_CAPSLOCK] =   GLFW_KEY_CAPS_LOCK,

        [KEY_F1] =  GLFW_KEY_F1,
        [KEY_F2] =  GLFW_KEY_F2,
        [KEY_F3] =  GLFW_KEY_F3,
        [KEY_F4] =  GLFW_KEY_F4,
        [KEY_F5] =  GLFW_KEY_F5,
        [KEY_F6] =  GLFW_KEY_F6,
        [KEY_F7] =  GLFW_KEY_F7,
        [KEY_F8] =  GLFW_KEY_F8,
        [KEY_F9] =  GLFW_KEY_F9,
        [KEY_F10] = GLFW_KEY_F10,
        [KEY_F11] = GLFW_KEY_F11,
        [KEY_F12] = GLFW_KEY_F12,

        [KEY_SYSRQ] =      GLFW_KEY_PRINT_SCREEN,
        [KEY_SCROLLLOCK] = GLFW_KEY_SCROLL_LOCK,
        [KEY_PAUSE] =      GLFW_KEY_PAUSE,
        [KEY_INSERT] =     GLFW_KEY_INSERT,
        [KEY_HOME] =       GLFW_KEY_HOME,
        [KEY_PAGEUP] =     GLFW_KEY_PAGE_UP,
        [KEY_DELETE] =     GLFW_KEY_DELETE,
        [KEY_END] =        GLFW_KEY_END,
        [KEY_PAGEDOWN] =   GLFW_KEY_PAGE_DOWN,
        [KEY_RIGHT] =      GLFW_KEY_RIGHT,
        [KEY_LEFT] =       GLFW_KEY_LEFT,
        [KEY_DOWN] =       GLFW_KEY_DOWN,
        [KEY_UP] =         GLFW_KEY_UP,

        [KEY_NUMLOCK] =    GLFW_KEY_NUM_LOCK,
        [KEY_KPSLASH] =    GLFW_KEY_KP_DIVIDE,
        [KEY_KPASTERISK] = GLFW_KEY_KP_MULTIPLY,
        [KEY_KPMINUS] =    GLFW_KEY_KP_SUBTRACT,
        [KEY_KPPLUS] =     GLFW_KEY_KP_ADD,
        [KEY_KPENTER] =    GLFW_KEY_KP_ENTER,
        [KEY_KP1] =   GLFW_KEY_KP_1,
        [KEY_KP2] =   GLFW_KEY_KP_2,
        [KEY_KP3] =   GLFW_KEY_KP_3,
        [KEY_KP4] =   GLFW_KEY_KP_4,
        [KEY_KP5] =   GLFW_KEY_KP_5,
        [KEY_KP6] =   GLFW_KEY_KP_6,
        [KEY_KP7] =   GLFW_KEY_KP_7,
        [KEY_KP8] =   GLFW_KEY_KP_8,
        [KEY_KP9] =   GLFW_KEY_KP_9,
        [KEY_KP0] =   GLFW_KEY_KP_0,
        //[KEY_KPDOT] = GLFW_KEY_KP_DECIMAL,

        //[KEY_102ND] =   0x64,
        //[KEY_COMPOSE] = 0x65,
        //[KEY_POWER] =   0x66,
        [KEY_KPEQUAL] = GLFW_KEY_KP_EQUAL,

        [KEY_F13] = GLFW_KEY_F13,
        [KEY_F14] = GLFW_KEY_F14,
        [KEY_F15] = GLFW_KEY_F15,
        [KEY_F16] = GLFW_KEY_F16,
        [KEY_F17] = GLFW_KEY_F17,
        [KEY_F18] = GLFW_KEY_F18,
        [KEY_F19] = GLFW_KEY_F19,
        [KEY_F20] = GLFW_KEY_F20,
        [KEY_F21] = GLFW_KEY_F21,
        [KEY_F22] = GLFW_KEY_F22,
        [KEY_F23] = GLFW_KEY_F23,
        [KEY_F24] = GLFW_KEY_F24,

        [KEY_KPCOMMA] = GLFW_KEY_KP_DECIMAL,

        [KEY_LEFTCTRL] =   GLFW_KEY_LEFT_CONTROL,
        [KEY_LEFTSHIFT] =  GLFW_KEY_LEFT_SHIFT,
        [KEY_LEFTALT] =    GLFW_KEY_LEFT_ALT,
        [KEY_LEFTMETA] =   GLFW_KEY_LEFT_SUPER,
        [KEY_RIGHTCTRL] =  GLFW_KEY_RIGHT_CONTROL,
        [KEY_RIGHTSHIFT] = GLFW_KEY_RIGHT_SHIFT,
        [KEY_RIGHTALT] =   GLFW_KEY_RIGHT_ALT,
        [KEY_RIGHTMETA] =  GLFW_KEY_RIGHT_SUPER
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
