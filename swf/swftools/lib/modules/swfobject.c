/* swfobject.c

   Object place and move routines

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

int swf_GetPlaceObject(TAG *tag, SWFPLACEOBJECT *obj, int version)
{
    if (obj)
        memset(obj, 0, sizeof(SWFPLACEOBJECT));

    if (!tag)
    {
        swf_GetMatrix(0, &obj->matrix);
        swf_GetCXForm(0, &obj->cxform, 1);
        return PF_MATRIX | PF_CXFORM;
    }
    swf_SetTagPos(tag, 0);

    if (tag->id == ST_PLACEOBJECT)
    {
        obj->id = swf_GetU16(tag);
        obj->depth = swf_GetU16(tag);
        swf_GetMatrix(tag, &obj->matrix);
        swf_GetCXForm(tag, &obj->cxform, 0);
        return PF_CHAR | PF_MATRIX | PF_CXFORM;
    } else if (tag->id == ST_PLACEOBJECT2 || tag->id == ST_PLACEOBJECT3)
    {
        U8 flags, flags2 = 0;
        flags = swf_GetU8(tag);
        if (tag->id == ST_PLACEOBJECT3)
            flags2 = swf_GetU8(tag);
        memset(obj, 0, sizeof(SWFPLACEOBJECT));
        swf_GetMatrix(0, &obj->matrix);
        swf_GetCXForm(0, &obj->cxform, 1);

        obj->flags = flags;
        obj->depth = swf_GetU16(tag);
        if ((flags2 & PF2_CLASS_NAME) || ((flags & PF_CHAR) && (flags2 & PF2_HAS_IMAGE)))
        {
            int t = 0, l = strlen((const char *)&tag->data[tag->pos]);
            U8 *data = (U8*)malloc(l+1);
            obj->class_name = (char*)data;
            while ((data[t++] = swf_GetU8(tag)));
        }
        if (flags & PF_MOVE) obj->move = 1;
        if (flags & PF_CHAR) obj->id = swf_GetU16(tag);
        if (flags & PF_MATRIX) swf_GetMatrix(tag, &obj->matrix);
        if (flags & PF_CXFORM) swf_GetCXForm(tag, &obj->cxform,1);
        if (flags & PF_RATIO) obj->ratio = swf_GetU16(tag);
        if (flags & PF_NAME)
        {
            swf_ResetReadBits(tag);
            int t = 0, l = strlen((const char *)&tag->data[tag->pos]);
            U8 *data = (U8*)malloc(l+1);
            obj->name = (char*)data;
            while((data[t++] = swf_GetU8(tag)));
        }
        if (flags & PF_CLIPDEPTH)
            obj->clipdepth = swf_GetU16(tag);
        if (flags2 & PF2_FILTERS)
        {
            int num_filters = swf_GetU8(tag);
            for (int i = 0; i < num_filters; i++)
            {
                int filter = swf_GetU8(tag);
                if (0 == filter)
                {   // DROPSHADOWFILTER
                    swf_GetRGBA(tag, 0);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU16(tag);
                    swf_GetU8(tag);
                } else if (1 == filter)
                {   // BLURFILTER
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU8(tag);
                } else if (2 == filter)
                {   // GLOWFILTER
                    swf_GetRGBA(tag, 0);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU16(tag);
                    swf_GetU8(tag);
                } else if (3 == filter)
                {   // BEVELFILTER
                    swf_GetRGBA(tag, 0);
                    swf_GetRGBA(tag, 0);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU16(tag);
                    swf_GetU8(tag);
                } else if (4 == filter)
                {   // GRADIENTGLOWFILTER
                    int num_colors = swf_GetU8(tag);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetRGBA(tag, 0);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetU8(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU16(tag);
                    swf_GetU8(tag);
                    num_colors = swf_GetU8(tag);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetRGBA(tag, 0);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetU8(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU16(tag);
                    swf_GetU8(tag);
                } else if (5 == filter)
                {   // CONVOLUTIONFILTER
                    int mat_x = swf_GetU8(tag);
                    int mat_y = swf_GetU8(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    for (int y = 0; y < mat_y; y++)
                        for (int x = 0; x < mat_x; x++)
                            swf_GetU32(tag);
                    swf_GetRGBA(tag, 0);
                    swf_GetU8(tag);
                } else if (6 == filter)
                {   // COLORMATRIXFILTER
                    for (int j = 0; j < 20; j++)
                        swf_GetU32(tag);
                } else if (7 == filter)
                {   // GRADIENTBEVELFILTER
                    int num_colors = swf_GetU8(tag);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetRGBA(tag, 0);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetU8(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU16(tag);
                    swf_GetU8(tag);
                    num_colors = swf_GetU8(tag);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetRGBA(tag, 0);
                    for (int j = 0; j < num_colors; j++)
                        swf_GetU8(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU32(tag);
                    swf_GetU16(tag);
                    swf_GetU8(tag);
                } else
                {
                    assert(0);
                }
            }
        }
        if (flags2 & PF2_BLENDMODE)
            obj->blendmode = swf_GetU8(tag);
        if (flags2 & PF2_ASBITMAP)
        {
            /*int bcache_enabled = */swf_GetU8(tag);
            swf_GetRGBA(tag, 0);
        }
        if (flags & PF_ACTIONEVENT)
        {
#ifndef NDEBUG
            int reserved =
#endif
                    swf_GetU16(tag); // unused
            assert(!reserved);
            U32 allevent_flags = swf_GetU16(tag);
            if (version >= 6)
                allevent_flags |= swf_GetU16(tag) << 16;
            while (1)
            {
                U32 event_flags = swf_GetU16(tag);
                if (version >= 6)
                    event_flags |= swf_GetU16(tag) << 16;
                if (!event_flags)
                    break;
                int actions_size = swf_GetU32(tag);
                if (event_flags & (2 << 16))
                {
                    /*int key_code = */swf_GetU8(tag);
                }
                for (int i = 0; i < 32; i++)
                    if (event_flags & (1 << i))
                    {
                        assert(i <= 18);
                        obj->actions[i] = realloc(obj->actions[i], 4 + actions_size);
                        *(int*)(obj->actions[i]) = actions_size;
                        memcpy(obj->actions[i] + 4, tag->data + tag->pos, actions_size);
                        assert(0 == *(obj->actions[i] + 4 + actions_size - 1));
                    }
                for (int i = 0; i < actions_size; i++)
                    swf_GetU8(tag);
            }
        }
        return flags;
    } else
    {
#ifdef _DEBUG
        printf("rfxswf: Bad Tag: %d not a placeobject\n", tag->id);
#endif
    }
    return 0;
}

void swf_PlaceObjectFree(SWFPLACEOBJECT* obj)
{
    if (obj->name)
        free(obj->name);
    obj->name = 0;
}

