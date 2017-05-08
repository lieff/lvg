#include <assert.h>
#include <limits.h>
#include <rfxswf.h>
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"
#include "lunzip.h"
#include "nanovg.h"
#include "nanosvg.h"
#include "lvg_header.h"

extern NVGcontext *vg;

enum CHARACTER_TYPE {none_type, shape_type, image_type, sprite_type, text_type, edittext_type, font_type};
typedef struct
{
    TAG *tag;
    SRECT bbox;
    int lvg_id;
    enum CHARACTER_TYPE type;
} character_t;


int compare_placements(const void *v1, const void *v2);

static void path_addPoint(NSVGpath *p, float x, float y)
{
    p->pts[p->npts*2 + 0] = x;
    p->pts[p->npts*2 + 1] = y;
    p->npts++;
}

static void path_moveTo(NSVGpath *p, float x, float y)
{
    if (p->npts > 0)
    {
        p->pts[(p->npts - 1)*2 + 0] = x;
        p->pts[(p->npts - 1)*2 + 1] = y;
    } else
        path_addPoint(p, x, y);
}

static void path_lineTo(NSVGpath *p, float x, float y)
{
    float px,py, dx,dy;
    if (p->npts > 0)
    {
        px = p->pts[(p->npts - 1)*2 + 0];
        py = p->pts[(p->npts - 1)*2 + 1];
        dx = x - px;
        dy = y - py;
        path_addPoint(p, px + dx/3.0f, py + dy/3.0f);
        path_addPoint(p, x - dx/3.0f, y - dy/3.0f);
        path_addPoint(p, x, y);
    }
}

static void path_cubicBezTo(NSVGpath* p, float cpx1, float cpy1, float cpx2, float cpy2, float x, float y)
{
    path_addPoint(p, cpx1, cpy1);
    path_addPoint(p, cpx2, cpy2);
    path_addPoint(p, x, y);
}

static void path_quadBezTo(NSVGpath* p, float cx, float cy, float x, float y)
{
    if (p->npts > 0)
    {
        float px = p->pts[(p->npts - 1)*2 + 0];
        float py = p->pts[(p->npts - 1)*2 + 1];
        float cpx1 = px + 2.0f/3.0f*(cx - px);
        float cpy1 = py + 2.0f/3.0f*(cy - py);
        float cpx2 = x + 2.0f/3.0f*(cx - x);
        float cpy2 = y + 2.0f/3.0f*(cy - y);
        path_cubicBezTo(p, cpx1, cpy1, cpx2, cpy2, x, y);
    }
}

static inline uint32_t RGBA2U32(RGBA *c)
{
    return c->r | (c->g << 8) | (c->b << 16) | (c->a << 24);
}

typedef struct SHAPE_PARTS
{
    SHAPELINE *start;
    SHAPELINE *prev;
    int num_lines, fill_style[2], fill_style_used[2];
} SHAPE_PARTS;

static int findConnectingPart(SHAPE_PARTS *parts, int num_parts, int x, int y, int from_move, int from_start, int fill_style, int idx, int *at_start)
{
    for (int i = 0; i < num_parts; i++)
    {
        if ((from_move && moveTo != parts[i].start->type) || (!from_move && moveTo == parts[i].start->type))
            continue;
        SHAPELINE *start = parts[i].prev ? parts[i].prev : parts[i].start;
        SHAPELINE *end = parts[i].start;
        for (int j = 0; j < (parts[i].num_lines - 1); j++)
            end = end->next;
        assert(start != end);
        if (parts[i].fill_style_used[idx] || parts[i].fill_style[idx] != fill_style)
            continue;
        if ((INT_MAX == x) || (from_start && start->x == x && start->y == y) || (!from_start && end->x == x && end->y == y))
        {
            if (at_start)
                *at_start = (start->x == x && start->y == y);
            parts[i].fill_style_used[idx] = 1;
            return i;
        }
    }
    return -1;
}

static int haveIntersect(SHAPELINE *lines, SHAPELINE *point)
{
    while (lines)
    {
        if (lines != point && lines->x == point->x && lines->y == point->y)
            return 1;
        lines = lines->next;
    }
    return 0;
}

