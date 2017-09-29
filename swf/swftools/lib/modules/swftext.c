/* swftext.c

   Text and font routines

   Extension module for the rfxswf library.
   Part of the swftools package.

   Copyright (c) 2001 Rainer B
   Copyright (c) 2003,2004,2005,2006,2007,2008,2009 Matthias Kramm

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

#include "../rfxswf.h"

U32 readUTF8char(U8 **text)
{
    U32 c = 0;
    if (!(*(*text) & 0x80))
        return *((*text)++);

    /* 0000 0080-0000 07FF   110xxxxx 10xxxxxx */
    if (((*text)[0] & 0xe0) == 0xc0 && (*text)[1])
    {
        c = ((*text)[0] & 0x1f) << 6 | ((*text)[1] & 0x3f);
        (*text) += 2;
        return c;
    }
    /* 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx */
    if (((*text)[0] & 0xf0) == 0xe0 && (*text)[1] && (*text)[2])
    {
        c = ((*text)[0] & 0x0f) << 12 | ((*text)[1] & 0x3f) << 6 | ((*text)[2] & 0x3f);
        (*text) += 3;
        return c;
    }
    /* 0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (((*text)[0] & 0xf8) == 0xf0 && (*text)[1] && (*text)[2] && (*text)[3])
    {
        c = ((*text)[0] & 0x07) << 18 | ((*text)[1] & 0x3f) << 12 | ((*text)[2] & 0x3f) << 6 | ((*text)[3] & 0x3f);
        (*text) += 4;
        return c;
    }
    /* 0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (((*text)[0] & 0xfc) == 0xf8 && (*text)[1] && (*text)[2] && (*text)[3] && (*text)[4])
    {
        c = ((*text)[0] & 0x03) << 24 | ((*text)[1] & 0x3f) << 18 | ((*text)[2] & 0x3f) << 12 | ((*text)[3] & 0x3f) << 6 | ((*text)[4] & 0x3f);
        (*text) += 5;
        return c;
    }
    /* 0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx */
    if (((*text)[0] & 0xfe) == 0xfc && (*text)[1] && (*text)[2] && (*text)[3] && (*text)[4] && (*text)[5])
    {
        c = ((*text)[0] & 0x01) << 30 | ((*text)[1] & 0x3f) << 24 |
            ((*text)[2] & 0x3f) << 18 | ((*text)[3] & 0x3f) << 12 | ((*text)[4] & 0x3f) << 6 | ((*text)[5] & 0x3f) << 6;
        (*text) += 6;
        return c;
    }
    return *((*text)++);
}

#define TF_TEXTCONTROL  0x80
#define TF_HASFONT      0x08
#define TF_HASCOLOR     0x04
#define TF_HASYOFFSET   0x02
#define TF_HASXOFFSET   0x01

#define FF_WIDECODES    0x01
#define FF_BOLD         0x02
#define FF_ITALIC       0x04
#define FF_ANSI         0x08
#define FF_SHIFTJIS     0x10
#define FF_UNICODE      0x20

#define FF2_BOLD         0x01
#define FF2_ITALIC       0x02
#define FF2_WIDECODES    0x04
#define FF2_WIDEOFFSETS  0x08
#define FF2_ANSI         0x10
#define FF2_UNICODE      0x20
#define FF2_SHIFTJIS     0x40
#define FF2_LAYOUT	 0x80

int swf_FontIsItalic(SWFFONT * f)
{
    return f->style & FONT_STYLE_ITALIC;
}

int swf_FontIsBold(SWFFONT * f)
{
    return f->style & FONT_STYLE_BOLD;
}

int swf_FontExtract_DefineFont(int id, SWFFONT * f, TAG * t)
{
    U16 fid;
    swf_SetTagPos(t, 0);

    fid = swf_GetU16(t);
    if ((!id) || (id == fid))
    {
        U16 of;
        int n, i;

        id = fid;
        f->version = 1;
        f->id = fid;

        of = swf_GetU16(t);
        n = of / 2;
        f->numchars = n;
        f->glyph = (SWFGLYPH*)calloc(1, sizeof(SWFGLYPH) * n);

        for (i = 1; i < n; i++)
            swf_GetU16(t);
        for (i = 0; i < n; i++)
            swf_GetSimpleShape(t, &f->glyph[i].shape);
    }
    return id;
}

