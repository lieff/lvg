#pragma once
#include <config.h>
#include <lvg_header.h>
#include <render/gl.h>
#include <video/video.h>
#include <audio/audio.h>
#include <render/render.h>
#include <platform/platform.h>
#include <lunzip.h>

struct LVGEngine
{
    const render *render;
    void *render_obj;
    const audio_render *audio_render;
    void *audio_render_obj;
    const platform *platform;
    void *platform_obj;
#if ENABLE_SCRIPT
    void *script;
#endif

    zip_t zip;
    LVGMovieClip *clip;
    LVGColorf bgColor;
    platform_params params;
    double last_click;
    int b_no_actionscript, b_fullscreen, b_interpolate, b_gles3;
    int last_enter;
};
