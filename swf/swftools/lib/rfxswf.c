/* vi: set sts=2 sw=2 :*/
/* rfxswf.c

   Library for creating and reading SWF files or parts of it.
   There's a module directory which provides some extended functionality.
   Most modules are included at the bottom of this file.

   Part of the swftools package.

   Copyright (c) 2000-2003 Rainer Bohme <rfxswf@reflex-studio.de>
   Copyright (c) 2003 Matthias Kramm <kramm@quiss.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include "rfxswf.h"
#include <assert.h>

#include "./bitio.h"

// internal constants

#define MALLOC_SIZE     128

#define MEMSIZE(l) (((l/MALLOC_SIZE) + 1)*MALLOC_SIZE)

// inline wrapper functions

TAG *swf_NextTag(TAG *t)   { return t->next; }
TAG *swf_PrevTag(TAG *t)   { return t->prev; }
uint16_t  swf_GetTagID(TAG *t)  { return t->id; }
uint32_t  swf_GetTagLen(TAG *t) { return t->len; }
uint8_t  *swf_GetTagLenPtr(TAG *t) { return &(t->data[t->len]); }
uint32_t  swf_GetTagPos(TAG *t) { return t->pos; }

void swf_SetTagPos(TAG *t, uint32_t pos)
{
    swf_ResetReadBits(t);
    if (pos <= t->len)
        t->pos = pos;
#ifdef _DEBUG
    else
    {
        printf("SetTagPos(%d) out of bounds: TagID = %i\n",pos, t->id); fflush(stdout);
    }
#endif
}

char *swf_GetString(TAG *t)
{
    int pos = t->pos;
    while (t->pos < t->len && swf_GetU8(t));
    /* make sure we always have a trailing zero byte */
    if (t->pos == t->len)
    {
        if (t->len == t->memsize)
        {
            assert(0);
        }
        t->data[t->len] = 0;
    }
    return (char*)&(t->data[pos]);
}

uint8_t swf_GetU8(TAG *t)
{
    swf_ResetReadBits(t);
    if ((int)t->pos >= (int)t->len)
    {
#ifdef _DEBUG
        printf("GetU8() out of bounds: TagID = %i\n", t->id); fflush(stdout);
#endif
        return 0;
    }
    return t->data[t->pos++];
}

uint16_t swf_GetU16(TAG *t)
{
    uint16_t res;
    swf_ResetReadBits(t);
    if ((int)t->pos > ((int)t->len - 2))
    {
#ifdef _DEBUG
        printf("GetU16() out of bounds: TagID = %i\n", t->id); fflush(stdout);
#endif
        return 0;
    }
    res = t->data[t->pos] | (t->data[t->pos+1]<<8);
    t->pos+=2;
    return res;
}

uint32_t swf_GetU32(TAG *t)
{
    uint32_t res;
    swf_ResetReadBits(t);
    if ((int)t->pos > ((int)t->len - 4))
    {
#ifdef _DEBUG
        printf("GetU32() out of bounds: TagID = %i\n", t->id); fflush(stdout);
#endif
        return 0;
    }
    res = t->data[t->pos] | (t->data[t->pos + 1] << 8) |
        (t->data[t->pos + 2] << 16) | (t->data[t->pos + 3] << 24);
    t->pos += 4;
    return res;
}

int swf_GetBlock(TAG *t, uint8_t *b, int l)
// returns number of bytes written (<=l)
// b = NULL -> skip data
{
    swf_ResetReadBits(t);
    if ((t->len - t->pos) < l)
        l = t->len - t->pos;
    if (b && l)
        memcpy(b, &t->data[t->pos], l);
    t->pos += l;
    return l;
}

int swf_SetBlock(TAG *t, const uint8_t *b, int l)
// Appends Block to the end of Tagdata, returns size
{
    uint32_t newlen = t->len + l;
    swf_ResetWriteBits(t);
    if (newlen > t->memsize)
    {
        uint32_t newmem  = MEMSIZE(newlen);
        uint8_t *newdata = (uint8_t*)(realloc(t->data, newmem));
        t->memsize = newmem;
        t->data    = newdata;
    }
    if (b)
        memcpy(&t->data[t->len], b, l);
    else
        memset(&t->data[t->len], 0x00, l);
    t->len+=l;
    return l;
}

