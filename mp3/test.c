#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include "minimp3.h"

void dec(FILE *f, FILE *o)
{
    mp3_info_t info;
    mp3_decoder_t dec = mp3_create();
#define BUFS 4096
    char buf[BUFS];
    int nbuf = 0;
    while (!feof(f))
    {
        int i, frame_size, samples;
        short frame_buf[MP3_MAX_SAMPLES_PER_FRAME];
        nbuf += fread(buf + nbuf, 1, BUFS - nbuf, f);

        frame_size = mp3_decode(dec, buf, nbuf, frame_buf, &info);
        printf("frame_size %d\n", frame_size);
        if (!frame_size)
            break;
        samples = info.audio_bytes/(info.channels*2);
        fwrite(frame_buf, samples * info.channels*2, 1, o);
        memmove(buf, buf + frame_size, nbuf -= frame_size);
    }
    mp3_done(dec);
    fclose(f);
    fclose(o);
}

int main(int argc, char* argv[])
{
    FILE *filein;
    char *input_file_name = (argc > 1) ? argv[1] : NULL;
    if (!input_file_name)
    {
        printf("ERROR: no file name given!\n");
        return 1;
    }
    filein = fopen(input_file_name, "rb");
    if (!filein)
    {
        printf("ERROR: can't open file %s!\n", input_file_name);
        return 1;
    }
    dec(filein, fopen("out.pcm","wb"));
    return 0;
}