static void parseShape(character_t *idtable, LVGMovieClip *clip, NSVGshape *shape, SHAPE_PARTS *parts, int num_parts, SHAPE_PARTS *part, FILLSTYLE *fills, LINESTYLE *lineStyles, int idx)
{
    assert(part->num_lines > 0);
    assert(!part->prev || part->prev->next == part->start);
    assert(part->prev || moveTo == part->start->type);
    int fillStyle = part->fill_style[idx];
    int lineStyle = part->start->linestyle;
    part->fill_style_used[idx] = 1;
    shape->flags |= NSVG_FLAGS_VISIBLE;
    shape->fillRule = NSVG_FILLRULE_EVENODD;
    if (fillStyle)
    {
        FILLSTYLE *f = fills + fillStyle - 1;
        if (FILL_SOLID == f->type)
        {
            shape->fill.type = NSVG_PAINT_COLOR;
            shape->fill.color = RGBA2U32(&f->color);
        } else if (f->type == FILL_TILED || f->type == FILL_CLIPPED || f->type == (FILL_TILED|2) || f->type == (FILL_CLIPPED|2))
        {
            shape->fill.type = NSVG_PAINT_IMAGE;
            if (f->id_bitmap != 65535)
                shape->fill.color = clip->images[idtable[f->id_bitmap].lvg_id];
        } else if (FILL_LINEAR == f->type || FILL_RADIAL == f->type)
        {
            assert(f->gradient.num >= 2);
            shape->fill.type = (FILL_LINEAR == f->type) ? NSVG_PAINT_LINEAR_GRADIENT : NSVG_PAINT_RADIAL_GRADIENT;
            shape->fill.gradient = (NSVGgradient*)calloc(1, sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*(f->gradient.num - 1));
            shape->fill.gradient->nstops = f->gradient.num;
            for (int i = 0; i < f->gradient.num; i++)
            {
                shape->fill.gradient->stops[i].color = RGBA2U32(&f->gradient.rgba[i]);
                shape->fill.gradient->stops[i].offset = f->gradient.ratios[i]/255.0f;
            }
        }
    }
    if (lineStyle)
    {
        lineStyles += lineStyle - 1;
        shape->stroke.type = NSVG_PAINT_COLOR;
        shape->stroke.color = RGBA2U32(&lineStyles->color);
        shape->strokeWidth = lineStyles->width/20.0f;
    }
    shape->paths = (NSVGpath*)calloc(1, sizeof(NSVGpath));
    NSVGpath *path = shape->paths;
add_shape:
    path->closed = shape->fill.type != NSVG_PAINT_NONE;
    int alloc_pts = 1 + ((moveTo == part->start->type) ? 3*(part->num_lines - 1) : 3*part->num_lines);
    path->pts = (float*)malloc(sizeof(path->pts[0])*alloc_pts*2);
    path->npts = 0;

    SHAPELINE *lines = part->start;
    int x, y, num_lines = part->num_lines;
    if (moveTo == part->start->type)
    {
        x = part->start->x, y = part->start->y;
        lines = lines->next;
        num_lines--;
    } else
    {
        x = part->prev->x, y = part->prev->y;
    }
    path_moveTo(path, x/20.0f, y/20.0f);
    for (int i = 0; i < num_lines; i++)
    {
        assert(moveTo != lines->type);
        if (lineTo == lines->type)
            path_lineTo(path, lines->x/20.0f, lines->y/20.0f);
        else if (splineTo == lines->type)
            path_quadBezTo(path, lines->sx/20.0f, lines->sy/20.0f, lines->x/20.0f, lines->y/20.0f);
        if (i != (num_lines - 1))
            lines = lines->next;
    }
    int end_x = lines->x, end_y = lines->y;
    assert(path->npts == alloc_pts);
    while (x != end_x || y != end_y)
    {   // not full path, try connect parts
        int at_start = 0;
        int p = findConnectingPart(parts, num_parts, end_x, end_y, 0, 1, fillStyle, idx, &at_start);
        if (p < 0)
            p = findConnectingPart(parts, num_parts, end_x, end_y, 1, 1, fillStyle, idx, &at_start);
        if (p < 0)
            p = findConnectingPart(parts, num_parts, end_x, end_y, 0, 0, fillStyle, 1 - idx, &at_start);
        if (p < 0)
            p = findConnectingPart(parts, num_parts, end_x, end_y, 1, 0, fillStyle, 1 - idx, &at_start);
        //assert(p >= 0);
        if (p < 0)
            break;
        SHAPE_PARTS *cpart = parts + p;
        //assert(moveTo != cpart->start->type);
        lines = cpart->start;
        num_lines = cpart->num_lines;
        if (moveTo == cpart->start->type)
        {
            lines = lines->next;
            num_lines--;
        }
        alloc_pts += 3*num_lines;
        path->pts = (float*)realloc(path->pts, sizeof(path->pts[0])*alloc_pts*2);
        SHAPELINE *new_lines = 0;
        if (!at_start)
        {   // connect from end of part - reverse part path
            new_lines = (SHAPELINE *)malloc(sizeof(SHAPELINE)*num_lines);
            lines = cpart->prev ? cpart->prev : cpart->start;
            for (int i = 0; i < num_lines; i++)
            {
                SHAPELINE *new_line = &new_lines[num_lines - 1 - i];
                *new_line = *lines;
                lines = lines->next;
                assert(moveTo != lines->type);
                new_line->type = lines->type;
                new_line->sx = lines->sx;
                new_line->sy = lines->sy;
            }
            for (int i = 0; i < num_lines; i++)
            {
                if (i != (num_lines - 1))
                    new_lines[i].next = &new_lines[i + 1];
                else
                    new_lines[i].next = 0;
            }
            lines = new_lines;
        }
        for (int i = 0; i < num_lines; i++)
        {
            assert(moveTo != lines->type);
            if (lineTo == lines->type)
                path_lineTo(path, lines->x/20.0f, lines->y/20.0f);
            else if (splineTo == lines->type)
                path_quadBezTo(path, lines->sx/20.0f, lines->sy/20.0f, lines->x/20.0f, lines->y/20.0f);
            if (i != (num_lines - 1))
            {
                assert(x != lines->x || y != lines->y);
                lines = lines->next;
            }
        }
        assert(path->npts == alloc_pts);
        end_x = lines->x, end_y = lines->y;
        if (new_lines)
            free(new_lines);
    }
    int p = findConnectingPart(parts, num_parts, INT_MAX, INT_MAX, 1, 0, fillStyle, idx, 0);
    if (p < 0)
    {
        idx = 1 - idx;
        p = findConnectingPart(parts, num_parts, INT_MAX, INT_MAX, 1, 0, fillStyle, idx, 0);
    }
    if (p >= 0)
    {
        part = parts + p;
        path->next = (NSVGpath*)calloc(1, sizeof(NSVGpath));
        path = path->next;
        goto add_shape;
    }
}