int swf_FontExtract_DefineFontInfo(int id, SWFFONT *f, TAG *t)
{
    U16 fid;
    U16 maxcode;
    U8 flags;
    swf_SetTagPos(t, 0);

    fid = swf_GetU16(t);
    if (fid == id)
    {
        U8 l = swf_GetU8(t);
        int i;

        if (f->version > 1)
        {
            /* Especially with Flash MX, DefineFont2 may have FontInfo fields,
               too. However, they only add little information to what's already
               inside the DefineFont2 tag */
            return id;
        }

        if (f->name)
            free(f->name);

        f->name = (U8 *) malloc(l + 1);
        swf_GetBlock(t, f->name, l);
        f->name[l] = 0;

        flags = swf_GetU8(t);
        if (flags & 2)
            f->style |= FONT_STYLE_BOLD;
        if (flags & 4)
            f->style |= FONT_STYLE_ITALIC;
        if (flags & 8)
            f->encoding |= FONT_ENCODING_ANSI;
        if (flags & 16)
            f->encoding |= FONT_ENCODING_SHIFTJIS;
        if (flags & 32)
            f->encoding |= FONT_ENCODING_UNICODE;

        if (t->id == ST_DEFINEFONTINFO2)
        {
            f->language = swf_GetU8(t);
        }

        f->glyph2ascii = (U16 *) malloc(sizeof(U16) * f->numchars);
        maxcode = 0;
        for (i = 0; i < f->numchars; i++)
        {
            f->glyph2ascii[i] = ((flags & FF_WIDECODES) ? swf_GetU16(t) : swf_GetU8(t));
            if (f->glyph2ascii[i] > maxcode)
                maxcode = f->glyph2ascii[i];
        }
        maxcode++;
        if (maxcode < 256)
            maxcode = 256;
        f->maxascii = maxcode;
        f->ascii2glyph = (int *) malloc(sizeof(int) * maxcode);
        memset(f->ascii2glyph, -1, sizeof(int) * maxcode);

        for (i = 0; i < f->numchars; i++)
            f->ascii2glyph[f->glyph2ascii[i]] = i;
    }
    return id;
}

int swf_FontExtract_GlyphNames(int id, SWFFONT * f, TAG * tag)
{
    U16 fid;
    swf_SetTagPos(tag, 0);

    fid = swf_GetU16(tag);

    if (fid == id)
    {
        int num = swf_GetU16(tag);
        int t;
        f->glyphnames = (char**)malloc(sizeof(char *) * num);
        for (t = 0; t < num; t++)
        {
            f->glyphnames[t] = strdup(swf_GetString(tag));
        }
    }
    return id;
}

