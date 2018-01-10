#include <config.h>
#include <audio/audio.h>
#if ENABLE_AUDIO && !defined(_TEST)
#include <stddef.h>
#include <stdlib.h>
#include <sys/param.h>
#include "lunzip.h"
#include "mp3/minimp3.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern const audio_render *g_audio_render;
extern void *g_audio_render_obj;

short *lvgLoadMP3Buf(const unsigned char *buf, uint32_t buf_size, int *rate, int *channels, int *nsamples)
{
    /*FILE *f = fopen("out.mp3", "wb");
    fwrite(buf, 1, buf_size, f);
    fclose(f);*/
    mp3dec_frame_info_t info;
    mp3dec_t dec;
    int alloc_samples = 1024*1024, num_samples = 0;
    short *music_buf = (short *)malloc(alloc_samples*2*2);
    mp3dec_init(&dec);
    while (buf_size)
    {
        short frame_buf[2*1152];
        int samples = mp3dec_decode_frame(&dec, buf, buf_size, frame_buf, &info);
        assert(samples && info.frame_bytes > 0);
        if (samples <= 0 || info.frame_bytes <= 0)
            break;
        if (alloc_samples < (num_samples + samples))
        {
            alloc_samples *= 2;
            music_buf = (short *)realloc(music_buf, alloc_samples*2*info.channels);
        }
        memcpy(music_buf + num_samples*info.channels, frame_buf, samples*info.channels*2);
        buf += info.frame_bytes;
        buf_size -= info.frame_bytes;
        num_samples += samples;
    }
    if (alloc_samples > num_samples)
        music_buf = (short *)realloc(music_buf, num_samples*2*info.channels);
    if (rate)
        *rate = info.hz;
    if (channels)
        *channels = info.channels;
    if (num_samples)
        *nsamples = num_samples;
    return music_buf;
}

short *lvgLoadMP3(const char *file_name, int *rate, int *channels, int *num_samples)
{
    unsigned char *buf;
    uint32_t music_size;
    if ((buf = (unsigned char *)lvgGetFileContents(file_name, &music_size)))
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
short *lvgLoadMP3Buf(const unsigned char *buf, uint32_t buf_size, int *rate, int *channels, int *nsamples)
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
