/* swfshape.c

 shape functions

 Extension module for the rfxswf library.
 Part of the swftools package.

 Copyright (c) 2001 Rainer B

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
#include <assert.h>

#define SF_MOVETO       0x01
#define SF_FILL0        0x02
#define SF_FILL1        0x04
#define SF_LINE         0x08
#define SF_NEWSTYLE     0x10

void swf_ShapeFree(SHAPE *s)
{
    if (!s)
        return;
    if (s->linestyle.data)
        free(s->linestyle.data);
    s->linestyle.data = NULL;
    s->linestyle.n    = 0;
    if (s->fillstyle.data)
        free(s->fillstyle.data);
    s->fillstyle.data = NULL;
    s->fillstyle.n    = 0;
    if (s->data)
        free(s->data);
    s->data = NULL;
    free(s);
}

int swf_ShapeNew(SHAPE * * s)
{
    SHAPE * sh;
    if (!s) return -1;
    sh = (SHAPE *)calloc(1, sizeof(SHAPE));
    *s = sh;
    return 0;
}

int swf_GetSimpleShape(TAG *t, SHAPE **s) // without Linestyle/Fillstyle Record
{
    SHAPE * sh;
    int bitl, len;
    int end;
    uint32_t pos;

    if (FAILED(swf_ShapeNew(s)))
        return -1;
    sh = s[0];

    swf_ResetReadBits(t);
    bitl = 8; end = 0; pos = swf_GetTagPos(t);
    sh->bits.fill = (uint16_t)swf_GetBits(t, 4);
    sh->bits.line = (uint16_t)swf_GetBits(t, 4);

    while (!end)
    {
        int edge = swf_GetBits(t, 1);
        bitl += 1;
        if (edge)
        {
            bitl += 1;
            if (swf_GetBits(t, 1))              // Line
            {
                uint16_t nbits = swf_GetBits(t,4)+2;
                bitl += 5;

                if (swf_GetBits(t,1))           // x/y Line
                {
                    swf_GetBits(t,nbits);
                    swf_GetBits(t,nbits);
                    bitl+=nbits*2;
                } else                          // hline/vline
                {
                    swf_GetBits(t,nbits+1);
                    bitl+=nbits+1;
                }
            } else                              // Curve
            {
                uint16_t nbits = swf_GetBits(t,4)+2;
                bitl += 4;

                swf_GetBits(t, nbits);
                swf_GetBits(t, nbits);
                swf_GetBits(t, nbits);
                swf_GetBits(t, nbits);

                bitl += 4*nbits;
            }
        } else
        {
            uint16_t flags = swf_GetBits(t, 5);
            bitl += 5;
            if (flags)
            {
                if (flags & SF_MOVETO)
                {
                    uint16_t nbits = swf_GetBits(t, 5);
                    bitl += 5;
                    swf_GetBits(t, nbits);
                    swf_GetBits(t, nbits);
                    bitl += 2*nbits;
                }
                if (flags & SF_FILL0)
                {
                    swf_GetBits(t,sh->bits.fill);
                    bitl+=sh->bits.fill;
                }
                if (flags & SF_FILL1)
                {
                    swf_GetBits(t,sh->bits.fill);
                    bitl+=sh->bits.fill;
                }
                if (flags & SF_LINE)
                {
                    swf_GetBits(t,sh->bits.line);
                    bitl+=sh->bits.line;
                }
                if (flags & SF_NEWSTYLE)
                {
#ifdef _DEBUG
                    printf("RFXSWF: Can't process extended styles in shape.\n");
#endif
                }
            } else
                end = 1;
        }
    }
    swf_SetTagPos(t, pos);
    len = (bitl + 7)/8;

    if (sh->data)
        free(sh->data);
    sh->data = (uint8_t*)malloc(len);

    if (sh->data)
    {
        sh->bitlen = bitl;
        swf_GetBlock(t, sh->data, len);
    } else
        return -1;

    return len;
}

int swf_ShapeAddFillStyle(SHAPE *s, uint8_t type, MATRIX *m, RGBA *color,uint16_t id_bitmap, GRADIENT *gradient)
{
    RGBA def_c;
    MATRIX def_m;
    GRADIENT def_g;

    // handle defaults

    if (!s) return -1;
    if (!color)
    {
        color = &def_c;
        def_c.a = 0xff;
        def_c.r = def_c.g = def_c.b = 0;
    }
    if (!m)
    {
        m = &def_m;
        swf_GetMatrix(NULL, m);
    }
    if (!gradient)
    {
        gradient = &def_g;
        swf_GetGradient(NULL, gradient, 1);
    }

    // handle memory
    if (s->fillstyle.data)
    {
        FILLSTYLE *xnew = (FILLSTYLE *)realloc(s->fillstyle.data, (s->fillstyle.n + 1)*sizeof(FILLSTYLE));
        if (!xnew)
            return -1;
        s->fillstyle.data = xnew;
        memset(s->fillstyle.data + s->fillstyle.n, 0, sizeof(FILLSTYLE));
    } else
    {
        s->fillstyle.data = (FILLSTYLE *)calloc(1, sizeof(FILLSTYLE));
        s->fillstyle.n = 0;
        if (!s->fillstyle.data)
            return -1;
    }
    // set fillstyle
    s->fillstyle.data[s->fillstyle.n].type = type;
    s->fillstyle.data[s->fillstyle.n].id_bitmap = id_bitmap;
    memcpy(&s->fillstyle.data[s->fillstyle.n].m,m,sizeof(MATRIX));
    memcpy(&s->fillstyle.data[s->fillstyle.n].color,color,sizeof(RGBA));
    memcpy(&s->fillstyle.data[s->fillstyle.n].gradient,gradient,sizeof(GRADIENT));

    return (++s->fillstyle.n);
}

int swf_ShapeAddSolidFillStyle(SHAPE * s, RGBA *color)
{
    return swf_ShapeAddFillStyle(s, FILL_SOLID, NULL, color, 0, 0);
}

static void parseFillStyle(FILLSTYLE *dest, TAG *tag, int num)
{
    int morph = 0, type = swf_GetU8(tag); //type
    dest->type = type;
    if (tag->id == ST_DEFINEMORPHSHAPE || tag->id==ST_DEFINEMORPHSHAPE2)
        morph = 1;
    if (type == 0)
    {   /* plain color */
        if (num >= 3)
            { swf_GetRGBA(tag, &dest->color); if (morph) swf_GetRGBA(tag, NULL); }
        else
            { swf_GetRGB(tag, &dest->color);  if (morph) swf_GetRGB(tag, NULL); }
    } else if (type == 0x10 || type == 0x11 || type == 0x12 || type == 0x13)
    {   /* linear/radial gradient fill */
        swf_ResetReadBits(tag);
        swf_GetMatrix(tag, &dest->m);
        if (morph)
            swf_GetMatrix(tag, NULL);
        swf_ResetReadBits(tag);
        if (morph)
        {
            //GRADIENT gradient2;
            swf_GetMorphGradient(tag, &dest->gradient, /*&gradient2*/0);
            if (type == 0x13)
            {
                dest->gradient.focal = swf_GetFixed8(tag);
                swf_GetU16(tag);
            }
        } else
        {
            swf_GetGradient(tag, &dest->gradient, num >= 3 ? 1 : 0);
            if (type == 0x13)
                dest->gradient.focal = swf_GetFixed8(tag);
        }
    } else if (type == 0x40 || type == 0x41 || type == 0x42 || type == 0x43)
    {   /* bitmap fill */
        swf_ResetReadBits(tag);
        dest->id_bitmap = swf_GetU16(tag); //id
        swf_GetMatrix(tag, &dest->m);
        if (morph)
            swf_GetMatrix(tag, NULL);
    } else
    {
#ifdef _DEBUG
        printf("rfxswf:swfshape.c Unknown fillstyle:0x%02x in tag %02d\n",type, tag->id);
#endif
        assert(0);
    }
}

