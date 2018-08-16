#include <config.h>
#if PLATFORM_SDL || (ENABLE_AUDIO && AUDIO_SDL)
#include <platform/platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

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
    char keys[SDL_NUM_SCANCODES];
} platform_ctx;

static void sdl_release(void *ctx)
{
#if PLATFORM_SDL
    platform_ctx *platform = (platform_ctx *)ctx;
    if (platform->context)
        SDL_GL_DeleteContext(platform->context);
    if (platform->window)
        SDL_DestroyWindow(platform->window);
#endif
    SDL_Quit();
#if PLATFORM_SDL
    free(platform);
#endif
}

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
    /*SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);*/
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#ifndef EMSCRIPTEN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    platform->window = SDL_CreateWindow("LVG Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 800,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!platform->window)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
        platform->window = SDL_CreateWindow("LVG Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 800,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
        if (!platform->window)
        {
            printf("error: sdl2 init failed: %s\n", SDL_GetError()); fflush(stdout);
            sdl_release(platform);
            return 0;
        }
    }
    platform->context = SDL_GL_CreateContext(platform->window);
    SDL_GL_MakeCurrent(platform->window, platform->context);
#ifndef EMSCRIPTEN
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        printf("error: glad init failed\n");
        sdl_release(platform);
        return 0;
    }
#endif
    *ctx = platform;
#endif
    return 1;
}

#if PLATFORM_SDL
static double sdl_get_time(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    return (SDL_GetTicks() - platform->startTime)/1000.0;
}

static void sdl_pull_events(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    platform_params *params = platform->params;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_MOUSEMOTION:
            params->mx = event.motion.x;
            params->my = event.motion.y;
            break;
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.state == SDL_PRESSED)
                params->mkeys |= 1 << (event.button.button - 1);
            else
                params->mkeys &= ~(1 << (event.button.button - 1));
            break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            platform->keys[event.key.keysym.scancode] = event.key.state;
            break;
        case SDL_QUIT:
            platform->done = 1;
            break;
        }
    }
    SDL_GetWindowSize(platform->window, &params->winWidth, &params->winHeight);
    SDL_GL_GetDrawableSize(platform->window, &params->width, &params->height);
    params->time = sdl_get_time(ctx);
}

static void sdl_main_loop(void *ctx)
{
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(drawframe, 0, 1);
#else
    platform_ctx *platform = (platform_ctx *)ctx;
    platform->startTime = SDL_GetTicks();
    while (!platform->done)
    {
        drawframe();
        if (platform->keys[KEY_ESC])
            platform->done = 1;
    }
#endif
}

static void sdl_set_exit(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    platform->done = 1;
}

static void sdl_swap_buffers(void *ctx)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    SDL_GL_SwapWindow(platform->window);
}

static void sdl_fullscreen(void *ctx, int b_fullscreen)
{
    platform_ctx *platform = (platform_ctx *)ctx;

    /*SDL_Rect bounds, usablebounds;
    float hdpi = 0;
    float vdpi = 0;
    SDL_DisplayMode mode;
    uint32_t Rmask, Gmask, Bmask, Amask;
    int bpp, i, j, m, n = SDL_GetNumVideoDisplays();
    SDL_Log("Number of displays: %d\n", n);
    for (i = 0; i < n; ++i)
    {
        SDL_Log("Display %d: %s\n", i, SDL_GetDisplayName(i));

        SDL_zero(bounds);
        SDL_GetDisplayBounds(i, &bounds);

        SDL_zero(usablebounds);
        //SDL_GetDisplayUsableBounds(i, &usablebounds);

        SDL_GetDisplayDPI(i, NULL, &hdpi, &vdpi);

        SDL_Log("Bounds: %dx%d at %d,%d\n", bounds.w, bounds.h, bounds.x, bounds.y);
        //SDL_Log("Usable bounds: %dx%d at %d,%d\n", usablebounds.w, usablebounds.h, usablebounds.x, usablebounds.y);
        SDL_Log("DPI: %fx%f\n", hdpi, vdpi);

        SDL_GetDesktopDisplayMode(i, &mode);
        SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask, &Gmask,
                                   &Bmask, &Amask);
        SDL_Log("  Current mode: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
                mode.w, mode.h, mode.refresh_rate, bpp,
                SDL_GetPixelFormatName(mode.format));
        if (Rmask || Gmask || Bmask)
        {
            SDL_Log("      Red Mask   = 0x%.8x\n", Rmask);
            SDL_Log("      Green Mask = 0x%.8x\n", Gmask);
            SDL_Log("      Blue Mask  = 0x%.8x\n", Bmask);
            if (Amask)
                SDL_Log("      Alpha Mask = 0x%.8x\n", Amask);
        }

        m = SDL_GetNumDisplayModes(i);
        if (m == 0)
        {
            SDL_Log("No available fullscreen video modes\n");
        } else
        {
            SDL_Log("  Fullscreen video modes:\n");
            for (j = 0; j < m; ++j)
            {
                SDL_GetDisplayMode(i, j, &mode);
                SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask,
                                           &Gmask, &Bmask, &Amask);
                SDL_Log("    Mode %d: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
                        j, mode.w, mode.h, mode.refresh_rate, bpp,
                        SDL_GetPixelFormatName(mode.format));
                if (Rmask || Gmask || Bmask) {
                    SDL_Log("        Red Mask   = 0x%.8x\n", Rmask);
                    SDL_Log("        Green Mask = 0x%.8x\n", Gmask);
                    SDL_Log("        Blue Mask  = 0x%.8x\n", Bmask);
                    if (Amask)
                        SDL_Log("        Alpha Mask = 0x%.8x\n", Amask);
                }
                if (b_fullscreen && 1024 == mode.w && 32 == bpp)
                {
                    SDL_SetWindowDisplayMode(platform->window, &mode);
                    SDL_SetWindowFullscreen(platform->window, SDL_WINDOW_FULLSCREEN);
                    return;
                }
            }
        }
    }*/

    SDL_SetWindowFullscreen(platform->window, b_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

static int sdl_get_key(void *ctx, int key)
{
    platform_ctx *platform = (platform_ctx *)ctx;
    return platform->keys[key];
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
    sdl_set_exit,
    sdl_swap_buffers,
    sdl_fullscreen,
    sdl_get_time,
    sdl_get_key,
    sdl_get_proc_address,
    sdl_extension_supported
#else
    0, 0, 0, 0, 0, 0, 0, 0, 0
#endif
};
#endif
