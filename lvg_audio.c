#include <stddef.h>
#include <stdlib.h>
#include <sys/param.h>
#include "lunzip.h"
#include "nanovg.h"
#include "nanosvg.h"
#include "lvg_header.h"
#include "minimp3.h"
#include <SDL2/SDL.h>
//#include "resample.h"

//static struct rs_data *g_resample;
SDL_AudioCVT g_cvt, g_cvt_record;
SDL_AudioCallback g_audio_cb;
void *g_audio_cb_user_data;
SDL_AudioCallback g_record_cb;
void *g_record_cb_user_data;
int g_dev = -1, g_dev_record = -1;

short *lvgLoadMP3Buf(const char *buf, uint32_t buf_size, int *rate, int *channels, int *nsamples)
{
    /*FILE *f = fopen("out.mp3", "wb");
    fwrite(buf, 1, buf_size, f);
    fclose(f);*/
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

static void audio_cb(void *udata, Uint8 *stream, int len)
{
    if (g_cvt.needed)
    {
        g_cvt.len = ((int)(len/g_cvt.len_ratio) + 1) & (~1);
        if (!g_cvt.buf)
            g_cvt.buf = malloc(g_cvt.len*g_cvt.len_mult);
        while (len)
        {
            if (!g_cvt.len_cvt && g_cvt.len)
            {
                g_audio_cb(g_audio_cb_user_data, g_cvt.buf, g_cvt.len);
                SDL_ConvertAudio(&g_cvt);
                //g_cvt.len_cvt = resample(g_resample, (short*)g_cvt.buf, g_cvt.len/2, (short*)g_cvt.buf, g_cvt.len*2, 0)*2;
            }
            if (!g_cvt.len_cvt)
            {
                memset(stream, 0, len);
                return;
            }
            //printf("len=%d, rest=%d, g_cvt.len=%d, g_cvt.len_cvt=%d\n", len, rest, g_cvt.len, g_cvt.len_cvt);
            int to_copy = MIN(g_cvt.len_cvt, len);
            memcpy(stream, g_cvt.buf, to_copy);
            stream += to_copy;
            g_cvt.len_cvt -= to_copy;
            len -= to_copy;
            memmove(g_cvt.buf, g_cvt.buf + to_copy, g_cvt.len_cvt);
        }
    } else
        g_audio_cb(g_audio_cb_user_data, stream, len);
}

static void record_cb(void *udata, Uint8 *stream, int len)
{
    if (g_cvt_record.needed)
    {
        g_cvt_record.len = len;
        if (!g_cvt_record.buf)
            g_cvt_record.buf = malloc(g_cvt_record.len*g_cvt_record.len_mult);
        memcpy(g_cvt_record.buf, stream, len);
        SDL_ConvertAudio(&g_cvt_record);
        g_record_cb(g_record_cb_user_data, g_cvt_record.buf, g_cvt_record.len_cvt);
    } else
        g_record_cb(g_record_cb_user_data, stream, len);
}

static void sound_play_cb(void *udata, char *stream, int len)
{
    //SDL_AudioCallback cb = (SDL_AudioCallback)udata;
    LVGSound *sound = (LVGSound *)udata;
    int rest = sound->num_samples*sound->channels*2 - sound->cur_play_byte;
    char *buf = (char *)sound->samples + sound->cur_play_byte;
    int to_copy = MIN(len, rest);
    memcpy(stream, buf, to_copy);
    len -= to_copy;
    stream += to_copy;
    sound->cur_play_byte += to_copy;
    if (len)
        memset(stream, 0, len);
}

#define SDL2
int lvgStartAudio(int samplerate, int channels, int format, int buffer, int is_capture, void (*callback)(void *userdata, char *stream, int len), void *userdata)
{
#ifdef SDL2
    if (!is_capture && g_dev >= 0)
        return -1;
#else
    if (is_capture || g_dev >= 0)
        return -1;
#endif
    SDL_AudioSpec wanted, have;
    memset(&wanted, 0, sizeof(wanted));
    wanted.freq = samplerate;
    wanted.format = format ? AUDIO_F32 : AUDIO_S16;
    wanted.channels = channels;
    wanted.samples = buffer ? buffer : 4096;
    wanted.callback = is_capture ? record_cb : audio_cb;
    wanted.userdata = callback;
    if (is_capture)
    {
        g_record_cb = (SDL_AudioCallback)callback;
        g_record_cb_user_data = userdata;
    } else
    {
        g_audio_cb = (SDL_AudioCallback)callback;
        g_audio_cb_user_data = userdata;
    }
#ifdef SDL2
    int dev = SDL_OpenAudioDevice(NULL, is_capture, &wanted, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (dev < 0)
    {
        printf("error: couldn't open audio: %s\n", SDL_GetError());
        return -1;
    }
    if (is_capture)
        g_dev_record = dev;
    else
        g_dev = dev;
#else
    if (SDL_OpenAudio(&wanted, &have) < 0)
    {
        printf("error: couldn't open audio: %s\n", SDL_GetError());
        return -1;
    }
#endif
    SDL_AudioCVT *cvt = is_capture ? &g_cvt_record : &g_cvt;
    if (SDL_BuildAudioCVT(cvt, wanted.format, wanted.channels, wanted.freq, have.format, have.channels, have.freq) < 0)
    {
        printf("error: couldn't open converter: %s\n", SDL_GetError());
        return -1;
    }
    //g_resample = resample_init(have.freq, wanted.freq, 65536);
    //printf("info: rate=%d, channels=%d, format=%x, change=%d\n", have.freq, have.channels, have.format, g_cvt.needed); fflush(stdout);
    cvt->len_cvt = 0;
#ifdef SDL2
    SDL_PauseAudioDevice(dev, 0);
#else
    SDL_PauseAudio(0);
#endif
    return 0;
}

void lvgPlaySound(LVGSound *sound)
{
    sound->cur_play_byte = 0;
    lvgStartAudio(sound->rate, 1, 0, 0, 0, sound_play_cb, sound);
}
