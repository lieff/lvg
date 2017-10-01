#pragma once
#include <render/render.h>

#define AUDIO_NUM_CHANNELS 32
#define AUDIO_LOOP 1
#define AUDIO_FORMAT_FLOAT 1

typedef struct audio_render
{
    int (*init)(void **audio_render, int samplerate, int channels, int format, int buffer, int is_capture);
    void (*release)(void *audio_render);
    void (*play)(void *audio_render, LVGSound *sound, int flags);
    void (*stop)(void *audio_render, LVGSound *sound);
    void (*stop_all)(void *audio_render);
} audio_render;
