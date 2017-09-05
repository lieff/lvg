#include <stdint.h>
#include <rfxswf.h>

static const char index_tables[4][16] = {
    { -1, 2 },
    { -1, -1, 2, 4 },
    { -1, -1, -1, -1, 2, 4, 6, 8 },
    { -1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 13, 16 }
};

static const int16_t adpcm_step_table[89] = {
        7,     8,     9,    10,    11,    12,    13,    14,    16,    17,
       19,    21,    23,    25,    28,    31,    34,    37,    41,    45,
       50,    55,    60,    66,    73,    80,    88,    97,   107,   118,
      130,   143,   157,   173,   190,   209,   230,   253,   279,   307,
      337,   371,   408,   449,   494,   544,   598,   658,   724,   796,
      876,   963,  1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,
     2272,  2499,  2749,  3024,  3327,  3660,  4026,  4428,  4871,  5358,
     5894,  6484,  7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

typedef struct ADPCMChannel
{
    int predictor;
    int16_t step_index;
} ADPCMChannel;

static inline int clip(int val, int vmin, int vmax)
{
    if (val < vmin)
        return vmin;
    else if (val > vmax)
        return vmax;
    else
        return val;
}

static inline int16_t clip16(int val)
{
    if ((val + 0x8000U) & ~0xFFFF)
        return (val >> 31) ^ 0x7FFF;
    else
        return val;
}

static inline int getBitPos(TAG *tag, int start_pos)
{
    int pos = ((int)tag->pos - start_pos)*8;
    int bits = __builtin_clz(tag->readBit) - 24;
    if (!tag->readBit)
        bits = 0;
    return pos + bits;
}

int adpcm_decode(TAG *tag, int buf_size, int channels, int16_t *samples, int max_samples)
{
    int i, count, size = buf_size*8, samples_num = 0, start_pos = tag->pos;
    int nbits = swf_GetBits(tag, 2) + 2;
    const char *table = index_tables[nbits - 2];
    int k0 = 1 << (nbits - 2);
    int signmask = 1 << (nbits - 1);

    ADPCMChannel status[2];
    while (getBitPos(tag, start_pos) <= (size - 22*channels))
    {
        for (i = 0; i < channels; i++)
        {
            if (samples_num < max_samples)
            {
                *samples++ = status[i].predictor = (int16_t)(uint16_t)swf_GetBits(tag, 16);
                samples_num++;
            }
            status[i].step_index = swf_GetBits(tag, 6);
        }
        for (count = 0; getBitPos(tag, start_pos) <= (size - nbits*channels) && (count < 4095); count++)
        {
            for (i = 0; i < channels; i++)
            {
                int delta = swf_GetBits(tag, nbits);
                int step = adpcm_step_table[status[i].step_index];
                long vpdiff = 0;
                int k = k0;

                do {
                    if (delta & k)
                        vpdiff += step;
                    step >>= 1;
                    k >>= 1;
                } while(k);
                vpdiff += step;

                if (delta & signmask)
                    status[i].predictor -= vpdiff;
                else
                    status[i].predictor += vpdiff;

                status[i].step_index += table[delta & (~signmask)];
                status[i].step_index = clip(status[i].step_index, 0, 88);
                status[i].predictor = clip16(status[i].predictor);

                if (samples_num < max_samples)
                {
                    *samples++ = status[i].predictor;
                    samples_num++;
                }
            }
        }
    }
    return samples_num;
}

