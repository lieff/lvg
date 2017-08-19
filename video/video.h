#pragma once

enum { VIDEO_CODEC_H263, VIDEO_CODEC_SCREENVIDEO, VIDEO_CODEC_VP6, VIDEO_CODEC_VP6A, VIDEO_CODEC_SCREENVIDEO2, VIDEO_CODEC_H264 };

typedef struct video_dec
{
    void (*init)(void **dec, int type);
    void (*release)(void *dec);
    int (*decode)(void *dec, void *buf, int len);
} video_dec;