int swf_FontExtract_DefineFont2(int id, SWFFONT *font, TAG *tag)
{
    int t, glyphcount;
    int maxcode;
    int fid;
    U32 offset_start;
    U32 *offset;
    U8 flags1, /*langcode, */namelen;
    swf_SetTagPos(tag, 0);
    font->version = tag->id==ST_DEFINEFONT3?3:2;
    fid = swf_GetU16(tag);
    if (id && id != fid)
        return id;
    font->id = fid;
    flags1 = swf_GetU8(tag);
    /*langcode = */swf_GetU8(tag);	//reserved flags

    if (flags1 & 1)
        font->style |= FONT_STYLE_BOLD;
    if (flags1 & 2)
        font->style |= FONT_STYLE_ITALIC;
    if (flags1 & 16)
        font->encoding |= FONT_ENCODING_ANSI;
    if (flags1 & 32)
        font->encoding |= FONT_ENCODING_UNICODE;
    if (flags1 & 64)
        font->encoding |= FONT_ENCODING_SHIFTJIS;

    namelen = swf_GetU8(tag);
    font->name = (U8 *) malloc(namelen + 1);
    font->name[namelen] = 0;
    swf_GetBlock(tag, font->name, namelen);
    glyphcount = swf_GetU16(tag);
    font->numchars = glyphcount;

    font->glyph = (SWFGLYPH *) calloc(1, sizeof(SWFGLYPH) * glyphcount);
    font->glyph2ascii = (U16 *) calloc(1, sizeof(U16) * glyphcount);

    offset = (U32*)calloc(1, sizeof(U32)*(glyphcount+1));
    offset_start = tag->pos;

    if (flags1 & 8)
    {   // wide offsets
        for (t = 0; t < glyphcount; t++)
            offset[t] = swf_GetU32(tag);	//offset[t]

        if (glyphcount)		/* this _if_ is not in the specs */
            offset[glyphcount] = swf_GetU32(tag);	// fontcodeoffset
        else
            offset[glyphcount] = tag->pos;
    } else
    {
        for (t = 0; t < glyphcount; t++)
            offset[t] = swf_GetU16(tag);	//offset[t]

        if (glyphcount)		/* this _if_ is not in the specs */
            offset[glyphcount] = swf_GetU16(tag);	// fontcodeoffset
        else
            offset[glyphcount] = tag->pos;
    }
    for (t = 0; t < glyphcount; t++)
    {
        swf_SetTagPos(tag, offset[t] + offset_start);
        swf_GetSimpleShape(tag, &(font->glyph[t].shape));
    }

    if (glyphcount)
        swf_SetTagPos(tag, offset[glyphcount] + offset_start);

    free(offset);

    maxcode = 0;
    for (t = 0; t < glyphcount; t++)
    {
        int code;
        if (flags1 & 4)		// wide codes (always on for definefont3)
            code = swf_GetU16(tag);
        else
            code = swf_GetU8(tag);
        font->glyph2ascii[t] = code;
        if (code > maxcode)
            maxcode = code;
    }
    maxcode++;
    if (maxcode < 256)
        maxcode = 256;
    font->maxascii = maxcode;
    font->ascii2glyph = (int *) malloc(sizeof(int) * maxcode);
    memset(font->ascii2glyph, -1, sizeof(int) * maxcode);
    for (t = 0; t < glyphcount; t++)
    {
        font->ascii2glyph[font->glyph2ascii[t]] = t;
    }

    if (flags1 & 128)
    {   // has layout
        U16 kerningcount;
        font->layout = (SWFLAYOUT *) malloc(sizeof(SWFLAYOUT));
        font->layout->ascent = swf_GetU16(tag);
        font->layout->descent = swf_GetU16(tag);
        font->layout->leading = swf_GetU16(tag);
        for (t = 0; t < glyphcount; t++)
        {
            S16 advance = swf_GetS16(tag);
            font->glyph[t].advance = advance;
        }
        font->layout->bounds = (SRECT*)malloc(glyphcount * sizeof(SRECT));
        for (t = 0; t < glyphcount; t++)
        {
            swf_ResetReadBits(tag);
            swf_GetRect(tag, &font->layout->bounds[t]);
            SRECT b = font->layout->bounds[t];
            if ((b.xmin|b.xmax|b.ymin|b.ymax) == 0)
            {
                // recalculate bounding box
                SHAPE2 *shape2 = swf_ShapeToShape2(font->glyph[t].shape);
                font->layout->bounds[t] = swf_GetShapeBoundingBox(shape2);
                swf_Shape2Free(shape2); free(shape2);
            }
        }

        kerningcount = swf_GetU16(tag);
        font->layout->kerningcount = kerningcount;

        font->layout->kerning = (SWFKERNING *) malloc(sizeof(SWFKERNING) * kerningcount);
        if (kerningcount)
        {
            font->layout->kerning = (SWFKERNING*)malloc(sizeof(*font->layout->kerning) * kerningcount);
            for (t = 0; t < kerningcount; t++)
            {
                if (flags1 & 4)
                {	// wide codes
                    font->layout->kerning[t].char1 = swf_GetU16(tag);
                    font->layout->kerning[t].char2 = swf_GetU16(tag);
                } else
                {
                    font->layout->kerning[t].char1 = swf_GetU8(tag);
                    font->layout->kerning[t].char2 = swf_GetU8(tag);
                }
                font->layout->kerning[t].adjustment = swf_GetS16(tag);
            }
        }
    }
    return font->id;
}

