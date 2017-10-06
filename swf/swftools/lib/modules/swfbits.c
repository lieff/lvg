/* swfbits.c

   Bitmap functions (needs libjpeg)

   Extension module for the rfxswf library.
   Part of the swftools package.

   Copyright (c) 2000, 2001 Rainer Bˆhme <rfxswf@reflex-studio.de>

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

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include "../../config.h"
#ifdef HAVE_ZLIB
#include <zconf.h>
#include <zlib.h>
#else
#include "stb_image.h"
#endif

#ifdef HAVE_JPEGLIB
//#include "stb_image.h"
//#define HAVE_BOOLEAN
#ifdef __cplusplus
extern "C" {
#endif
#include <jpeglib.h>
#ifdef __cplusplus
}
#endif
#endif // HAVE_JPEGLIB

#include "../rfxswf.h"

#define OUTBUFFER_SIZE 0x8000

int swf_ImageHasAlpha(RGBA *img, int width, int height)
{
    int t, len = width*height, hasalpha = 0;
    for (t = 0; t < len; t++)
    {
        if (img[t].a >= 4 && img[t].a < 0xfc)
            return 2;
        if (img[t].a < 4)
            hasalpha=1;
    }
    return hasalpha;
}

int swf_ImageGetNumberOfPaletteEntries(RGBA*img, int width, int height, RGBA*palette)
{
    int len = width*height;
    int t;
    int palsize = 0;
    U32* pal;
    int size[256];
    int palette_overflow = 0;
    U32 lastcol32 = 0;

    pal = (U32*)malloc(65536*sizeof(U32));
    memset(size, 0, sizeof(size));

    _Static_assert(sizeof(RGBA) == sizeof(U32), "sizeof(RGBA)!=sizeof(U32))");

    lastcol32 = (*(U32*)&img[0])^0xffffffff; // don't match

    for (t = 0; t < len; t++)
    {
        U32 hash, col32 = *(U32*)&img[t];
        int i, csize;
        U32* cpal;
        if (col32 == lastcol32)
            continue;
        hash = (col32 >> 17) ^ col32;
        hash ^= ((hash>>8) + 1) ^ hash;
        hash &= 255;

        csize = size[hash];
        cpal = &pal[hash*256];
        for (i  =0; i < csize; i++)
        {
            if (col32 == cpal[i])
                break;
        }
        if (i == csize)
        {
            if (palsize==256)
            {
                palette_overflow = 1;
                break;
            }
            cpal[size[hash]++] = col32;
            palsize++;
        }
        lastcol32 = col32;
    }
    if (palette_overflow)
    {
        free(pal);
        return width*height;
    }
    if (palette)
    {
        int i = 0;
        for(t = 0; t < 256; t++)
        {
            int s;
            int csize = size[t];
            U32* cpal = &pal[t*256];
            for (s = 0; s < csize; s++)
                palette[i++] = *(RGBA*)(&cpal[s]);
        }
    }
    free(pal);
    return palsize;
}

RGBA *swf_JPEG2TagToImage(TAG *tag, int *width, int *height)
{
    RGBA *dest;
    int y;
    int offset = 0;
    int oldtaglen = 0;
    *width = 0;
    *height = 0;

    if (tag->id == ST_DEFINEBITSJPEG)
    {
#ifdef _DEBUG
        printf("rfxswf: extracting from definebitsjpeg not yet supported\n");
#endif
        assert(0);
        return 0;
    }
    if (tag->id == ST_DEFINEBITSJPEG3)
    {
        offset = swf_GetU32(tag);
        oldtaglen = tag->len;
        tag->len = offset+6;
    }

#if defined(HAVE_JPEGLIB)
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    struct jpeg_source_mgr mgr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    cinfo.client_data = (void *) tag;
    cinfo.src = &mgr;
    cinfo.src->init_source = tag_init_source;
    cinfo.src->fill_input_buffer = tag_fill_input_buffer;
    cinfo.src->skip_input_data = tag_skip_input_data;
    cinfo.src->resync_to_restart = jpeg_resync_to_restart;
    cinfo.src->term_source = tag_term_source;
    cinfo.out_color_space = JCS_RGB;

    jpeg_read_header(&cinfo, TRUE);
    *width = cinfo.image_width;
    *height = cinfo.image_height;
    dest = (RGBA*)malloc(sizeof(RGBA) * cinfo.image_width * cinfo.image_height);

    jpeg_start_decompress(&cinfo);
    for (y = 0; y < cinfo.output_height; y++) {
        RGBA *line = &dest[y * cinfo.image_width];
        U8 *to = (U8 *) line;
        int x;
        jpeg_read_scanlines(&cinfo, &to, 1);
        for (x = cinfo.output_width - 1; x >= 0; --x) {
            int r = to[x * 3 + 0];
            int g = to[x * 3 + 1];
            int b = to[x * 3 + 2];
            line[x].r = r;
            line[x].g = g;
            line[x].b = b;
            line[x].a = 255;
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
#else
    dest = (RGBA *)stbi_load_from_memory(&tag->data[tag->pos], tag->len - tag->pos, width, height, &y, 4);
#endif

    if (offset)
    {
        size_t datalen = (*width)*(*height);
        U8* alphadata = (U8*)malloc(datalen);
        tag->len = oldtaglen;
        swf_SetTagPos(tag, 6+offset);
#ifdef HAVE_ZLIB
        int error = uncompress(alphadata, &datalen, &tag->data[tag->pos], tag->len - tag->pos);
        if (error != Z_OK)
        {
#ifdef _DEBUG
            printf("rfxswf: Zlib error %d while extracting definejpeg3\n", error);
#endif
            assert(0);
            return 0;
        }
#else
        stbi_zlib_decode_buffer((char *)alphadata, datalen, (char *)&tag->data[tag->pos], tag->len - tag->pos);
#endif
        for (y = 0; y < (*height); y++)
        {
            RGBA *line = &dest[y*(*width)];
            U8 *aline = &alphadata[y*(*width)];
            int x;
            for (x = 0; x < (*width); x++)
            {
                line[x].r = line[x].r < aline[x] ? line[x].r : aline[x];
                line[x].g = line[x].g < aline[x] ? line[x].g : aline[x];
                line[x].b = line[x].b < aline[x] ? line[x].b : aline[x];
                line[x].a = aline[x];
            }
        }
        free(alphadata);
    }
    return dest;
}

RGBA *swf_DefineLosslessBitsTagToImage(TAG *tag, int *dwidth, int *dheight)
{
    int id, format, height, width, pos;
    size_t datalen;//, datalen2;
#ifdef HAVE_ZLIB
    int error;
#endif
    int bpp = 1;
    int cols = 0;
    int pos2 = 0;
    char alpha = tag->id == ST_DEFINEBITSLOSSLESS2;
    int t, x, y;
    RGBA *palette = 0;
    U8 *data;//, *data2;
    RGBA *dest;
    swf_SetTagPos(tag, 0);
    id = swf_GetU16(tag);
    format = swf_GetU8(tag);
    if (format == 3)
        bpp = 8;
    if (format == 4)
        bpp = 16;
    if (format == 5)
        bpp = 32;
    if (format != 3 && format != 5)
    {
#ifdef _DEBUG
        if (format == 4)
            printf("rfxswf: Can't handle 16-bit palette images yet (image %d)\n", id);
        else
            printf("rfxswf: Unknown image type %d in image %d\n", format, id);
#endif
        assert(0);
        return 0;
    }
    *dwidth = width = swf_GetU16(tag);
    *dheight = height = swf_GetU16(tag);

    dest = (RGBA*)malloc(sizeof(RGBA) * width * height);

    if (format == 3)
        cols = swf_GetU8(tag) + 1;
    else
        cols = 0;

    data = 0;
    datalen = (width * height * bpp / 8 + cols * 8);
#ifdef HAVE_ZLIB
    do {
        if (data)
            free(data);
        datalen += 4096;
        data = (U8*)malloc(datalen);
        error = uncompress(data, &datalen, &tag->data[tag->pos], tag->len - tag->pos);
    } while (error == Z_BUF_ERROR);
    if (error != Z_OK)
    {
#ifdef _DEBUG
        printf("rfxswf: Zlib error %d (image %d)\n", error, id);
#endif
        assert(0);
        return 0;
    }
#else
    data = (U8*)malloc(datalen);
    stbi_zlib_decode_buffer((char *)data, datalen, (char *)&tag->data[tag->pos], tag->len - tag->pos);
#endif
    pos = 0;

    if (cols)
    {
        palette = (RGBA *) malloc(cols * sizeof(RGBA));
        for (t = 0; t < cols; t++)
        {
            palette[t].r = data[pos++];
            palette[t].g = data[pos++];
            palette[t].b = data[pos++];
            if (alpha)
                palette[t].a = data[pos++];
            else
                palette[t].a = 255;
        }
    }

    for (y = 0; y < height; y++)
    {
        int srcwidth = width * (bpp / 8);
        if (bpp == 32)
        {
            if (!alpha)
            {
                // 32 bit to 24 bit "conversion"
                for (x = 0; x < width; x++)
                {
                    dest[pos2].r = data[pos + 1];
                    dest[pos2].g = data[pos + 2];
                    dest[pos2].b = data[pos + 3];
                    dest[pos2].a = 255;
                    pos2++;
                    pos += 4;        //ignore padding byte
                }
            } else
            {
                for (x = 0; x < width; x++)
                {
                    /* remove premultiplication */
                    int alpha = data[pos+0];
                    if(alpha)
                        alpha = 0xff0000/alpha;
                    dest[pos2].r = (data[pos + 1]*alpha)>>16;
                    dest[pos2].g = (data[pos + 2]*alpha)>>16;
                    dest[pos2].b = (data[pos + 3]*alpha)>>16;
                    dest[pos2].a = data[pos + 0];        //alpha
                    pos2++;
                    pos += 4;
                }
            }
        } else
        {
            for (x = 0; x < srcwidth; x++)
            {
                dest[pos2] = palette[data[pos++]];
                pos2++;
            }
        }
        pos += ((srcwidth + 3) & ~3) - srcwidth;        //align
    }
    if (palette)
        free(palette);
    free(data);
    return dest;
}