static void parseGroup(TAG *firstTag, character_t *idtable, LVGMovieClip *clip, LVGMovieClipGroup *group)
{
    group->num_frames = 0;
    TAG *tag = firstTag;
    while (tag)
    {
        if (tag->id == ST_SHOWFRAME)
            group->num_frames++;
        tag = tag->next;
    }

    group->frames = calloc(1, sizeof(LVGMovieClipFrame)*group->num_frames);
    group->num_frames = 0;
    tag = firstTag;
    while (tag)
    {
        if (swf_isDefiningTag(tag))
        {
            int id = swf_GetDefineID(tag);
            assert(none_type == idtable[id].type);
            assert(group == clip->groups);
            idtable[id].tag = tag;
            idtable[id].bbox = swf_GetDefineBBox(tag);

            if (swf_isShapeTag(tag))
            {
                SHAPE2 *swf_shape = (SHAPE2*)rfx_calloc(sizeof(SHAPE2));
                swf_ParseDefineShape(tag, swf_shape);

                SHAPELINE tmp_line = { 0 };
                SHAPELINE *lines = swf_shape->lines;
                SHAPELINE *startLine = lines;
                SHAPELINE *startPrevLine = 0;
                SHAPELINE *prevLine = lines;
                SHAPE_PARTS *parts = (SHAPE_PARTS*)calloc(1, sizeof(SHAPE_PARTS)*65536);
                LVGShapeCollection *shape = clip->shapes + clip->num_shapes;
                int fillStyle0 = lines->fillstyle0, fillStyle1 = lines->fillstyle1, numParts = 0, numLines = 0;
                if (lines)
                {
                    if (moveTo != lines->type)
                    {
                        tmp_line.fillstyle0 = fillStyle0;
                        tmp_line.fillstyle1 = fillStyle1;
                        tmp_line.linestyle  = lines->linestyle;
                        tmp_line.next = lines;
                        lines = &tmp_line;
                        prevLine = startPrevLine = lines;
                    } else
                        numLines++;
                    lines = lines->next;
                }
                while (lines)
                {
                    while (lines && lines->next && moveTo == lines->type && moveTo == lines->next->type)
                    {
                        prevLine->next = lines->next;
                        lines = prevLine->next;
                    }
                    if (fillStyle0 != lines->fillstyle0 || fillStyle1 != lines->fillstyle1 || moveTo == lines->type || (moveTo != prevLine->type && haveIntersect(swf_shape->lines, prevLine)))
                    {
                        assert(numLines > 1 || (moveTo != startLine->type));
                        parts[numParts].start = startLine;
                        parts[numParts].prev = (moveTo != startLine->type) ? startPrevLine : 0;
                        parts[numParts].num_lines = numLines;
                        parts[numParts].fill_style[0] = fillStyle0;
                        parts[numParts].fill_style[1] = fillStyle1;
                        fillStyle0 = lines->fillstyle0;
                        fillStyle1 = lines->fillstyle1;
                        startPrevLine = prevLine;
                        startLine = lines;
                        numLines = 0;
                        numParts++;
                    }
                    numLines++;
                    prevLine = lines;
                    lines = lines->next;
                }
                if (numLines)
                {
                    parts[numParts].start = startLine;
                    parts[numParts].prev = (moveTo != startLine->type) ? startPrevLine : 0;
                    parts[numParts].num_lines = numLines;
                    parts[numParts].fill_style[0] = fillStyle0;
                    parts[numParts].fill_style[1] = fillStyle1;
                    numParts++;
                }

                shape->shapes = (NSVGshape*)calloc(1, numParts*2*sizeof(NSVGshape));
                shape->bounds[0] = idtable[id].bbox.xmin/20.0f;
                shape->bounds[1] = idtable[id].bbox.ymin/20.0f;
                shape->bounds[2] = idtable[id].bbox.xmax/20.0f;
                shape->bounds[3] = idtable[id].bbox.ymax/20.0f;

                lines = swf_shape->lines;
                for (;;)
                {
                    int i = 0, idx = 0;
                    for (; i < numParts; i++)
                    {
                        if (moveTo == parts[i].start->type && parts[i].fill_style[0] && !parts[i].fill_style_used[0])
                            break;
                        if (moveTo == parts[i].start->type && parts[i].fill_style[1] && !parts[i].fill_style_used[1])
                        {
                            idx = 1;
                            break;
                        }
                    }
                    if (i == numParts)
                    {
                        for (i = 0; i < numParts; i++)
                        {
                            if (parts[i].fill_style[0] && !parts[i].fill_style_used[0])
                                break;
                            if (parts[i].fill_style[1] && !parts[i].fill_style_used[1])
                            {
                                idx = 1;
                                break;
                            }
                        }
                        if (i == numParts)
                            break;
                    }
                    memcpy(shape->shapes[i].bounds, shape->bounds, sizeof(shape->bounds));
                    parseShape(idtable, clip, shape->shapes + shape->num_shapes++, parts, numParts, parts + i, swf_shape->fillstyles, swf_shape->linestyles, idx);
                }
                shape->shapes = (NSVGshape*)realloc(shape->shapes, shape->num_shapes*sizeof(NSVGshape));
                free(parts);
                idtable[id].type = shape_type;
                idtable[id].lvg_id = clip->num_shapes++;
            } else if (swf_isImageTag(tag))
            {
                int width, height;
                RGBA *data = swf_ExtractImage(tag, &width, &height);
                *(clip->images + clip->num_images) = nvgCreateImageRGBA(vg, width, height, 0, (const unsigned char *)data);
                idtable[id].type = image_type;
                idtable[id].lvg_id = clip->num_images++;
                free(data);
            } else if (tag->id == ST_DEFINESPRITE)
            {
                swf_UnFoldSprite(tag);
                parseGroup(tag->next, idtable, clip, &clip->groups[clip->num_groups]);
                swf_FoldSprite(tag);
                idtable[id].type = sprite_type;
                idtable[id].lvg_id = clip->num_groups++;
            }
        } else if (tag->id == ST_SHOWFRAME)
            group->num_frames++;
        else if (tag->id == ST_END)
            break;
        tag = tag->next;
    }
}