int swf_FontExtract_DefineFontAlignZones(int id, SWFFONT * font, TAG * tag)
{
    U16 fid;
    swf_SetTagPos(tag, 0);
    fid = swf_GetU16(tag);

    if (fid == id) {
        font->alignzone_flags = swf_GetU8(tag);
        font->alignzones = calloc(1, sizeof(ALIGNZONE)*font->numchars);
        int i=0;
        while(tag->pos < tag->len) {
            if(i>=font->numchars)
                break;
            int nr = swf_GetU8(tag); // should be 2
            if(nr!=1 && nr!=2) {
                fprintf(stderr, "rfxswf: Can't parse alignzone tags with %d zones", nr);
                break;
            }
            U16 x = swf_GetU16(tag);
            U16 y = swf_GetU16(tag);
            U16 dx = (nr==2)?swf_GetU16(tag):0xffff;
            U16 dy = (nr==2)?swf_GetU16(tag):0xffff;
            U8 xy = swf_GetU8(tag);

#ifdef DEBUG_RFXSWF
            if((!(xy&1) && (x!=0 || (dx!=0 && dx!=0xffff))) ||
                    (!(xy&2) && (y!=0 || (dy!=0 && dy!=0xffff)))) {
                fprintf(stderr, "Warning: weird combination of alignzone bits and values (%d x:%04x-%04x y:%04x-%04x)\n", xy,
                        x,dx,y,dy);
            }
#endif
            if(!(xy&1)) {
                x = 0xffff;
                dx = 0xffff;
            } else if(!(xy&2)) {
                y = 0xffff;
                dy = 0xffff;
            }
            font->alignzones[i].x = x;
            font->alignzones[i].y = y;
            font->alignzones[i].dx = dx;
            font->alignzones[i].dy = dy;
            i++;
        }
    }
    return id;
}

int swf_FontSetID(SWFFONT *f, U16 id)
{
    if (!f)
        return -1;
    f->id = id;
    return 0;
}

void swf_LayoutFree(SWFLAYOUT * l)
{
    if (l) {
        if (l->kerning)
            free(l->kerning);
        l->kerning = NULL;
        if (l->bounds)
            free(l->bounds);
        l->bounds = NULL;
    }
    free(l);
}

static void font_freeglyphnames(SWFFONT*f)
{
    if (f->glyphnames)
    {
        int t;
        for (t = 0; t < f->numchars; t++)
        {
            if (f->glyphnames[t])
            {
                free(f->glyphnames[t]);
                f->glyphnames[t] = 0;
            }
        }
        free(f->glyphnames);
        f->glyphnames = 0;
    }
}

static void font_freeusage(SWFFONT*f)
{
    if (f->use)
    {
        if (f->use->chars)
        {
            free(f->use->chars);f->use->chars = 0;
        }
        if (f->use->neighbors)
        {
            free(f->use->neighbors);f->use->neighbors = 0;
        }
        if (f->use->neighbors_hash)
        {
            free(f->use->neighbors_hash);f->use->neighbors_hash = 0;
        }
        free(f->use); f->use = 0;
    }
}

static void font_freelayout(SWFFONT*f)
{
    if (f->layout)
    {
        swf_LayoutFree(f->layout);
        f->layout = 0;
    }
}

static void font_freename(SWFFONT*f)
{
    if (f->name)
    {
        free(f->name);
        f->name = 0;
    }
}

//static SWFFONT* font_to_sort;
int cmp_chars(const void*a, const void*b)
{
    //    int x = *(const int*)a;
    //    int y = *(const int*)b;
    return 0;
}