RGBA *swf_ExtractImage(TAG * tag, int *dwidth, int *dheight)
{
    swf_SetTagPos(tag, 2); // id is 2 bytes

    if (tag->id == ST_DEFINEBITSJPEG || tag->id == ST_DEFINEBITSJPEG2 || tag->id == ST_DEFINEBITSJPEG3)
    {
        return swf_JPEG2TagToImage(tag, dwidth, dheight);
    }
    if (tag->id == ST_DEFINEBITSLOSSLESS || tag->id == ST_DEFINEBITSLOSSLESS2)
    {
        return swf_DefineLosslessBitsTagToImage(tag, dwidth, dheight);
    }
    assert(0);
    return 0;
}

void swf_RemoveJPEGTables(SWF *swf)
{
    TAG *tag = swf->firstTag;
    TAG *tables_tag = 0;
    while (tag)
    {
        if (tag->id == ST_JPEGTABLES)
            tables_tag = tag;
        tag = tag->next;
    }

    if (!tables_tag)
        return;

    tag = swf->firstTag;
    while (tag)
    {
        if (tag->id == ST_DEFINEBITSJPEG)
        {
            int len = tag->len;
            void *data = malloc(len);
            swf_GetBlock(tag, (U8*)data, tag->len);
            swf_ResetTag(tag, ST_DEFINEBITSJPEG2);
            swf_SetBlock(tag, &((U8*)data)[0], 2); //id
            swf_SetBlock(tag, tables_tag->data, tables_tag->len);
            swf_SetBlock(tag, &((U8*)data)[2], len-2);
            free(data);
        }
        tag = tag->next;
    }
    if (swf->firstTag == tables_tag)
        swf->firstTag = tables_tag->next;
    swf_DeleteTag(swf, tables_tag);
}