uint32_t swf_GetBits(TAG *t, int nbits)
{
    uint32_t res = 0;
    if (!nbits)
        return 0;
    if (!t->readBit)
        t->readBit = 0x80;
    while (nbits)
    {
        res <<= 1;
        if (t->pos >= t->len)
        {
#ifdef _DEBUG
            printf("GetBits() out of bounds: TagID = %i, pos=%d, len=%d\n", t->id, t->pos, t->len);
            int i, m = t->len > 10 ? 10 : t->len;
            for (i = -1; i < m; i++)
            {
                printf("(%d)%02x ", i, t->data[i]);
            }
            printf("\n"); fflush(stdout);
#endif
            return res;
        }
        if (t->data[t->pos] & t->readBit)
            res |= 1;
        t->readBit >>= 1;
        nbits--;
        if (!t->readBit)
        {
            if (nbits)
                t->readBit = 0x80;
            t->pos++;
        }
    }
    return res;
}

int32_t swf_GetSBits(TAG * t, int nbits)
{
    uint32_t res = swf_GetBits(t, nbits);
    if (res & (1 << (nbits - 1)))
        res |= (0xffffffff << nbits);
    return (int32_t)res;
}

uint32_t reader_GetBits(reader_t *reader, int nbits)
{
    return reader_readbits(reader, nbits);
}

int32_t reader_GetSBits(reader_t *reader, int nbits)
{
    uint32_t res = reader_readbits(reader, nbits);
    if (res & (1 << (nbits - 1)))
        res |= (0xffffffff << nbits);
    return (int32_t)res;
}

// Advanced Data Access Functions

double swf_GetFixed(TAG *t)
{
    uint16_t low =  swf_GetU16(t);
    uint16_t high = swf_GetU16(t);
    return high + low*(1/65536.0);
}

float swf_GetFixed8(TAG *t)
{
    uint8_t low =  swf_GetU8(t);
    uint8_t high = swf_GetU8(t);
    return (float)(high + low*(1/256.0));
}

uint32_t swf_GetU30(TAG *tag)
{
    uint32_t shift = 0;
    uint32_t s = 0;
    int nr=0;
    while(1)
    {
        uint8_t b = swf_GetU8(tag);
        nr++;
        s |= (b & 127) << shift;
        shift += 7;
        if (!(b & 128) || shift >= 32)
            break;
    }
    return s;
}

uint32_t swf_GetABCU32(TAG*tag)
{
    return swf_GetU30(tag);
}

int32_t swf_GetABCS32(TAG*tag)
{
    return swf_GetABCU32(tag);
}

#if 0

/*The AVM2 spec is just plain wrong, claiming that int32_t values are sign
extended. They're not.
This wastes up to 4 bytes for every negative value. */

void swf_SetABCS32(TAG*tag, int32_t s)
{
  printf("write int32_t: %d\n", s);
    int32_t neg = s<0?-1:0;
    uint8_t sign = s<0?0x40:0;
    while(1) {
        uint8_t val = s&0x7f;
        uint8_t vsign = s&0x40;
        s>>=7;
        neg>>=7;
        if(s==neg && vsign==sign) {
            /* if the value we now write has the same sign as s
               and all the remaining bits are equal to the sign of s
               too, stop writing */
            swf_SetU8(tag, val);
            printf("put %02x\n", val);
            break;
        } else {
            swf_SetU8(tag, 0x80 | val);
            printf("put %02x\n", 0x80|val);
        }
    };
}
int swf_GetS30(TAG*tag)
{
    uint32_t shift = 0;
    uint32_t s = 0;
    int nr=0;
    while(1) {
        uint8_t b = swf_GetU8(tag);
        nr++;
        nt i,m=t->len>10?10:t->len;
        for(i=0;i<m;i++) {
            printf("%02x ", t->data[i]);
        }
        printf("\n");
        s|=(b&127)<<shift;
        shift+=7;
        if(!(b&128) || shift>=32)
        {
            if(b&64) {
                if(shift<32)
                  s|=0xffffffff<<shift;
            }
            break;
        }
    }
    /* It's not uncommon for other applications (Flex for all negative numbers, and
       Flash for -1) to generate a lot more bytes than would be necessary.
       int nr2= swf_SetS30(0, s);
    if(nr!=nr2) {
      printf("Signed value %d stored in %d bytes, I'd store it in %d bytes\n", s, nr, nr2);
    }*/
    return s;
}
#endif

