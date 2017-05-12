#include <stddef.h>
#include <stdlib.h>
#include "lunzip.h"
#include "nanovg.h"
#include "nanosvg.h"
#include "lvg_header.h"
#include "minimp3.h"

short *lvgLoadMP3Buf(const char *buf, uint32_t buf_size, int *rate, int *channels, int *nsamples)
{
    mp3_info_t info;
    mp3_decoder_t dec = mp3_create();
    int alloc_samples = 1024*1024, num_samples = 0;
    short *music_buf = (short *)malloc(alloc_samples*2);
    while (buf_size)
    {
        short frame_buf[MP3_MAX_SAMPLES_PER_FRAME];
        int frame_size = mp3_decode(dec, (short *)buf, buf_size, frame_buf, &info);
        if (!frame_size)
            break;
        int samples = info.audio_bytes/(info.channels*2);
        if (alloc_samples < (num_samples + samples))
        {
            alloc_samples *= 2;
            music_buf = (short *)realloc(music_buf, alloc_samples*2);
        }
        if (2 == info.channels)
        {   // TODO: joint stereo not supported?
            for (int i = 0; i < samples; i++)
                music_buf[num_samples + i] = frame_buf[i*2];
        } else
        {
            for (int i = 0; i < samples; i++)
                music_buf[num_samples + i] = frame_buf[i];
        }
        buf += frame_size;
        buf_size -= frame_size;
        num_samples += samples;
    }
    mp3_done(dec);
    if (alloc_samples > num_samples)
        music_buf = (short *)realloc(music_buf, num_samples*2);
    if (rate)
        *rate = info.sample_rate;
    if (channels)
        *channels = 1;//info.channels;
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
