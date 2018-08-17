#pragma once
#include <lvg_header.h>

#define AUDIO_NUM_CHANNELS 32
#define AUDIO_FORMAT_FLOAT 1

#define PLAY_SyncStop    32 //Stop the sound now.
#define PLAY_SyncNoMultiple 16 //Don’t start the sound if already playing.
#define PLAY_HasEnvelope 8  //Has envelope information.
#define PLAY_HasLoops    4  //Has loop information.
#define PLAY_HasOutPoint 2  //Has out-point information.
#define PLAY_HasInPoint  1  //Has in-point information.

typedef struct audio_render
{
    int (*init)(void **audio_render, int samplerate, int channels, int format, int buffer, int is_capture);
    void (*release)(void *audio_render);
    void (*play)(void *audio_render, LVGSound *sound, int flags, int start_sample, int end_sample, int loops);
    void (*stop_all)(void *audio_render);
    void (*resample)(void *audio_render, LVGSound *sound);
} audio_render;
