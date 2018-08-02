#include <config.h>
#if ENABLE_AUDIO && AUDIO_SDL
#include <audio/audio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/param.h>
#include "lunzip.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
//#include "resample.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#define SDL2

typedef struct audio_channel
{
    LVGSound *sound;
    //struct rs_data *g_resample;
    SDL_AudioCVT cvt;
    int cur_play_byte, start_byte, end_byte, flags, loops;
} audio_channel;

typedef struct audio_ctx
{
    audio_channel channels[AUDIO_NUM_CHANNELS];

    SDL_AudioCVT cvt_record;
    /*SDL_AudioCallback audio_cb;
    void *audio_cb_user_data;
    SDL_AudioCallback record_cb;
    void *record_cb_user_data;*/
#ifdef SDL2
    int dev, dev_record;
#endif
    SDL_AudioSpec outputSpec;
} audio_ctx;

static void sound_drain(LVGSound *sound, audio_channel *channel, Uint8 *stream, int len)
{
    if (!channel->sound)
    {
        memset(stream, 0, len);
        return;
    }
    int rest = channel->end_byte - channel->cur_play_byte;
    char *buf = (char *)sound->samples + channel->cur_play_byte;
    int to_copy = MIN(len, rest);
    memcpy(stream, buf, to_copy);
    len -= to_copy;
    rest -= to_copy;
    stream += to_copy;
    channel->cur_play_byte += to_copy;
    if (!rest)
    {
        memset(stream, 0, len);
        if (channel->flags & PLAY_HasLoops)
        {
            if (32767 != channel->loops && channel->loops)
                channel->loops--;
            if (!channel->loops)
                goto stop;
            channel->cur_play_byte = channel->start_byte;
        } else
        {
stop:
            if (channel->cvt.buf)
                free(channel->cvt.buf);
            memset(channel, 0, sizeof(*channel));
        }
    }
}

static void audio_cb(void *udata, Uint8 *stream, int len)
{
    audio_ctx *ctx = (audio_ctx *)udata;
    memset(stream, 0, len);
    Uint8 *buf = alloca(len);
    for (int i = 0; i < AUDIO_NUM_CHANNELS; i++)
    {
        audio_channel *channel = &ctx->channels[i];
        LVGSound *sound = channel->sound;
        if (!sound)
            continue;
        SDL_AudioCVT *cvt = &ctx->channels[i].cvt;
        if (cvt->needed)
        {
            int drain_len = len;
            Uint8 *pbuf = buf;
            while (drain_len)
            {
                if (!cvt->len)
                {
                    cvt->len = ((int)(len/cvt->len_ratio) + 1) & (~1);
                    if (!cvt->buf)
                        cvt->buf = malloc(cvt->len*cvt->len_mult);
                }
                if (!cvt->len_cvt && cvt->len)
                {
                    sound_drain(sound, channel, cvt->buf, cvt->len);
                    SDL_ConvertAudio(cvt);
                    //g_cvt.len_cvt = resample(g_resample, (short*)g_cvt.buf, g_cvt.len/2, (short*)g_cvt.buf, g_cvt.len*2, 0)*2;
                }
                if (!cvt->len_cvt)
                {
                    memset(pbuf, 0, drain_len);
                    break;
                }
                //printf("len=%d, rest=%d, g_cvt.len=%d, g_cvt.len_cvt=%d\n", len, rest, g_cvt.len, g_cvt.len_cvt);
                int to_copy = MIN(cvt->len_cvt, drain_len);
                memcpy(pbuf, cvt->buf, to_copy);
                pbuf += to_copy;
                cvt->len_cvt -= to_copy;
                drain_len -= to_copy;
                memmove(cvt->buf, cvt->buf + to_copy, cvt->len_cvt);
            }
        } else
            sound_drain(sound, channel, buf, len);
        SDL_MixAudioFormat(stream, buf, ctx->outputSpec.format, len, SDL_MIX_MAXVOLUME);
    }
}

