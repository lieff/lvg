#include <config.h>
#if ENABLE_VIDEO && VIDEO_FFMPEG
#include "../video.h"
#include <libavcodec/avcodec.h>
#include <stdio.h>
#include <assert.h>

static int g_ffmpeg_initialized;

typedef struct ffmpeg_decoder
{
    AVCodecContext *dec_ctx;
    AVFrame *frame;
    AVPacket *pkt;
} ffmpeg_decoder;

static int getCodecType(int type)
{
    switch(type)
    {
    case VIDEO_CODEC_H263:
        return AV_CODEC_ID_FLV1;
    case VIDEO_CODEC_VP6:
        return AV_CODEC_ID_VP6F;
    case VIDEO_CODEC_VP6A:
        return AV_CODEC_ID_VP6A;
    case VIDEO_CODEC_SCREENVIDEO:
        return AV_CODEC_ID_FLASHSV;
    case VIDEO_CODEC_SCREENVIDEO2:
        return AV_CODEC_ID_FLASHSV2;
    case VIDEO_CODEC_H264:
        return AV_CODEC_ID_H264;
    }
    return 0;
}

static void ff_init(void **_dec, int type)
{
    *_dec = 0;
    if (!g_ffmpeg_initialized)
    {
        g_ffmpeg_initialized = 1;
        avcodec_register_all();
    }

    AVCodec *codec = avcodec_find_decoder(getCodecType(type));
    if (!codec)
        return;
    ffmpeg_decoder *dec = (ffmpeg_decoder*)malloc(sizeof(ffmpeg_decoder));
    dec->dec_ctx = avcodec_alloc_context3(codec);
    dec->frame = av_frame_alloc();
#ifdef OLD_API
    dec->pkt = malloc(sizeof(AVPacket));
    av_init_packet(dec->pkt);
#else
    dec->pkt = av_packet_alloc();
#endif
    if (!dec->dec_ctx || !dec->frame || !dec->pkt)
        return;
    if (avcodec_open2(dec->dec_ctx, codec, NULL) < 0)
    {
        printf("Could not open codec\n");
        return;
    }
    assert(AV_PIX_FMT_YUV420P == dec->dec_ctx->pix_fmt);
    *_dec = dec;
}

static void ff_release(void *_dec)
{
    ffmpeg_decoder *dec = _dec;
    if (dec->dec_ctx)
        avcodec_free_context(&dec->dec_ctx);
    if (dec->frame)
        av_frame_free(&dec->frame);
    if (dec->pkt)
#ifdef OLD_API
    {
        av_packet_unref(dec->pkt);
        free(dec->pkt);
    }
#else
        av_packet_free(&dec->pkt);
#endif
    free(dec);
}

static int ff_decode(void *_dec, void *buf, int len, video_frame *out)
{
    ffmpeg_decoder *dec = _dec;
    int ret;
    out->planes[0] = NULL;
    if (!dec || !buf || !len)
        return 0;
#ifndef OLD_API
    ret = avcodec_receive_frame(dec->dec_ctx, dec->frame);
    if (!ret)
        goto decoded;
#endif
    dec->pkt->data = buf;
    dec->pkt->size = len;
#ifndef OLD_API
    ret = avcodec_send_packet(dec->dec_ctx, dec->pkt);
    if (ret < 0) {
        printf("Error sending a packet for decoding\n");
        return 0;
    }
    ret = avcodec_receive_frame(dec->dec_ctx, dec->frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return 0;
    else if (ret < 0)
    {
        printf("Error during decoding\n");
        return 0;
    }
#else
    int got_frame = 0;
    ret = avcodec_decode_video2(dec->dec_ctx, dec->frame, &got_frame, dec->pkt);
    if (!got_frame)
        return 0;
#endif
decoded:
    for(int i = 0; i < 3; i++)
    {
        out->planes[i] = dec->frame->data[i];
        out->stride[i] = dec->frame->linesize[i];
    }
    out->width  = dec->frame->width;
    out->height = dec->frame->height;
    return 1;
}

const video_dec ff_decoder =
{
    ff_init,
    ff_release,
    ff_decode
};
#endif