int parseFillStyleArray(TAG *tag, SHAPE2 *shape)
{
    uint16_t count;
    int t;
    int num=0, morph = 0;
    int fillstylestart = shape->numfillstyles;
    int linestylestart = shape->numlinestyles;

    if(tag->id == ST_DEFINESHAPE)
        num = 1;
    else if(tag->id == ST_DEFINESHAPE2)
        num = 2;
    else if(tag->id == ST_DEFINESHAPE3)
        num = 3;
    else if(tag->id == ST_DEFINESHAPE4)
        num = 4;
    else if(tag->id == ST_DEFINEMORPHSHAPE)
        num = 3;
    else if(tag->id == ST_DEFINEMORPHSHAPE2)
        num = 4;
    if (tag->id == ST_DEFINEMORPHSHAPE || tag->id == ST_DEFINEMORPHSHAPE2)
        morph = 1;

    count = swf_GetU8(tag);
    if (count == 0xff && num > 1) // defineshape2,3 only
        count = swf_GetU16(tag);

    shape->numfillstyles += count;
    if (shape->numfillstyles)
    {
        shape->fillstyles = (FILLSTYLE*)realloc(shape->fillstyles, sizeof(FILLSTYLE)*shape->numfillstyles);
        memset(shape->fillstyles + fillstylestart, 0, (shape->numfillstyles - fillstylestart)*sizeof(FILLSTYLE));

        for (t = fillstylestart; t < shape->numfillstyles; t++)
            parseFillStyle(&shape->fillstyles[t], tag, num);
    }

    swf_ResetReadBits(tag);
    count = swf_GetU8(tag); // line style array
    if (count == 0xff)
        count = swf_GetU16(tag);

    shape->numlinestyles += count;
    if (count)
    {
        shape->linestyles = (LINESTYLE*)realloc(shape->linestyles, sizeof(LINESTYLE)*shape->numlinestyles);
        memset(shape->linestyles + linestylestart, 0, (shape->numlinestyles - linestylestart)*sizeof(LINESTYLE));
        /* TODO: should we start with 1 and insert a correct definition of the "built in" linestyle 0? */
        for (t = linestylestart; t < shape->numlinestyles; t++)
        {
            int fill = 0;
            shape->linestyles[t].width = swf_GetU16(tag);
            if (morph)
                swf_GetU16(tag);

            if (num >= 4)
            {
                uint16_t flags = swf_GetU16(tag);
                shape->linestyles[t].flags = flags;
                if ((flags & 0x30) == 0x20)
                    shape->linestyles[t].mitterLimit = swf_GetFixed8(tag); // miter limit
                if (flags & 0x08)
                {
#ifdef _DEBUG
                    printf("Warning: Filled strokes parsing not yet fully supported\n");
#endif
                    fill = 1;
                }
            }

            if (fill)
            {
                FILLSTYLE f;
                parseFillStyle(&f, tag, num);
                shape->linestyles[t].color = f.color;
            } else
            {
                if (num >= 3)
                    { swf_GetRGBA(tag, &shape->linestyles[t].color); if (morph) swf_GetRGBA(tag, NULL); }
                else
                    { swf_GetRGB(tag, &shape->linestyles[t].color);  if (morph) swf_GetRGB(tag, NULL); }
            }
        }
    }
    return 1;
}

