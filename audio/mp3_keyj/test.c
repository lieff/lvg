#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include "minimp3.h"

void dec(FILE *f, FILE *r, FILE *o)
{
    mp3_info_t info;
    mp3_decoder_t dec = mp3_create();
#define BUFS 4096
    char buf[BUFS];
    int nbuf = 0, total_samples = 0, maxdiff = 0;
    double MSE = 0.0, psnr;
    do
    {
        int i, frame_size, samples;
        short frame_buf[MP3_MAX_SAMPLES_PER_FRAME];
        nbuf += fread(buf + nbuf, 1, BUFS - nbuf, f);

        frame_size = mp3_decode(dec, buf, nbuf, frame_buf, &info);
        //printf("frame_size %d\n", frame_size);
        if (!frame_size)
            break;
        samples = info.audio_bytes/(info.channels*2);
        short ref[MP3_MAX_SAMPLES_PER_FRAME];
        fread(ref, 1, info.audio_bytes, r);
        total_samples += samples*info.channels;
        for (i = 0; i < samples*info.channels; i++)
        {
            int MSEtemp = abs((int)frame_buf[i] - (int)ref[i]);
            if (MSEtemp > maxdiff)
                maxdiff = MSEtemp;
            MSE += MSEtemp*MSEtemp;
        }
        if (o)
            fwrite(frame_buf, samples * info.channels*2, 1, o);
        memmove(buf, buf + frame_size, nbuf -= frame_size);
    } while(nbuf);
    mp3_done(dec);

    MSE /= total_samples;
    if (0 == MSE)
        psnr = 99.0;
    else
        psnr = 10.0*log10(((double)0x7fff*0x7fff)/MSE);
    printf("rate=%d samples=%d max_diff=%d PSNR=%f\n", info.sample_rate, total_samples, maxdiff, psnr);
    if (psnr < 96)
    {
        printf("PSNR compliance failed\n");
        //exit(1);
    }

    fclose(f);
    fclose(r);
    if (o)
        fclose(o);
}

int main(int argc, char* argv[])
{
    char *input_file_name = (argc > 1) ? argv[1] : NULL;
    char *ref_file_name    = (argc > 2) ? argv[2] : NULL;
    char *output_file_name = (argc > 3) ? argv[3] : NULL;
    if (!input_file_name || !ref_file_name)
    {
        printf("error: no file names given\n");
        return 1;
    }
    dec(fopen(input_file_name, "rb"), fopen(ref_file_name, "rb"), output_file_name ? fopen(output_file_name, "wb") : NULL);
    return 0;
}
