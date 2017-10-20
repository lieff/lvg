/* swftools.c

   Math and matrix functions, misc tools

   Extension module for the rfxswf library.
   Part of the swftools package.

   Copyright (c) 2000, 2001 Rainer B

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

// Matrix & Math tools for SWF files

#include "../rfxswf.h"
#include <assert.h>

uint16_t swf_GetDefineID(TAG * t)
// up to SWF 4.0
{
    uint32_t oldTagPos;
    uint16_t id = 0;

    oldTagPos = swf_GetTagPos(t);
    swf_SetTagPos(t, 0);

    switch (swf_GetTagID(t))
    {
    case ST_DEFINESHAPE:
    case ST_DEFINESHAPE2:
    case ST_DEFINESHAPE3:
    case ST_DEFINESHAPE4:
    case ST_DEFINEMORPHSHAPE:
    case ST_DEFINEMORPHSHAPE2:
    case ST_DEFINEEDITTEXT:
    case ST_DEFINEBITS:
    case ST_DEFINEBITSJPEG2:
    case ST_DEFINEBITSJPEG3:
    case ST_DEFINEBITSLOSSLESS:
    case ST_DEFINEBITSLOSSLESS2:
    case ST_DEFINESCALINGGRID: //pseudodefine
    case ST_DEFINEBUTTON:
    case ST_DEFINEBUTTON2:
    case ST_DEFINEBUTTONCXFORM: //pseudodefine
    case ST_DEFINEBUTTONSOUND: //pseudodefine
    case ST_CSMTEXTSETTINGS: //pseudodefine
    case ST_DEFINEFONT:
    case ST_DEFINEFONT2:
    case ST_DEFINEFONT3:
    case ST_DEFINEFONTINFO: //pseudodefine
    case ST_DEFINEFONTINFO2: //pseudodefine
    case ST_DEFINEFONTALIGNZONES: //pseudodefine
    case ST_DEFINEFONTNAME: //pseudodefine
    case ST_DEFINETEXT:
    case ST_DEFINEBINARY:
    case ST_DEFINETEXT2:
    case ST_DEFINESOUND:
    case ST_DEFINESPRITE:
    case ST_DEFINEMOVIE:
    case ST_DEFINEVIDEOSTREAM:
    case ST_GLYPHNAMES: //pseudodefine
    case ST_VIDEOFRAME: //pseudodefine
    case ST_NAMECHARACTER: //pseudodefine
    case ST_DOINITACTION: //pseudodefine
        id = swf_GetU16(t);
        break;
    default:
#ifdef _DEBUG
        printf("rfxswf: Error: tag %d has no id\n", t->id);
#endif
        assert(0);
    }

    swf_SetTagPos(t, oldTagPos);
    return id;
}

SRECT swf_GetDefineBBox(TAG *t)
{
    uint32_t oldTagPos;
    //uint16_t id = 0;
    SRECT b1,b2;
    memset(&b1, 0, sizeof(b1));

    oldTagPos = swf_GetTagPos(t);
    swf_SetTagPos(t, 0);

    swf_GetRect(0, &b1);

    switch (swf_GetTagID(t))
    {
    case ST_DEFINESHAPE:
    case ST_DEFINESHAPE2:
    case ST_DEFINESHAPE3:
    case ST_DEFINESHAPE4:
    case ST_DEFINEEDITTEXT:
    case ST_DEFINETEXT:
    case ST_DEFINETEXT2:
        /*id = */swf_GetU16(t);
        swf_GetRect(t, &b1);
        break;
    case ST_DEFINEMORPHSHAPE:
    case ST_DEFINEMORPHSHAPE2:
        /*id = */swf_GetU16(t);
        swf_GetRect(t, &b1);
        swf_GetRect(t, &b2);
        swf_ExpandRect2(&b1, &b2);
        break;
    case ST_DEFINEBITSLOSSLESS:
    case ST_DEFINEBITSLOSSLESS2:
    case ST_DEFINEBITS:
    case ST_DEFINEBITSJPEG2:
    case ST_DEFINEBITSJPEG3:
        // FIXME
        break;
    }

    swf_SetTagPos(t, oldTagPos);
    return b1;
}