void swf_FontSort(SWFFONT * font)
{
    int i, j;
    int *newplace;
    int *newpos;
    if (!font)
        return;

    newplace = (int*)malloc(sizeof(int) * font->numchars);

    for (i = 0; i < font->numchars; i++)
    {
        newplace[i] = i;
    }
    //qsort(newplace, sizeof(newplace[0]), font->numchars, cmp_chars);

    for (i = 0; i < font->numchars; i++)
        for (j = 0; j < i; j++)
        {
            if (font->glyph2ascii[i] < font->glyph2ascii[j])
            {
                int n1, n2;
                char *c1, *c2;
                SWFGLYPH g1, g2;
                SRECT r1, r2;
                n1 = newplace[i];
                n2 = newplace[j];
                newplace[j] = n1;
                newplace[i] = n2;
                n1 = font->glyph2ascii[i];
                n2 = font->glyph2ascii[j];
                font->glyph2ascii[j] = n1;
                font->glyph2ascii[i] = n2;
                g1 = font->glyph[i];
                g2 = font->glyph[j];
                font->glyph[j] = g1;
                font->glyph[i] = g2;
                if (font->glyphnames)
                {
                    c1 = font->glyphnames[i];
                    c2 = font->glyphnames[j];
                    font->glyphnames[j] = c1;
                    font->glyphnames[i] = c2;
                }
                if (font->layout)
                {
                    r1 = font->layout->bounds[i];
                    r2 = font->layout->bounds[j];
                    font->layout->bounds[j] = r1;
                    font->layout->bounds[i] = r2;
                }
            }
        }
    newpos = (int*)malloc(sizeof(int) * font->numchars);
    for (i = 0; i < font->numchars; i++)
    {
        newpos[newplace[i]] = i;
    }
    for (i = 0; i < font->maxascii; i++)
    {
        if (font->ascii2glyph[i] >= 0)
            font->ascii2glyph[i] = newpos[font->ascii2glyph[i]];
    }

    free(newplace);
    font->glyph2glyph = newpos;
}

static unsigned hash2(int char1, int char2)
{
    unsigned hash = char1^(char2<<8);
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

int swf_FontUseGetPair(SWFFONT * f, int char1, int char2)
{
    FONTUSAGE*u = f->use;
    if (!u || !u->neighbors_hash_size)
        return 0;
    unsigned hash = hash2(char1, char2);
    while (1)
    {
        hash = hash%u->neighbors_hash_size;
        int pos = u->neighbors_hash[hash];
        if (!pos)
            return 0;
        if (pos && u->neighbors[pos-1].char1 == char1 && u->neighbors[pos-1].char2 == char2)
            return pos;
        hash++;
    }

}

void swf_FontAddLayout(SWFFONT * f, int ascent, int descent, int leading)
{
    f->layout = (SWFLAYOUT *) malloc(sizeof(SWFLAYOUT));
    f->layout->ascent = ascent;
    f->layout->descent = descent;
    f->layout->leading = leading;
    f->layout->kerningcount = 0;
    f->layout->kerning = 0;
    f->layout->bounds = (SRECT *) calloc(1, sizeof(SRECT) * f->numchars);
}

static void font_freealignzones(SWFFONT * f)
{
    if(f->alignzones)
        free(f->alignzones);
    f->alignzones = 0;
}

void swf_FontFree(SWFFONT * f)
{
    int i;
    if (!f)
        return;

    if (f->glyph)
    {
        for (i = 0; i < f->numchars; i++)
            if (f->glyph[i].shape)
            {
                swf_ShapeFree(f->glyph[i].shape);
                f->glyph[i].shape = NULL;
            }
        free(f->glyph);
        f->glyph = NULL;
    }
    if (f->ascii2glyph)
    {
        free(f->ascii2glyph);
        f->ascii2glyph = NULL;
    }
    if (f->glyph2ascii)
    {
        free(f->glyph2ascii);
        f->glyph2ascii = NULL;
    }
    if (f->glyph2glyph) {
        free(f->glyph2glyph);
        f->glyph2glyph = NULL;
    }
    font_freename(f);
    font_freelayout(f);
    font_freeglyphnames(f);
    font_freeusage(f);
    font_freealignzones(f);

    free(f);
}