SRECT swf_GetShapeBoundingBox(SHAPE2*shape2)
{
    SRECT r;
    SHAPELINE *l = shape2->lines;
    int lastx = 0, lasty = 0;
    int valid = 0;
    r.xmin = r.ymin = SCOORD_MAX;
    r.xmax = r.ymax = SCOORD_MIN;

    while (l)
    {
        int t1;
        if (l->linestyle > 0)
            t1 = shape2->linestyles[l->linestyle - 1].width*3/2;
        else
            t1 = 0;
        if (l->type == lineTo || l->type == splineTo)
        {
            valid = 1;
            if (lastx - t1 < r.xmin)
                r.xmin = lastx - t1;
            if (lasty - t1 < r.ymin)
                r.ymin = lasty - t1;
            if (lastx + t1 > r.xmax)
                r.xmax = lastx + t1;
            if (lasty + t1 > r.ymax)
                r.ymax = lasty + t1;
            if (l->x - t1 < r.xmin)
                r.xmin = l->x - t1;
            if (l->y - t1 < r.ymin)
                r.ymin = l->y - t1;
            if (l->x + t1 > r.xmax)
                r.xmax = l->x + t1;
            if (l->y + t1 > r.ymax)
                r.ymax = l->y + t1;
            if (l->type == splineTo)
            {
                if (l->sx - t1 < r.xmin)
                    r.xmin = l->sx - t1;
                if (l->sy - t1 < r.ymin)
                    r.ymin = l->sy - t1;
                if (l->sx + t1 > r.xmax)
                    r.xmax = l->sx + t1;
                if (l->sy + t1 > r.ymax)
                    r.ymax = l->sy + t1;
            }
        }
        lastx = l->x;
        lasty = l->y;
        l = l->next;
    }
    if (!valid)
        memset(&r, 0, sizeof(SRECT));
    return r;
}

void swf_ShapeFreeSubpaths(SHAPE2 *s)
{
    if (s->linestyles)
    {
        for (int i = 0; i < s->numlinestyles; i++)
        {
            LINESTYLE *ls = &s->linestyles[i];
            if (ls->subpaths)
            {
                for (int j = 0; j < ls->num_subpaths; j++)
                {
                    if (ls->subpaths[j].subpath)
                        free(ls->subpaths[j].subpath);
                    if (ls->subpaths[j].subpath2)
                        free(ls->subpaths[j].subpath2);
                }
                free(ls->subpaths);
            }
            ls->subpaths = 0;
            ls->num_subpaths = 0;
        }
    }
    if (s->fillstyles)
    {
        for (int i = 0; i < s->numfillstyles; i++)
        {
            FILLSTYLE *fs = &s->fillstyles[i];
            if (fs->subpaths)
            {
                for(int j = 0; j < fs->num_subpaths; j++)
                {
                    if (fs->subpaths[j].subpath)
                        free(fs->subpaths[j].subpath);
                    if (fs->subpaths[j].subpath2)
                        free(fs->subpaths[j].subpath2);
                }
                free(fs->subpaths);
            }
            fs->subpaths = 0;
            fs->num_subpaths = 0;
        }
    }
}