float swf_GetF16(TAG * t)
{
    uint16_t f1 = swf_GetU16(t);
    if (!(f1 & 0x3ff))
        return 0.0;

    // IEEE 16 is 1-5-10
    // IEEE 32 is 1-8-23
    /* gcc 4.1.2 seems to require a union here. *(float*)u doesn't work */
    union {
        uint32_t u;
        float f;
    } f2;

    uint16_t e = (f1 >> 10) & 0x1f;
    uint16_t m = f1 & 0x3ff;
    /* find highest bit in mantissa */
    int h=0;
    while (!(m & 0x400))
    {
        m <<= 1;
        h++;
    }
    m &= 0x3ff;
    e -= h;
    e += 0x6f;

    f2.u = (f1 & 0x8000) << 16; //sign
    f2.u |= e << 23; //exponent
    f2.u |= m << 13; //mantissa
    return *(float*)&f2;
}

float F16toFloat(uint16_t x)
{
    TAG t;
    t.data = (void*)&x;
    t.readBit = 0;
    t.pos = 0;
    t.len = 2;
    return swf_GetF16(&t);
}

float swf_GetFloat(TAG *tag)
{
    union {
        uint32_t uint_bits;
        float float_bits;
    } f;
    f.uint_bits = swf_GetU32(tag);
    return f.float_bits;
}

double swf_GetD64(TAG *tag)
{
    /* FIXME: this is not big-endian compatible */
    double value = *(double*)&tag->data[tag->pos];
    swf_GetU32(tag);
    swf_GetU32(tag);
    return value;
}

int swf_GetU24(TAG *tag)
{
    int b1 = swf_GetU8(tag);
    int b2 = swf_GetU8(tag);
    int b3 = swf_GetU8(tag);
    return b3 << 16 | b2 << 8 | b1;
}

int swf_GetS24(TAG *tag)
{
    int b1 = swf_GetU8(tag);
    int b2 = swf_GetU8(tag);
    int b3 = swf_GetU8(tag);
    if (b3 & 0x80)
        return -1 - ((b3 << 16 | b2 << 8 | b1) ^ 0xffffff);
    else
        return b3 << 16 | b2 << 8 | b1;
}

void swf_GetRGB(TAG *t, RGBA * col)
{
    RGBA dummy;
    if(!col)
        col = &dummy;
    col->r = swf_GetU8(t);
    col->g = swf_GetU8(t);
    col->b = swf_GetU8(t);
    col->a = 255;
}

void swf_GetRGBA(TAG *t, RGBA * col)
{
    RGBA dummy;
    if (!col)
        col = &dummy;
    col->r = swf_GetU8(t);
    col->g = swf_GetU8(t);
    col->b = swf_GetU8(t);
    col->a = swf_GetU8(t);
}

void swf_GetGradient(TAG *tag, GRADIENT *gradient, char alpha)
{
    int t;
    if (!tag)
    {
        memset(gradient, 0, sizeof(GRADIENT));
        return;
    }
    uint8_t num = swf_GetU8(tag) & 15;
    if (gradient)
    {
        gradient->num = num;
        gradient->rgba = (RGBA*)calloc(1, sizeof(RGBA)*gradient->num);
        gradient->ratios = (uint8_t*)calloc(1, sizeof(gradient->ratios[0])*gradient->num);
    }
    for (t = 0; t < num; t++)
    {
        uint8_t ratio = swf_GetU8(tag);
        RGBA color;
        if (!alpha)
            swf_GetRGB(tag, &color);
        else
            swf_GetRGBA(tag, &color);
        if (gradient)
        {
            gradient->ratios[t] = ratio;
            gradient->rgba[t]   = color;
        }
    }
}

void swf_FreeGradient(GRADIENT *gradient)
{
    if (gradient->ratios)
        free(gradient->ratios);
    if (gradient->rgba)
        free(gradient->rgba);
    memset(gradient, 0, sizeof(GRADIENT));
}

int swf_GetRect(TAG *t, SRECT *r)
{
    int nbits;
    SRECT dummy;
    if (!t)
    {
        r->xmin = r->xmax = r->ymin = r->ymax = 0;
        return 0;
    }
    if (!r)
        r = &dummy;
    nbits = (int) swf_GetBits(t, 5);
    r->xmin = swf_GetSBits(t, nbits);
    r->xmax = swf_GetSBits(t, nbits);
    r->ymin = swf_GetSBits(t, nbits);
    r->ymax = swf_GetSBits(t, nbits);
    return 0;
}

int reader_GetRect(reader_t *reader, SRECT *r)
{
    int nbits;
    SRECT dummy;
    if (!r)
        r = &dummy;
    nbits = (int) reader_GetBits(reader, 5);
    r->xmin = reader_GetSBits(reader, nbits);
    r->xmax = reader_GetSBits(reader, nbits);
    r->ymin = reader_GetSBits(reader, nbits);
    r->ymax = reader_GetSBits(reader, nbits);
    return 0;
}