static void parsePlacements(TAG *firstTag, character_t *idtable, LVGMovieClip *clip, LVGMovieClipGroup *group)
{
    group->num_frames = 0;
    SWFPLACEOBJECT *placements = (SWFPLACEOBJECT*)calloc(1, sizeof(SWFPLACEOBJECT)*65536);
    int i, j;
    for (i = 0; i < 65536; i++)
        swf_GetPlaceObject(0, placements + i);
    TAG *tag = firstTag;
    while (tag)
    {
        if (swf_isPlaceTag(tag))
        {
            SWFPLACEOBJECT p;
            int flags = swf_GetPlaceObject(tag, &p);
            swf_PlaceObjectFree(&p);
            SWFPLACEOBJECT *target = &placements[p.depth];
            if (!p.id)
                p.id = target->id;
            assert(p.id > 0);
            target->id = p.id;
            target->depth = p.depth;
            if (flags & PF_MATRIX) target->matrix = p.matrix;
            if (flags & PF_CXFORM) target->cxform = p.cxform;
            if (flags & PF_RATIO) target->ratio = p.ratio;
            if (flags & PF_CLIPDEPTH) target->clipdepth = p.clipdepth;
        } else if (tag->id == ST_DEFINESPRITE)
        {
            swf_UnFoldSprite(tag);
            parsePlacements(tag->next, idtable, clip, &clip->groups[clip->num_groups]);
            swf_FoldSprite(tag);
            clip->num_groups++;
        } else if (tag->id == ST_REMOVEOBJECT || tag->id == ST_REMOVEOBJECT2)
        {
            U32 oldTagPos = swf_GetTagPos(tag);
            swf_SetTagPos(tag, 0);
            int id;
            if (ST_REMOVEOBJECT == tag->id)
                id = swf_GetU16(tag);
            int depth = swf_GetU16(tag);
            if (ST_REMOVEOBJECT == tag->id)
            {
                assert(placements[depth].id == id);
            }
            swf_GetPlaceObject(0, placements + depth);
            swf_SetTagPos(tag, oldTagPos);
        } else if (tag->id == ST_SHOWFRAME)
        {
            int numplacements = 0;
            for (i = 0; i < 65536; i++)
            {
                SWFPLACEOBJECT *p = &placements[i];
                if (!p->id)
                    continue;
                numplacements++;
            }
            group->frames[group->num_frames].num_objects = numplacements;
            group->frames[group->num_frames].objects = calloc(1, sizeof(LVGObject)*numplacements);
            for (i = 0, j = 0; i < 65536; i++)
            {
                SWFPLACEOBJECT *p = &placements[i];
                if (!p->id)
                    continue;
                MATRIX *m = &p->matrix;
                CXFORM *cx = &p->cxform;
                LVGObject *o = &group->frames[group->num_frames].objects[j++];
                character_t *c = &idtable[p->id];
                o->id = c->lvg_id;
                o->depth = p->depth;
                o->type = c->type;
                o->t[0] = m->sx/65536.0f;
                o->t[1] = m->r0/65536.0f;
                o->t[2] = m->r1/65536.0f;
                o->t[3] = m->sy/65536.0f;
                o->t[4] = m->tx/20.0f;
                o->t[5] = m->ty/20.0f;
                o->color_mul[0] = cx->r0/256.0f;
                o->color_mul[1] = cx->g0/256.0f;
                o->color_mul[2] = cx->b0/256.0f;
                o->color_mul[3] = cx->a0/256.0f;
                o->color_add[0] = cx->r1/256.0f;
                o->color_add[1] = cx->g1/256.0f;
                o->color_add[2] = cx->b1/256.0f;
                o->color_add[3] = cx->a1/256.0f;
            }
            group->num_frames++;
        } else if (tag->id == ST_END)
            break;
        tag = tag->next;
    }
    free(placements);
}

