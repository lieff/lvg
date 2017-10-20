#include <config.h>
#include <audio/audio.h>
#if ENABLE_AUDIO && !defined(_TEST)
#include <stddef.h>
#include <stdlib.h>
#include <sys/param.h>
#include "lunzip.h"
#include "mp3_keyj/minimp3.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern const audio_render *g_audio_render;
extern void *g_audio_render_obj;

short *lvgLoadMP3Buf(const char *buf, uint32_t buf_size, int *rate, int *channels, int *nsamples)
{
    /*FILE *f = fopen("out.mp3", "wb");
    fwrite(buf, 1, buf_size, f);
    fclose(f);*/
    mp3_info_t info;
    mp3_decoder_t dec = mp3_create();
    int alloc_samples = 1024*1024, num_samples = 0;
    short *music_buf = (short *)malloc(alloc_samples*2*2);
    while (buf_size)
    {
        short frame_buf[MP3_MAX_SAMPLES_PER_FRAME];
        int frame_size = mp3_decode(dec, (short *)buf, buf_size, frame_buf, &info);
        assert(frame_size && info.audio_bytes > 0);
        if (frame_size <= 0 || info.audio_bytes <= 0)
            break;
        int samples = info.audio_bytes/(info.channels*2);
        if (alloc_samples < (num_samples + samples))
        {
            alloc_samples *= 2;
            music_buf = (short *)realloc(music_buf, alloc_samples*2*info.channels);
        }
        memcpy(music_buf + num_samples*info.channels, frame_buf, info.audio_bytes);
        buf += frame_size;
        buf_size -= frame_size;
        num_samples += samples;
    }
    mp3_done(dec);
    if (alloc_samples > num_samples)
        music_buf = (short *)realloc(music_buf, num_samples*2*info.channels);
    if (rate)
        *rate = info.sample_rate;
    if (channels)
        *channels = info.channels;
    if (num_samples)
        *nsamples = num_samples;
    return music_buf;
}

short *lvgLoadMP3(const char *file_name, int *rate, int *channels, int *num_samples)
{
    char *buf;
    uint32_t music_size;
    if ((buf = (char *)lvgGetFileContents(file_name, &music_size)))
    {
        short *ret = lvgLoadMP3Buf(buf, music_size, rate, channels, num_samples);
        free(buf);
        return ret;
    }
    return 0;
}

void lvgPlaySound(LVGSound *sound, int flags, int start_sample, int end_sample, int loops)
{
    g_audio_render->play(g_audio_render_obj, sound, flags, start_sample, end_sample, loops);
}

void lvgStopAudio()
{
    g_audio_render->stop_all(g_audio_render_obj);
}
#else
short *lvgLoadMP3Buf(const char *buf, uint32_t buf_size, int *rate, int *channels, int *nsamples)
{
    *nsamples = 0;
    return 0;
}

short *lvgLoadMP3(const char *file_name, int *rate, int *channels, int *num_samples)
{
    *num_samples = 0;
    return 0;
}

void lvgPlaySound(LVGSound *sound, int flags, int start_sample, int end_sample, int loops)
{
}

void lvgStopAudio()
{
}
#endif