static void record_cb(void *udata, Uint8 *stream, int len)
{
    //audio_ctx *ctx = (audio_ctx *)udata;
    /*if (g_cvt_record.needed)
    {
        g_cvt_record.len = len;
        if (!g_cvt_record.buf)
            g_cvt_record.buf = malloc(g_cvt_record.len*g_cvt_record.len_mult);
        memcpy(g_cvt_record.buf, stream, len);
        SDL_ConvertAudio(&g_cvt_record);
        g_record_cb(g_record_cb_user_data, g_cvt_record.buf, g_cvt_record.len_cvt);
    } else
        g_record_cb(g_record_cb_user_data, stream, len);*/
}

static int sdl_audio_init(void **audio_render, int samplerate, int channels, int format, int buffer, int is_capture)
{
    *audio_render = 0;
    audio_ctx *ctx = calloc(1, sizeof(audio_ctx));
    SDL_AudioSpec wanted;
    memset(&wanted, 0, sizeof(wanted));
    wanted.freq = samplerate;
    wanted.format = format ? AUDIO_F32 : AUDIO_S16;
    wanted.channels = channels;
    wanted.samples = buffer ? buffer : 4096;
    wanted.callback = is_capture ? record_cb : audio_cb;
    wanted.userdata = ctx;
    /*if (is_capture)
    {
        ctx->record_cb = (SDL_AudioCallback)callback;
        ctx->record_cb_user_data = userdata;
    } else
    {
        ctx->audio_cb = (SDL_AudioCallback)callback;
        ctx->audio_cb_user_data = userdata;
    }*/
#ifdef SDL2
    int dev = SDL_OpenAudioDevice(NULL, is_capture, &wanted, &ctx->outputSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (dev <= 0)
    {
        printf("error: couldn't open audio: %s\n", SDL_GetError());
        free(ctx);
        return 0;
    }
    if (is_capture)
        ctx->dev_record = dev;
    else
        ctx->dev = dev;
#else
    if (SDL_OpenAudio(&wanted, &ctx->outputSpec) < 0)
    {
        printf("error: couldn't open audio: %s\n", SDL_GetError());
        return 0;
    }
#endif
    /*SDL_AudioCVT *cvt = is_capture ? &ctx->cvt_record : &ctx->cvt;
    if (SDL_BuildAudioCVT(cvt, wanted.format, wanted.channels, wanted.freq, have.format, have.channels, have.freq) < 0)
    {
        printf("error: couldn't open converter: %s\n", SDL_GetError());
        return -1;
    }*/
    //g_resample = resample_init(have.freq, wanted.freq, 65536);
    //printf("info: rate=%d, channels=%d, format=%x, change=%d\n", have.freq, have.channels, have.format, g_cvt.needed); fflush(stdout);
    //cvt->len_cvt = 0;
#ifdef SDL2
    SDL_PauseAudioDevice(dev, 0);
#else
    SDL_PauseAudio(0);
#endif
    *audio_render = ctx;
    return 1;
}

static void sdl_audio_stop_all(void *audio_render)
{
    audio_ctx *ctx = (audio_ctx *)audio_render;
    SDL_LockAudioDevice(ctx->dev);
    for (int i = 0; i < sizeof(ctx->channels)/sizeof(ctx->channels[0]); i++)
        if (ctx->channels[i].cvt.buf)
            free(ctx->channels[i].cvt.buf);
    memset(ctx->channels, 0, sizeof(ctx->channels));
    SDL_UnlockAudioDevice(ctx->dev);
}

static void sdl_audio_release(void *audio_render)
{
    audio_ctx *ctx = (audio_ctx *)audio_render;
    sdl_audio_stop_all(audio_render);
#ifdef SDL2
    if (ctx->dev_record)
    {
        SDL_CloseAudioDevice(ctx->dev_record);
        ctx->dev_record = 0;
    }
    if (ctx->dev)
    {
        SDL_CloseAudioDevice(ctx->dev);
        ctx->dev = 0;
    }
#else
    SDL_PauseAudio(1);
    SDL_CloseAudio();
#endif
    free(ctx);
}

static void sdl_audio_stop(audio_ctx *ctx, LVGSound *sound)
{
    int i;
    for (i = 0; i < sizeof(ctx->channels)/sizeof(ctx->channels[0]); i++)
    {
        if (ctx->channels[i].sound == sound)
        {
            if (ctx->channels[i].cvt.buf)
                free(ctx->channels[i].cvt.buf);
            memset(&ctx->channels[i], 0, sizeof(ctx->channels[i]));
        }
    }
}

static void sdl_audio_play(void *audio_render, LVGSound *sound, int flags, int start_sample, int end_sample, int loops)
{
    audio_ctx *ctx = (audio_ctx *)audio_render;
    if (!sound->num_samples)
        return;
    SDL_LockAudioDevice(ctx->dev);
    int i;
    if (flags & PLAY_SyncStop)
    {
        sdl_audio_stop(ctx, sound);
        goto done;
    }
    if (flags & PLAY_SyncNoMultiple)
    {
        for (i = 0; i < sizeof(ctx->channels)/sizeof(ctx->channels[0]); i++)
        {
            if (ctx->channels[i].sound == sound)
                goto done;
        }
    }
    for (i = 0; i < sizeof(ctx->channels)/sizeof(ctx->channels[0]); i++)
    {
        if (!ctx->channels[i].sound)
            break;
    }
    if (AUDIO_NUM_CHANNELS == i)
        return;
    SDL_AudioCVT *cvt = &ctx->channels[i].cvt;
    if (SDL_BuildAudioCVT(cvt, AUDIO_S16, sound->channels, sound->rate, ctx->outputSpec.format, ctx->outputSpec.channels, ctx->outputSpec.freq) < 0)
    {
        printf("error: couldn't open converter: %s\n", SDL_GetError());
        goto done;
    }
    int bytes = sound->num_samples*sound->channels*2;
    ctx->channels[i].start_byte = (uint64_t)start_sample*sound->rate/sound->orig_rate*sound->channels*2;
    ctx->channels[i].end_byte   = (uint64_t)end_sample*sound->rate/sound->orig_rate*sound->channels*2;
    if (ctx->channels[i].start_byte > bytes)
        ctx->channels[i].start_byte = bytes;
    if (ctx->channels[i].end_byte > bytes)
        ctx->channels[i].end_byte = bytes;
    if (ctx->channels[i].start_byte == ctx->channels[i].end_byte)
        goto done;
    ctx->channels[i].cur_play_byte = ctx->channels[i].start_byte;
    ctx->channels[i].flags = flags;
    ctx->channels[i].loops = loops;
    ctx->channels[i].sound = sound;
done:
    SDL_UnlockAudioDevice(ctx->dev);
}

static void sdl_resample(void *audio_render, LVGSound *sound)
{
    audio_ctx *ctx = (audio_ctx *)audio_render;
    if (!sound->num_samples)
        return;
    SDL_AudioCVT cvt;
    if (SDL_BuildAudioCVT(&cvt, AUDIO_S16, sound->channels, sound->rate, AUDIO_S16, sound->channels, ctx->outputSpec.freq) < 0)
    {
        printf("error: couldn't open converter: %s\n", SDL_GetError());
        return;
    }
    cvt.len = sound->num_samples*sound->channels*2;
    cvt.buf = (Uint8 *)malloc(cvt.len*cvt.len_mult);
    memcpy(cvt.buf, sound->samples, cvt.len);
    SDL_ConvertAudio(&cvt);
    free(sound->samples);
    sound->samples = (short *)cvt.buf;
    sound->num_samples = cvt.len_cvt/(sound->channels*2);
    sound->orig_rate = sound->rate;
    sound->rate = ctx->outputSpec.freq;
    //printf("converted: %d-%d\n", sound->orig_rate, sound->rate);
}

const audio_render sdl_audio_render =
{
    sdl_audio_init,
    sdl_audio_release,
    sdl_audio_play,
    sdl_audio_stop_all,
    sdl_resample
};
#endif
