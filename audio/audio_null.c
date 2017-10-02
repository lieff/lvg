#include <config.h>
#include <audio/audio.h>

int null_audio_init(void **audio_render, int samplerate, int channels, int format, int buffer, int is_capture)
{
    *audio_render = (void *)1;
    return 0;
}

void null_audio_release(void *audio_render)
{
}

void null_audio_play(void *audio_render, LVGSound *sound, int flags)
{
}

void null_audio_stop(void *audio_render, LVGSound *sound)
{
}

void null_audio_stop_all(void *audio_render)
{
}

const audio_render null_audio_render =
{
    null_audio_init,
    null_audio_release,
    null_audio_play,
    null_audio_stop,
    null_audio_stop_all
};
