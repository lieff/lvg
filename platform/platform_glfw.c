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
#ifdef __MINGW32__
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

void drawframe();

typedef struct platform_ctx
{
    GLFWwindow *window;
    platform_params *params;
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
#ifdef __MINGW32__
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
    platform_ctx *platform = (platform_ctx *)ctx;
    platform_params *params = platform->params;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (b_fullscreen)
    {
        params->winWidth  = mode->width;
        params->winHeight = mode->height;
    } else
    {
        params->winWidth  = platform->defWidth;
        params->winHeight = platform->defHeight;
    }
    glfwSetWindowMonitor(platform->window, monitor, 0, 0, params->winWidth, params->winHeight, mode->refreshRate);
}

static double glfw_get_time(void *ctx)
{
    return glfwGetTime();
}

static int glfw_get_key(void *ctx, int key)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    return platform->keys[key];
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
    glfw_get_key
};
#endif