SRECT swf_ClipRect(SRECT border, SRECT r)
{
    if (r.xmax > border.xmax)
        r.xmax = border.xmax;
    if (r.ymax > border.ymax)
        r.ymax = border.ymax;
    if (r.xmax < border.xmin)
        r.xmax = border.xmin;
    if (r.ymax < border.ymin)
        r.ymax = border.ymin;

    if (r.xmin > border.xmax)
        r.xmin = border.xmax;
    if (r.ymin > border.ymax)
        r.ymin = border.ymax;
    if (r.xmin < border.xmin)
        r.xmin = border.xmin;
    if (r.ymin < border.ymin)
        r.ymin = border.ymin;
    return r;
}

void swf_ExpandRect(SRECT *src, SPOINT add)
{
    if ((src->xmin | src->ymin | src->xmax | src->ymax) == 0)
    {
        src->xmin = add.x;
        src->ymin = add.y;
        src->xmax = add.x;
        src->ymax = add.y;
        if ((add.x | add.y) == 0)
            src->xmax++; //make sure the bbox is not NULL anymore
        return;
    }
    if (add.x < src->xmin)
        src->xmin = add.x;
    if (add.x > src->xmax)
        src->xmax = add.x;
    if (add.y < src->ymin)
        src->ymin = add.y;
    if (add.y > src->ymax)
        src->ymax = add.y;
}

void swf_ExpandRect2(SRECT *src, SRECT *add)
{
    if ((add->xmin | add->ymin | add->xmax | add->ymax) == 0)
        return;
    if ((src->xmin | src->ymin | src->xmax | src->ymax) == 0)
        *src = *add;
    if (add->xmin < src->xmin)
        src->xmin = add->xmin;
    if (add->ymin < src->ymin)
        src->ymin = add->ymin;
    if (add->xmax > src->xmax)
        src->xmax = add->xmax;
    if (add->ymax > src->ymax)
        src->ymax = add->ymax;
}

void swf_ExpandRect3(SRECT *src, SPOINT center, int radius)
{
    if ((src->xmin | src->ymin | src->xmax | src->ymax) == 0)
    {
        src->xmin = center.x-radius;
        src->ymin = center.y-radius;
        src->xmax = center.x+radius;
        src->ymax = center.y+radius;
        if ((center.x | center.y | radius) == 0)
            src->xmax++; //make sure the bbox is not NULL anymore
        return;
    }
    if (center.x - radius < src->xmin)
        src->xmin = center.x - radius;
    if (center.x + radius > src->xmax)
        src->xmax = center.x + radius;
    if (center.y - radius < src->ymin)
        src->ymin = center.y - radius;
    if (center.y + radius > src->ymax)
        src->ymax = center.y + radius;
}

int swf_GetMatrix(TAG *t, MATRIX *m)
{
    MATRIX dummy;
    int nbits;

    if (!m)
        m = &dummy;

    if (!t)
    {
        m->sx = m->sy = 0x10000;
        m->r0 = m->r1 = 0;
        m->tx = m->ty = 0;
        return -1;
    }

    swf_ResetReadBits(t);

    if (swf_GetBits(t, 1))
    {
        nbits = swf_GetBits(t, 5);
        m->sx = swf_GetSBits(t, nbits);
        m->sy = swf_GetSBits(t, nbits);
    } else
        m->sx = m->sy = 0x10000;

    if (swf_GetBits(t, 1))
    {
        nbits = swf_GetBits(t, 5);
        m->r0 = swf_GetSBits(t, nbits);
        m->r1 = swf_GetSBits(t, nbits);
    } else
        m->r0 = m->r1 = 0x0;

    nbits = swf_GetBits(t, 5);
    m->tx = swf_GetSBits(t, nbits);
    m->ty = swf_GetSBits(t, nbits);
    return 0;
}