uint16_t swf_GetPlaceID(TAG * t)
// up to SWF 4.0
{
    uint32_t oldTagPos;
    uint16_t id = 0;

    oldTagPos = swf_GetTagPos(t);
    swf_SetTagPos(t, 0);

    switch (swf_GetTagID(t))
    {
    case ST_PLACEOBJECT:
    case ST_REMOVEOBJECT:
    case ST_FREECHARACTER:
    case ST_STARTSOUND:
        id = swf_GetU16(t);
        break;

    case ST_PLACEOBJECT2:
    {
        uint8_t flags = swf_GetU8(t);
        //      uint16_t d =
        swf_GetU16(t);
        id = (flags & PF_CHAR) ? swf_GetU16(t) : id;
    } break;
    case ST_PLACEOBJECT3:
    {
        uint8_t flags = swf_GetU8(t);
        //      uint8_t flags2 =
        swf_GetU8(t);
        //      uint16_t d =
        swf_GetU16(t);
        id = (flags & PF_CHAR) ? swf_GetU16(t) : id;
    } break;

    }

    swf_SetTagPos(t, oldTagPos);
    return id;
}

static int swf_definingtagids[] =
{
    ST_DEFINESHAPE,
    ST_DEFINESHAPE2,
    ST_DEFINESHAPE3,
    ST_DEFINESHAPE4,
    ST_DEFINEMORPHSHAPE,
    ST_DEFINEMORPHSHAPE2,
    ST_DEFINEFONT,
    ST_DEFINEFONT2,
    ST_DEFINEFONT3,
    ST_DEFINETEXT,
    ST_DEFINETEXT2,
    ST_DEFINEEDITTEXT,
    ST_DEFINEBITS,
    ST_DEFINEBITSJPEG2,
    ST_DEFINEBITSJPEG3,
    ST_DEFINEBITSLOSSLESS,
    ST_DEFINEBITSLOSSLESS2,
    ST_DEFINEMOVIE,
    ST_DEFINESPRITE,
    ST_DEFINEBUTTON,
    ST_DEFINEBUTTON2,
    ST_DEFINESOUND,
    ST_DEFINEVIDEOSTREAM,
    ST_DEFINEBINARY,
    -1
};

// tags which may be used inside a sprite definition
static int swf_spritetagids[] =
{
    ST_SHOWFRAME,
    ST_PLACEOBJECT,
    ST_PLACEOBJECT2,
    ST_PLACEOBJECT3,
    ST_REMOVEOBJECT,
    ST_REMOVEOBJECT2,
    ST_DOACTION,
    ST_DOABC,
    ST_STARTSOUND,
    ST_FRAMELABEL,
    ST_SOUNDSTREAMHEAD,
    ST_SOUNDSTREAMHEAD2,
    ST_SOUNDSTREAMBLOCK,
    ST_END,
    -1
};

/* tags which add content or information to a character with a given ID */
static int swf_pseudodefiningtagids[] =
{
    ST_DEFINEFONTINFO,
    ST_DEFINEFONTINFO2,
    ST_DEFINEFONTALIGNZONES,
    ST_DEFINEFONTNAME,
    ST_DEFINEBUTTONCXFORM,
    ST_DEFINEBUTTONSOUND,
    ST_DEFINESCALINGGRID,
    ST_CSMTEXTSETTINGS,
    ST_NAMECHARACTER,
    ST_DOINITACTION,
    ST_VIDEOFRAME,
    ST_GLYPHNAMES,
    -1
};

uint8_t swf_isAllowedSpriteTag(TAG * tag)
{
    int id = tag->id;
    int t = 0;
    while (swf_spritetagids[t]>=0)
    {
        if(swf_spritetagids[t] == id)
            return 1;
        t++;
    }
    return 0;
}

uint8_t swf_isDefiningTag(TAG * tag)
{
    int id = tag->id;
    int t = 0;
    while (swf_definingtagids[t]>=0)
    {
        if (swf_definingtagids[t] == id)
            return 1;
        t++;
    }
    return 0;
}

uint8_t swf_isPseudoDefiningTag(TAG * tag)
{
    int id = tag->id;
    int t = 0;
    while (swf_pseudodefiningtagids[t]>=0)
    {
        if (swf_pseudodefiningtagids[t] == id)
            return 1;
        t++;
    }
    return 0;
}

int swf_GetDepth(TAG *t)
{
    int depth = -1;
    uint32_t oldTagPos;
    oldTagPos = swf_GetTagPos(t);
    swf_SetTagPos(t, 0);

    switch (swf_GetTagID(t))
    {
    case ST_PLACEOBJECT:
    case ST_REMOVEOBJECT:
        swf_GetU16(t); //id
        depth = swf_GetU16(t);
        break;
    case ST_REMOVEOBJECT2:
        depth = swf_GetU16(t);
        break;
    case ST_PLACEOBJECT2:
    {
        //        uint8_t flags =
        swf_GetU8(t);
        depth = swf_GetU16(t);
    } break;
    case ST_PLACEOBJECT3:
    {
        //        uint8_t flags =
        swf_GetU8(t);
        //        uint8_t flags2 =
        swf_GetU8(t);
        depth = swf_GetU16(t);
    } break;
    case ST_SETTABINDEX:
    {
        depth = swf_GetU16(t);
    }
    }
    swf_SetTagPos(t, oldTagPos);
    return depth;
}

