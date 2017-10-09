#include <config.h>
#if PLATFORM_SDL || (ENABLE_AUDIO && AUDIO_SDL)
#include <platform/platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

void drawframe();

typedef struct platform_ctx
{
    SDL_Window *window;
    platform_params *params;
    int winX, winY;
    int defWidth, defHeight;
    char keys[256];
} platform_ctx;

static int sdl_init(void **ctx, platform_params *params, int audio_only)
{
    *ctx = 0;
#ifdef __MINGW32__
    putenv("SDL_AUDIODRIVER=winmm");
#endif
    int flags = audio_only ? SDL_INIT_AUDIO : SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC);
    if (SDL_Init(flags) < 0)
    {
        printf("error: sdl2 init failed: %s\n", SDL_GetError());
        return 0;
    }
#if PLATFORM_SDL
    platform_ctx *platform = (platform_ctx *)calloc(1, sizeof(platform_ctx));
    platform->params = params;
    platform->window = SDL_CreateWindow(title, 0, 0, state->window_w, state->window_h, state->window_flags);
#ifndef EMSCRIPTEN
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("error: glad init failed\n");
        return 0;
    }
#endif
    *ctx = platform;
#endif
    return 1;
}

static void sdl_release(void *ctx)
{
#if PLATFORM_SDL
    platform_ctx *platform = (platform_ctx *)ctx;
    free(platform);
#endif
    SDL_Quit();
}

#if PLATFORM_SDL
static void sdl_pull_events(void *ctx, platform_params *params)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    //SDL_Event event;
    //SDL_PollEvent(&event);
}

static void sdl_main_loop(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
}

static void sdl_swap_buffers(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    SDL_GL_SwapWindow(platform->window);
}

static void sdl_fullscreen(void *ctx, int b_fullscreen)
{
}

static double sdl_get_time(void *ctx)
{
    return 0.0;
}

static int sdl_get_key(void *ctx, int key)
{
}

static void *sdl_get_proc_address(const char *procname)
{
}

static int sdl_extension_supported(const char *ext)
{
}
#endif

const platform sdl_platform =
{
    sdl_init,
    sdl_release,
#if PLATFORM_SDL
    sdl_pull_events,
    sdl_main_loop,
    sdl_swap_buffers,
    sdl_fullscreen,
    sdl_get_time,
    sdl_get_key,
    sdl_get_proc_address,
    sdl_extension_supported
#else
    0, 0, 0, 0, 0, 0, 0, 0
#endif
};
#endif
