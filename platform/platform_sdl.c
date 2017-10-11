#include <config.h>
#if PLATFORM_SDL || (ENABLE_AUDIO && AUDIO_SDL)
#include <platform/platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

typedef void* (* GLADloadproc)(const char *name);
int gladLoadGLLoader(GLADloadproc);
void drawframe();

typedef struct platform_ctx
{
    SDL_Window *window;
    SDL_GLContext context;
    platform_params *params;
    int winX, winY, defWidth, defHeight, done;
    Uint32 startTime;
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
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    platform->window = SDL_CreateWindow("LVG Player", 0, 0, 1024, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    platform->context = SDL_GL_CreateContext(platform->window);
    SDL_GL_MakeCurrent(platform->window, platform->context);
#ifndef EMSCRIPTEN
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
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
    SDL_GL_DeleteContext(platform->context);
    SDL_DestroyWindow(platform->window);
#endif
    SDL_Quit();
#if PLATFORM_SDL
    free(platform);
#endif
}

#if PLATFORM_SDL
static void sdl_pull_events(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            platform->done = 1;
    }
}

static void sdl_main_loop(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    platform->startTime = SDL_GetTicks();
    platform->done = 0;
    while (!platform->done)
    {
        drawframe();
    }
}

static void sdl_swap_buffers(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    SDL_GL_SwapWindow(platform->window);
}

static void sdl_fullscreen(void *ctx, int b_fullscreen)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    SDL_SetWindowFullscreen(platform->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

static double sdl_get_time(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    return (SDL_GetTicks() - platform->startTime)/1000.0;
}

static int sdl_get_key(void *ctx, int key)
{
    return 0;
}

static void *sdl_get_proc_address(const char *procname)
{
    return SDL_GL_GetProcAddress(procname);
}

static int sdl_extension_supported(const char *ext)
{
    return SDL_GL_ExtensionSupported(ext);
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
