#pragma once

#define MOUSE_BUTTON_LEFT    1
#define MOUSE_BUTTON_RIGHT   2
#define MOUSE_BUTTON_MIDDLE  4

typedef struct platform_params
{
    int winWidth, winHeight;
    int width, height;
    double mx, my;
    double frame_time;
    int mkeys;
    int last_mkeys;
} platform_params;

typedef struct platform
{
    int (*init)(void **ctx, platform_params *params, int audio_only);
    void (*release)(void *ctx);
    void (*pull_events)(void *ctx);
    void (*main_loop)(void *ctx);
    void (*swap_buffers)(void *ctx);
    void (*fullscreen)(void *ctx, int b_fullscreen);
    double (*get_time)(void *ctx);
    int (*get_key)(void *ctx, int key);
    void *(*get_proc_address)(const char *procname);
    int (*extension_supported)(const char *ext);
} platform;

extern platform_params g_params;