void swf_Shape2Free(SHAPE2 *s)
{
    SHAPELINE *line = s->lines;
    s->lines = 0;
    while (line)
    {
        SHAPELINE*next = line->next;
        line->next = 0;
        free(line);
        line = next;
    }

    swf_ShapeFreeSubpaths(s);

    if (s->linestyles)
    {
        free(s->linestyles);
        s->linestyles = 0;
    }
    if (s->fillstyles)
    {
        for (int i = 0; i < s->numfillstyles; i++)
        {
            FILLSTYLE *f = &s->fillstyles[i];
            if (FILL_LINEAR == f->type || FILL_RADIAL == f->type)
            {
                free(f->gradient.rgba);
                free(f->gradient.ratios);
            }
        }
        free(s->fillstyles);
        s->fillstyles = 0;
    }
    if (s->bbox)
    {
        free(s->bbox);
        s->bbox = 0;
    }
}

SHAPE2 *swf_ShapeToShape2(SHAPE *shape)
{
    SHAPE2 *shape2 = (SHAPE2*)calloc(1, sizeof(SHAPE2));

    shape2->numlinestyles = shape->linestyle.n;
    if (shape2->numlinestyles)
    {
        shape2->linestyles = (LINESTYLE*)malloc(sizeof(LINESTYLE)*shape->linestyle.n);
        memcpy(shape2->linestyles, shape->linestyle.data, sizeof(LINESTYLE)*shape->linestyle.n);
    }

    shape2->numfillstyles = shape->fillstyle.n;
    if (shape2->numfillstyles)
    {
        shape2->fillstyles = (FILLSTYLE*)malloc(sizeof(FILLSTYLE)*shape->fillstyle.n);
        memcpy(shape2->fillstyles, shape->fillstyle.data, sizeof(FILLSTYLE)*shape->fillstyle.n);
    }

    //shape2->lines = swf_ParseShapeData(shape->data, shape->bitlen, shape->bits.fill, shape->bits.line, 1, 0);
    shape2->bbox = 0;
    return shape2;
}

void swf_ParseDefineShape(TAG *tag, SHAPE2 *shape)
{
    int num = 0, morph = 0/*, numshapes = 1, id*/;
    SRECT bbox2, edge_bounds, edge_bounds2;
    //uint16_t fill,line;
    //SHAPELINE*l;
    if(tag->id == ST_DEFINESHAPE)
        num = 1;
    else if(tag->id == ST_DEFINESHAPE2)
        num = 2;
    else if(tag->id == ST_DEFINESHAPE3)
        num = 3;
    else if(tag->id == ST_DEFINESHAPE4)
        num = 4;
    else if(tag->id == ST_DEFINEMORPHSHAPE)
        num = 3;
    else if(tag->id == ST_DEFINEMORPHSHAPE2)
        num = 4;
    if (tag->id == ST_DEFINEMORPHSHAPE || tag->id==ST_DEFINEMORPHSHAPE2) {
        //numshapes = 2;
        morph = 1;
    }

    swf_SetTagPos(tag, 0);

    /*id = */swf_GetU16(tag);
    memset(shape, 0, sizeof(SHAPE2));
    shape->bbox = (SRECT*)malloc(sizeof(SRECT));
    swf_GetRect(tag, shape->bbox);
    if (num >= 4)
    {
        swf_ResetReadBits(tag);
        swf_GetRect(tag, &edge_bounds); // edge bounds
    }
    if (morph)
    {
        swf_ResetReadBits(tag);
        swf_GetRect(tag, &bbox2); // end bounds
        if (num >= 4)
        {
            swf_ResetReadBits(tag);
            swf_GetRect(tag, &edge_bounds2); // end edge bounds
        }
    }
    if (num >= 4)
    {
        //uint8_t flags =
        swf_GetU8(tag); // flags, &1: contains scaling stroke, &2: contains non-scaling stroke
    }
    if (morph)
    {
        shape->endEdgesOffset = swf_GetU32(tag); //offset to endedges
        shape->endEdgesOffset += tag->pos;
    }

    if (!parseFillStyleArray(tag, shape))
        return;

    /*swf_ResetReadBits(tag);
    fill = (uint16_t)swf_GetBits(tag,4);
    line = (uint16_t)swf_GetBits(tag,4);
    if(!fill && !line) {
        printf("fill/line bits are both zero\n");
    }

    shape->lines = swf_ParseShapeData(&tag->data[tag->pos], (tag->len - tag->pos)*8, fill, line, num, shape);
    if (tag->id == ST_DEFINEMORPHSHAPE || tag->id == ST_DEFINEMORPHSHAPE2)
    {
        shape->lines2 = swf_ParseShapeData(&tag->data[tag->pos], (tag->len - tag->pos)*8, fill, line, num, shape);
    }*/
}