LVGMovieClip *swf_ReadObjects(SWF *swf)
{
    swf_OptimizeTagOrder(swf);
    swf_FoldAll(swf);
    swf_RemoveJPEGTables(swf);

    character_t *idtable = (character_t*)rfx_calloc(sizeof(character_t)*65536);
    LVGMovieClip *clip = calloc(1, sizeof(LVGMovieClip));
    clip->bounds[0] = swf->movieSize.xmin/20.0f;
    clip->bounds[1] = swf->movieSize.ymin/20.0f;
    clip->bounds[2] = swf->movieSize.xmax/20.0f;
    clip->bounds[3] = swf->movieSize.ymax/20.0f;
    clip->num_groups = 1;
    clip->fps = swf->frameRate/256.0;

    RGBA bg = swf_GetSWFBackgroundColor(swf);
    clip->bgColor = nvgRGBA(bg.r, bg.g, bg.b, bg.a);

    TAG *tag = swf->firstTag;
    while (tag)
    {
        if (swf_isDefiningTag(tag))
        {
            if (swf_isShapeTag(tag))
                clip->num_shapes++;
            else if(swf_isImageTag(tag))
                clip->num_images++;
            else if(tag->id == ST_DEFINESPRITE)
                clip->num_groups++;
        }
        tag = tag->next;
    }
    clip->shapes = calloc(1, sizeof(NSVGshape)*clip->num_shapes);
    clip->images = calloc(1, sizeof(int)*clip->num_images);
    clip->groups = calloc(1, sizeof(LVGMovieClipGroup)*clip->num_groups);

    clip->num_shapes = 0;
    clip->num_images = 0;
    clip->num_groups = 1;
    parseGroup(swf->firstTag, idtable, clip, clip->groups);
    clip->num_groups = 1;
    parsePlacements(swf->firstTag, idtable, clip, clip->groups);
    free(idtable);
    assert(clip->groups->num_frames == swf->frameCount);
    clip->last_time = g_time;
    return clip;
}