char *swf_GetName(TAG *t)
{
    char *name = 0;
    uint32_t oldTagPos;
    MATRIX m;
    CXFORM c;
    oldTagPos = swf_GetTagPos(t);
    swf_SetTagPos(t, 0);
    switch(swf_GetTagID(t))
    {
    case ST_FRAMELABEL:
        name = (char*)&t->data[swf_GetTagPos(t)];
        break;
    case ST_PLACEOBJECT3:
    case ST_PLACEOBJECT2:
    {
        uint8_t flags = swf_GetU8(t);
        if(t->id == ST_PLACEOBJECT3)
            swf_GetU8(t);
        swf_GetU16(t); //depth;
        if (flags & PF_CHAR)
            swf_GetU16(t); //id
        if (flags & PF_MATRIX)
            swf_GetMatrix(t, &m);
        if (flags & PF_CXFORM)
            swf_GetCXForm(t, &c, 1);
        if (flags & PF_RATIO)
            swf_GetU16(t);
        if (flags & PF_CLIPDEPTH)
            swf_GetU16(t);
        if (flags & PF_NAME)
        {
            swf_ResetReadBits(t);
            name = (char*)&t->data[swf_GetTagPos(t)];
        }
        break;
    }
    }
    swf_SetTagPos(t, oldTagPos);
    return name;
}

void swf_GetMorphGradient(TAG *tag, GRADIENT *gradient1, GRADIENT *gradient2)
{
    int t;
    int num = swf_GetU8(tag) & 15;
    if (gradient1)
        gradient1->num = num;
    if (gradient2)
        gradient2->num = num;

    if (gradient1)
    {
        gradient1->num = num;
        gradient1->rgba = (RGBA*)calloc(1, sizeof(RGBA)*gradient1->num);
        gradient1->ratios = (uint8_t*)calloc(1, sizeof(gradient1->ratios[0])*gradient1->num);
    }
    if (gradient2)
    {
        gradient2->num = num;
        gradient2->rgba = (RGBA*)calloc(1, sizeof(RGBA)*gradient2->num);
        gradient2->ratios = (uint8_t*)calloc(1, sizeof(gradient2->ratios[0])*gradient2->num);
    }
    for (t = 0; t < num; t++)
    {
        uint8_t ratio;
        RGBA color;

        ratio = swf_GetU8(tag);
        swf_GetRGBA(tag, &color);
        if (gradient1)
        {
            gradient1->ratios[t] = ratio;
            gradient1->rgba[t]   = color;
        }

        ratio = swf_GetU8(tag);
        swf_GetRGBA(tag, &color);
        if (gradient2)
        {
            gradient2->ratios[t] = ratio;
            gradient2->rgba[t]   = color;
        }
    }
}

uint8_t swf_isShapeTag(TAG *tag)
{
    if (tag->id == ST_DEFINESHAPE ||
        tag->id == ST_DEFINESHAPE2 ||
        tag->id == ST_DEFINESHAPE3 ||
        tag->id == ST_DEFINESHAPE4 ||
        tag->id == ST_DEFINEMORPHSHAPE ||
        tag->id == ST_DEFINEMORPHSHAPE2)
        return 1;
    return 0;
}

uint8_t swf_isPlaceTag(TAG *tag)
{
    if (tag->id == ST_PLACEOBJECT || tag->id == ST_PLACEOBJECT2 || tag->id == ST_PLACEOBJECT3)
        return 1;
    return 0;
}

uint8_t swf_isTextTag(TAG *tag)
{
    if (tag->id == ST_DEFINETEXT || tag->id == ST_DEFINETEXT2)
        return 1;
    return 0;
}

uint8_t swf_isFontTag(TAG *tag)
{
    if (tag->id == ST_DEFINEFONT ||
        tag->id == ST_DEFINEFONT2 ||
        tag->id == ST_DEFINEFONT3 ||
        tag->id == ST_DEFINEFONTINFO)
        return 1;
    return 0;
}

uint8_t swf_isImageTag(TAG *tag)
{
    if (tag->id == ST_DEFINEBITSJPEG ||
        tag->id == ST_DEFINEBITSJPEG2 ||
        tag->id == ST_DEFINEBITSJPEG3 ||
        tag->id == ST_DEFINEBITSLOSSLESS ||
        tag->id == ST_DEFINEBITSLOSSLESS2)
        return 1;
    return 0;
}