int swf_GetCXForm(TAG *t, CXFORM *cx,uint8_t alpha)
{
    CXFORM cxf;
    int hasadd, hasmul, nbits;

    if (!cx)
        cx = &cxf;

    cx->a0 = cx->r0 = cx->g0 = cx->b0 = 256;
    cx->a1 = cx->r1 = cx->g1 = cx->b1 = 0;

    if (!t)
        return 0;

    swf_ResetReadBits(t);
    hasadd = swf_GetBits(t, 1);
    hasmul = swf_GetBits(t, 1);
    nbits  = swf_GetBits(t, 4);

    if (hasmul)
    {
        cx->r0 = (int16_t)swf_GetSBits(t, nbits);
        cx->g0 = (int16_t)swf_GetSBits(t, nbits);
        cx->b0 = (int16_t)swf_GetSBits(t, nbits);
        if (alpha)
            cx->a0 = (int16_t)swf_GetSBits(t, nbits);
    }
    if (hasadd)
    {
        cx->r1 = (int16_t)swf_GetSBits(t, nbits);
        cx->g1 = (int16_t)swf_GetSBits(t, nbits);
        cx->b1 = (int16_t)swf_GetSBits(t, nbits);
        if (alpha)
            cx->a1 = (int16_t)swf_GetSBits(t, nbits);
    }
    return 0;
}

// Tag List Manipulating Functions

void swf_ResetTag(TAG *tag, uint16_t id)
{
    tag->len = tag->pos = tag->readBit = tag->writeBit = 0;
    tag->id = id;
}

TAG *swf_DeleteTag(SWF *swf, TAG *t)
{
    TAG *next = t->next;

    if (swf && swf->firstTag==t)
        swf->firstTag = t->next;
    if (t->prev)
        t->prev->next = t->next;
    if (t->next)
        t->next->prev = t->prev;

    if (t->data)
        free(t->data);
    free(t);
    return next;
}

TAG *swf_ReadTag(reader_t *reader, TAG *prev)
{
    TAG * t;
    uint16_t raw;
    uint32_t len;
    int id;

    if (reader->read(reader, &raw, 2) != 2)
        return NULL;
    raw = LE_16_TO_NATIVE(raw);

    len = raw & 0x3f;
    id  = raw >> 6;

    if (len == 0x3f)
    {
        len = reader_readU32(reader);
    }

    if (id == ST_DEFINESPRITE)
        len = 2*sizeof(uint16_t); // Sprite handling fix: Flatten sprite tree

    t = (TAG *)calloc(1, sizeof(TAG));

    t->len = len;
    t->id  = id;

    if (t->len)
    {
        t->data = (uint8_t*)malloc(t->len + 4); // pad for mp3 bitbuf
        t->memsize = t->len;
        if (reader->read(reader, t->data, t->len) != t->len)
        {
#ifdef _DEBUG
            printf("rfxswf: Warning: Short read (tagid %d). File truncated?\n", t->id); fflush(stdout);
#endif
            free(t->data);
            free(t);
            return NULL;
        }
    }

    if (prev)
    {
        t->prev  = prev;
        prev->next = t;
    }
    return t;
}

int swf_ReadSWF2(reader_t *reader, SWF *swf)   // Reads SWF to memory (malloc'ed), returns length or <0 if fails
{
    if (!swf)
        return -1;
    memset(swf, 0x00, sizeof(SWF));

    char b[32];                         // read Header
    int len;
    TAG * t;
    TAG t1;
    if ((len = reader->read(reader, b, 8)) < 8)
        return -1;

    if (b[0]!='F' && b[0]!='C')
        return -1;
    if (b[1]!='W')
        return -1;
    if (b[2]!='S')
        return -1;
    swf->fileVersion = b[3];
    swf->compressed  = (b[0]=='C')?1:0;
    swf->fileSize    = GET32(&b[4]);

    if (swf->compressed)
    {
        return -1;
    }
    swf->compressed = 0; // derive from version number from now on

    reader_GetRect(reader, &swf->movieSize);
    reader->read(reader, &swf->frameRate, 2);
    swf->frameRate = LE_16_TO_NATIVE(swf->frameRate);
    reader->read(reader, &swf->frameCount, 2);
    swf->frameCount = LE_16_TO_NATIVE(swf->frameCount);

    /* read tags and connect to list */
    t1.next = 0;
    t = &t1;
    while (t)
    {
        t = swf_ReadTag(reader,t);
        if (t && t->id == ST_FILEATTRIBUTES)
        {
            swf->fileAttributes = swf_GetU32(t);
            swf_ResetReadBits(t);
        }
    }
    swf->firstTag = t1.next;
    if (t1.next)
        t1.next->prev = NULL;

    return reader->pos;
}

void swf_FreeTags(SWF *swf)                 // Frees all malloc'ed memory for tags
{
    TAG * t = swf->firstTag;
    while (t)
    {
        TAG *tnew = t->next;
        if (t->data)
            free(t->data);
        free(t);
        t = tnew;
    }
    swf->firstTag = 0;
}