LVGMovieClip *lvgLoadSWF(const char *file)
{
    SWF swf;
    char *b;
    uint32_t file_size;
    if (!(b = lvgGetFileContents(file, &file_size)))
    {
        printf("error: could not open swf.\n");
        return 0;
    }

    if ((b[0] != 'F' && b[0] != 'C') || b[1] != 'W' || b[2] != 'S')
        return 0;
    uint32_t uncompressedSize = GET32(&b[4]);

    reader_t reader;
    if (b[0] == 'C')
    {
        char *u_data = malloc(uncompressedSize + 8);
        memcpy(u_data, b, 8);
        u_data[0] = 'F';
        stbi_zlib_decode_buffer(u_data + 8, uncompressedSize - 8, b + 8, file_size - 8);
        free(b);
        b = u_data;
        file_size = uncompressedSize;
    }
    reader_init_memreader(&reader, (void*)b, file_size);
    if (swf_ReadSWF2(&reader, &swf) < 0)
    {
        printf("error: could not open swf.\n");
        free(b);
        return 0;
    }
    free(b);
    LVGMovieClip *clip = swf_ReadObjects(&swf);
/*#ifdef DEBUG
    RENDERBUF buf;
    swf_Render_Init(&buf, 0,0, (swf.movieSize.xmax - swf.movieSize.xmin)/20, (swf.movieSize.ymax - swf.movieSize.ymin)/20, 2, 1);
    swf_RenderSWF(&buf, &swf);
    RGBA *img = swf_Render(&buf);
    //stbi_write_png("svg.png", buf.width, buf.height, 4, img, buf.width*4);
    swf_Render_Delete(&buf);
    clip->num_images++;
    clip->images = realloc(clip->images, clip->num_images*sizeof(int));
    clip->images[clip->num_images - 1] = nvgCreateImageRGBA(vg, buf.width, buf.height, 0, (const unsigned char *)img);
#endif*/
    return clip;
}
