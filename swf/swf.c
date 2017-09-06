#include <assert.h>
#include <limits.h>
#include <rfxswf.h>
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"
#include "lunzip.h"
#include "render/render.h"
#include "minimp3.h"
#include "adpcm.h"
#include "avm1.h"

extern render *g_render;
extern void *g_render_obj;

enum CHARACTER_TYPE {none_type, shape_type, image_type, video_type, sprite_type, button_type, sound_type, text_type, edittext_type, font_type};
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
    SHAPELINE *end;
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
        SHAPELINE *end = parts[i].end;
#ifdef DEBUG
        end = parts[i].start;
        for (int j = 0; j < (parts[i].num_lines - 1); j++)
            end = end->next;
        assert(end == parts[i].end);
#endif
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

static void expandBBox(float *bounds, float x, float y)
{
    if (bounds[0] > x)
        bounds[0] = x;
    if (bounds[1] > y)
        bounds[1] = y;
    if (bounds[2] < x)
        bounds[2] = x;
    if (bounds[3] < y)
        bounds[3] = y;
}

int compareStops(const void *a, const void *b)
{
    NSVGgradientStop *sa = (NSVGgradientStop *)a, *sb = (NSVGgradientStop *)b;
    if (sa->offset < sb->offset)
        return -1;
    if (sa->offset > sb->offset)
        return 1;
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
            shape->fill.spread = ((f->type & ~2) == FILL_CLIPPED) ? NSVG_SPREAD_PAD : NSVG_SPREAD_REPEAT;
            shape->fill.filtered = (f->type & 2) ? 0 : 1;
            if (f->id_bitmap != 65535)
                shape->fill.color = clip->images[idtable[f->id_bitmap].lvg_id];
            float *xf = shape->fill.xform;
            MATRIX *m = &f->m;
            xf[0] = m->sx/65536.0f;
            xf[1] = m->r0/65536.0f;
            xf[2] = m->r1/65536.0f;
            xf[3] = m->sy/65536.0f;
            xf[4] = m->tx/20.0f;
            xf[5] = m->ty/20.0f;
        } else if (FILL_LINEAR == f->type || FILL_RADIAL == f->type)
        {
            assert(f->gradient.num >= 2);
            assert(f->gradient.num < 16);
            shape->fill.type = (FILL_LINEAR == f->type) ? NSVG_PAINT_LINEAR_GRADIENT : NSVG_PAINT_RADIAL_GRADIENT;
            shape->fill.gradient = (NSVGgradient*)calloc(1, sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*(f->gradient.num - 1));
            shape->fill.gradient->nstops = f->gradient.num;
            for (int i = 0; i < f->gradient.num; i++)
            {
                shape->fill.gradient->stops[i].color = RGBA2U32(&f->gradient.rgba[i]);
                shape->fill.gradient->stops[i].offset = f->gradient.ratios[i]/255.0f;
            }
            qsort(shape->fill.gradient->stops, shape->fill.gradient->nstops, sizeof(shape->fill.gradient->stops[0]), compareStops);
            float *xf = shape->fill.gradient->xform;
            MATRIX *m = &f->m;
            xf[0] = m->sx/65536.0f;
            xf[1] = m->r0/65536.0f;
            xf[2] = m->r1/65536.0f;
            xf[3] = m->sy/65536.0f;
            xf[4] = m->tx/20.0f;
            xf[5] = m->ty/20.0f;
            g_render->cache_gradient(&shape->fill);
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
    int x, y, /*prev_x = INT_MAX, prev_y = INT_MAX, */num_lines = part->num_lines;
    if (moveTo == part->start->type)
    {
        x = part->start->x, y = part->start->y;
        lines = lines->next;
        num_lines--;
    } else
    {
        x = part->prev->x, y = part->prev->y;
    }
    expandBBox(shape->bounds, x/20.0f, y/20.0f);
    path_moveTo(path, x/20.0f, y/20.0f);
    for (int i = 0; i < num_lines; i++)
    {
        assert(moveTo != lines->type);
        expandBBox(shape->bounds, lines->x/20.0f, lines->y/20.0f);
        if (lineTo == lines->type)
            path_lineTo(path, lines->x/20.0f, lines->y/20.0f);
        else if (splineTo == lines->type)
            path_quadBezTo(path, lines->sx/20.0f, lines->sy/20.0f, lines->x/20.0f, lines->y/20.0f);
        if (i != (num_lines - 1))
        {
            assert(x != lines->x || y != lines->y);
            //prev_x = lines->x, prev_y = lines->y;
            lines = lines->next;
        }
    }
    int end_x = lines->x, end_y = lines->y;
    assert(path->npts == alloc_pts);
    while (x != end_x || y != end_y)
    {   // not full path, try connect parts
        int at_start = 0, search_idx = idx;
        int p = findConnectingPart(parts, num_parts, end_x, end_y, 0, 1, fillStyle, search_idx, &at_start);
        if (p < 0)
            p = findConnectingPart(parts, num_parts, end_x, end_y, 1, 1, fillStyle, search_idx, &at_start);
        if (p < 0)
        {
            search_idx = 1 - idx;
            p = findConnectingPart(parts, num_parts, end_x, end_y, 0, 0, fillStyle, search_idx, &at_start);
        }
        if (p < 0)
            p = findConnectingPart(parts, num_parts, end_x, end_y, 1, 0, fillStyle, search_idx, &at_start);
        //assert(p >= 0);
        if (p < 0)
            break;
        SHAPE_PARTS *cpart = parts + p;
        /*SHAPELINE *start = cpart->prev ? cpart->prev : cpart->start;
        if (at_start)
        {
            assert(start->x == end_x && start->y == end_y);
            start = start->next;
            if (start->x == prev_x && start->y == prev_y)
            {
                //assert(0);
                cpart->fill_style_used[search_idx] = 0;
                break;
            }
        } else
        {
            assert(cpart->end->x == end_x && cpart->end->y == end_y);
            //SHAPELINE *end = cpart->end;
            SHAPELINE *end = start;
            int to = cpart->prev ? (cpart->num_lines - 1) : (cpart->num_lines - 2);
            for (int j = 0; j < to; j++)
                end = end->next;
            if (end->x == prev_x && end->y == prev_y)
            {
                //assert(0);
                cpart->fill_style_used[search_idx] = 0;
                break;
            }
        }*/
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
            expandBBox(shape->bounds, lines->x/20.0f, lines->y/20.0f);
            if (lineTo == lines->type)
                path_lineTo(path, lines->x/20.0f, lines->y/20.0f);
            else if (splineTo == lines->type)
                path_quadBezTo(path, lines->sx/20.0f, lines->sy/20.0f, lines->x/20.0f, lines->y/20.0f);
            if (i != (num_lines - 1))
            {
                assert(x != lines->x || y != lines->y);
                //prev_x = lines->x, prev_y = lines->y;
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
    g_render->cache_shape(g_render_obj, shape);
}

static void actions_to_lvg(ActionTAG *actions, LVGAction *a)
{
    ActionTAG *atag = actions;
    while (atag)
    {
        a->data = 0;
        a->len  = 0;
        a->opcode = atag->op;
        if (ACTION_GOTO_FRAME == atag->op)
            a->sdata = *(uint16_t*)atag->data;
        else if (atag->len)
        {
            a->data = malloc(atag->len);
            a->len  = atag->len;
            memcpy((void*)a->data, atag->data, atag->len);
        }
        a++;
        atag = atag->next;
    }
}

static void add_playsound_action(LVGMovieClipGroup *group, int frame_num, int sound_id)
{
    assert(frame_num >= 0);
    LVGMovieClipFrame *frame = group->frames + frame_num;
    frame->actions = realloc(frame->actions, (frame->num_actions + 1)*sizeof(LVGAction));
    LVGAction *a = frame->actions + frame->num_actions++;
    memset(a, 0, sizeof(LVGAction));
    a->opcode = ACTION_STOP_SOUNDS; // our extension: play sound if sdata != 0 with id = sdata - 1
    a->sdata = sound_id + 1;
}

static void parseGroup(TAG *firstTag, character_t *idtable, LVGMovieClip *clip, LVGMovieClipGroup *group)
{
    static const int rates[4] = { 5500, 11025, 22050, 44100 };
    int stream_sound = -1, stream_buf_size = 0, stream_samples = 0, stream_format = 0, stream_bits = 0, stream_channels = 0, stream_rate = 0, stream_frame = -1;
    char *stream_buffer = 0;
    group->num_frames = 0;
    TAG *tag = firstTag;
    while (tag)
    {
        if (ST_END == tag->id)
            break;
        if (ST_SHOWFRAME == tag->id)
            group->num_frames++;
        tag = tag->next;
    }
    if (!group->num_frames) // no SHOWFRAME tag at end of the sprite
        group->num_frames++;
    group->frames = calloc(1, sizeof(LVGMovieClipFrame)*group->num_frames);

    int nframe = 0;
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
                SHAPE2 *swf_shape = (SHAPE2*)calloc(1, sizeof(SHAPE2));
                swf_ParseDefineShape(tag, swf_shape);
                if (!swf_shape->lines)
                {   // empty shape
                    tag = tag->next;
                    continue;
                }

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
                        SHAPELINE *l = lines->next;
                        free(prevLine->next);
                        prevLine->next = l;
                        lines = prevLine->next;
                    }
                    if (!lines->next && moveTo == lines->type)
                        break;
                    if (fillStyle0 != lines->fillstyle0 || fillStyle1 != lines->fillstyle1 || moveTo == lines->type || (moveTo != prevLine->type && haveIntersect(swf_shape->lines, prevLine)))
                    {
                        assert(numLines > 1 || (moveTo != startLine->type));
                        parts[numParts].start = startLine;
                        parts[numParts].end = prevLine;
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
                    parts[numParts].end = prevLine;
                    parts[numParts].prev = (moveTo != startLine->type) ? startPrevLine : 0;
                    parts[numParts].num_lines = numLines;
                    parts[numParts].fill_style[0] = fillStyle0;
                    parts[numParts].fill_style[1] = fillStyle1;
                    numParts++;
                }

                shape->shapes = (NSVGshape*)calloc(1, numParts*2*sizeof(NSVGshape));
                shape->bounds[2] = idtable[id].bbox.xmin/20.0f;
                shape->bounds[3] = idtable[id].bbox.ymin/20.0f;
                shape->bounds[0] = idtable[id].bbox.xmax/20.0f;
                shape->bounds[1] = idtable[id].bbox.ymax/20.0f;

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
                    memcpy(shape->shapes[shape->num_shapes].bounds, shape->bounds, sizeof(shape->bounds));
                    parseShape(idtable, clip, shape->shapes + shape->num_shapes++, parts, numParts, parts + i, swf_shape->fillstyles, swf_shape->linestyles, idx);
                }
                shape->shapes = (NSVGshape*)realloc(shape->shapes, shape->num_shapes*sizeof(NSVGshape));
                free(parts);
                swf_Shape2Free(swf_shape);
                free(swf_shape);
                idtable[id].type = shape_type;
                idtable[id].lvg_id = clip->num_shapes++;
            } else if (swf_isImageTag(tag))
            {
                int width, height;
                RGBA *data = swf_ExtractImage(tag, &width, &height);
                *(clip->images + clip->num_images) = g_render->cache_image(g_render_obj, width, height, 0, (const unsigned char *)data);
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
            } else if (ST_DEFINESOUND == tag->id)
            {
                LVGSound *sound = clip->sounds + clip->num_sounds;
                U32 oldTagPos = swf_GetTagPos(tag);
                swf_SetTagPos(tag, 0);
                id = swf_GetU16(tag);
                int format = swf_GetBits(tag, 4);
                sound->rate = rates[swf_GetBits(tag, 2)];
                /*int bits = */swf_GetBits(tag, 1);
                int stereo = swf_GetBits(tag, 1);
                int num_samples = swf_GetU32(tag);
                sound->channels = stereo ? 2 : 1;
                sound->samples = (short*)malloc(num_samples*2*sound->channels);
                assert(1 == format || 2 == format); // adpcm, mp3
                char *buf = (char *)&tag->data[tag->pos];
                int buf_size = tag->len - tag->pos;
                if (1 == format)
                {
                    int dec_samples = adpcm_decode(tag, buf_size, stereo + 1, sound->samples, num_samples);
                    assert(dec_samples == num_samples*sound->channels);
                    sound->num_samples = dec_samples;
                } else
                if (2 == format)
                {
                    mp3_info_t info;
                    mp3_decoder_t dec = mp3_create();
                    while (buf_size && sound->num_samples < num_samples)
                    {
                        short frame_buf[MP3_MAX_SAMPLES_PER_FRAME];
                        int frame_size = mp3_decode(dec, buf, buf_size, frame_buf, &info);
                        assert(frame_size && info.audio_bytes > 0);
                        if (!frame_size || info.audio_bytes <= 0)
                            break;
                        assert(info.channels == sound->channels);
                        assert(info.sample_rate == sound->rate);
                        int samples = info.audio_bytes/(info.channels*2);
                        if (num_samples < (sound->num_samples + samples))
                        {
                            num_samples = sound->num_samples + samples;
                            sound->samples = (short*)realloc(sound->samples, num_samples*2*info.channels);
                        }
                        memcpy(sound->samples + sound->num_samples*info.channels, frame_buf, info.audio_bytes);
                        buf += frame_size;
                        buf_size -= frame_size;
                        sound->num_samples += samples;
                    }
                    mp3_done(dec);
                    assert(num_samples == sound->num_samples);
                }
                swf_SetTagPos(tag, oldTagPos);
                idtable[id].type = sound_type;
                idtable[id].lvg_id = clip->num_sounds++;
            } else if (ST_DEFINEVIDEOSTREAM == tag->id)
            {
                U32 oldTagPos = swf_GetTagPos(tag);
                swf_SetTagPos(tag, 0);
                id = swf_GetU16(tag);
                idtable[id].type = video_type;
                idtable[id].lvg_id = clip->num_videos++;
                clip->videos = realloc(clip->videos, clip->num_videos*sizeof(LVGVideo));
                LVGVideo *video = clip->videos + idtable[id].lvg_id;
                video->num_frames = swf_GetU16(tag);
                video->width  = swf_GetU16(tag);
                video->height = swf_GetU16(tag);
                /*int reserved  = */swf_GetBits(tag, 4);
                /*int deblock   = */swf_GetBits(tag, 3);
                /*int smooth    = */swf_GetBits(tag, 1);
                video->codec  = swf_GetU8(tag) - 2;
                assert(video->codec >= 0 && video->codec <= 5);
                video->frames = calloc(1, video->num_frames*sizeof(LVGVideoFrame));
                video->image = g_render->cache_image(g_render_obj, video->width, video->height, 0, 0);
                video->cur_frame = -1;
                swf_SetTagPos(tag, oldTagPos);
            } else if (ST_DEFINEBUTTON == tag->id)
            {
                printf("button actions:\n");
                U32 oldTagPos = swf_GetTagPos(tag);
                id = swf_GetU16(tag);
                int state;
                while ((state = swf_GetU8(tag)))
                {
                    /*int cid   = */swf_GetU16(tag);
                    /*int depth = */swf_GetU16(tag);
                    MATRIX m;
                    CXFORM cm;
                    swf_GetMatrix(tag, &m);
                    swf_GetCXForm(tag, &cm, 1);
                }
                ActionTAG *actions = swf_ActionGet(tag);
                swf_DumpActions(actions, 0);
                swf_ActionFree(actions);
                swf_SetTagPos(tag, oldTagPos);
            } else if (ST_DEFINEBUTTON2 == tag->id)
            {
                U32 oldTagPos = swf_GetTagPos(tag);
                id = swf_GetU16(tag);
                printf("button2(%d) actions:\n", id);
                idtable[id].type = button_type;
                idtable[id].lvg_id = clip->num_buttons;
                clip->buttons = realloc(clip->buttons, (clip->num_buttons + 1)*sizeof(LVGButton));
                LVGButton *b = clip->buttons + clip->num_buttons++;
                memset(b, 0, sizeof(LVGButton));
                /*int flags = */swf_GetU8(tag);  // flags: 0 = track as normal button; 1 = track as menu button

                U32 offsetpos = swf_GetTagPos(tag);  // first offset
                swf_GetU16(tag);

                int state;
                while ((state = swf_GetU8(tag)))
                {
                    int cid   = swf_GetU16(tag);
                    int depth = swf_GetU16(tag);
                    printf("  state(0x%x) id=%d depth=%d\n", state, cid, depth);
                    LVGObject *o = 0;
                    assert(state < 16);
                    if (state & 15)
                    {
                        if (state & 1)
                        {
                            b->up_shapes = realloc(b->up_shapes, (b->num_up_shapes + 1)*sizeof(LVGObject));
                            o = b->up_shapes + b->num_up_shapes++;
                        }
                        if (state & 2)
                        {
                            b->over_shapes = realloc(b->over_shapes, (b->num_over_shapes + 1)*sizeof(LVGObject));
                            o = b->over_shapes + b->num_over_shapes++;
                        }
                        if (state & 4)
                        {
                            b->down_shapes = realloc(b->down_shapes, (b->num_down_shapes + 1)*sizeof(LVGObject));
                            o = b->down_shapes + b->num_down_shapes++;
                        }
                        if (state & 8)
                        {
                            b->hit_shapes = realloc(b->hit_shapes, (b->num_hit_shapes + 1)*sizeof(LVGObject));
                            o = b->hit_shapes + b->num_hit_shapes++;
                        }
                        memset(o, 0, sizeof(LVGObject));
                    }
                    MATRIX m;
                    CXFORM cx;
                    swf_GetMatrix(tag, &m);
                    swf_GetCXForm(tag, &cx, 1);
                    if (o)
                    {
                        o->id    = idtable[cid].lvg_id;
                        o->type  = idtable[cid].type;
                        o->depth = depth;
                        o->t[0] = m.sx/65536.0f;
                        o->t[1] = m.r0/65536.0f;
                        o->t[2] = m.r1/65536.0f;
                        o->t[3] = m.sy/65536.0f;
                        o->t[4] = m.tx/20.0f;
                        o->t[5] = m.ty/20.0f;
                        o->color_mul[0] = cx.r0/256.0f;
                        o->color_mul[1] = cx.g0/256.0f;
                        o->color_mul[2] = cx.b0/256.0f;
                        o->color_mul[3] = cx.a0/256.0f;
                        o->color_add[0] = cx.r1/256.0f;
                        o->color_add[1] = cx.g1/256.0f;
                        o->color_add[2] = cx.b1/256.0f;
                        o->color_add[3] = cx.a1/256.0f;
                    }
                }
                while (offsetpos)
                {
                    if (tag->pos >= tag->len)
                        break;
                    offsetpos = swf_GetU16(tag);
                    U32 condition = swf_GetU16(tag);                // condition
                    ActionTAG *actions = swf_ActionGet(tag);
                    ActionTAG *atag = actions;
                    printf("  condition %04x\n", condition);
                    int nactions = 0;
                    while (atag)
                    {
                        nactions++;
                        atag = atag->next;
                    }
                    b->actions = realloc(b->actions, (b->num_actions + nactions)*sizeof(LVGAction));
                    actions_to_lvg(actions, b->actions + b->num_actions);
                    b->num_actions += nactions;
                    swf_DumpActions(actions, "  ");
                    swf_ActionFree(actions);
                }
                swf_SetTagPos(tag, oldTagPos);
            }
        } else if (ST_SOUNDSTREAMHEAD == tag->id || ST_SOUNDSTREAMHEAD2 == tag->id)
        {
            U32 oldTagPos = swf_GetTagPos(tag);
            swf_SetTagPos(tag, 0);
            /*int reserve = */swf_GetBits(tag, 4);
            /*stream_rate = rates[*/swf_GetBits(tag, 2)/*]*/;
            /*stream_bits = */swf_GetBits(tag, 1);
            /*stream_channels = */swf_GetBits(tag, 1) ? 2 : 1;
            stream_format = swf_GetBits(tag, 4);
            stream_rate = rates[swf_GetBits(tag, 2)];
            stream_bits = swf_GetBits(tag, 1);
            stream_channels = swf_GetBits(tag, 1) ? 2 : 1;
            /*int avg_samples = */swf_GetU16(tag);
            //short latency_seek = 0;
            if (2 == stream_format)
                /*latency_seek = */swf_GetU16(tag);
            assert(0 == stream_format || 1 == stream_format || 2 == stream_format); // pcm, adpcm, mp3
            swf_SetTagPos(tag, oldTagPos);
        } else if (ST_SOUNDSTREAMBLOCK == tag->id)
        {
            U32 oldTagPos = swf_GetTagPos(tag);
            swf_SetTagPos(tag, 0);
            if (2 == stream_format)
            {
                int samples = swf_GetU16(tag);
                /*int seek_samples = */swf_GetU16(tag);
                stream_samples += samples;
            }
            int old_size = stream_buf_size, size = tag->len - tag->pos;
            //assert(size > 0);
            if (stream_sound < 0)
                stream_sound = clip->num_sounds++;
            LVGSound *sound = clip->sounds + stream_sound;
            sound->channels = stream_channels;
            sound->rate = stream_rate;
            if (stream_frame < 0)
                stream_frame = nframe;

            if (1 == stream_format)
            {
                short *buf = (short*)malloc(size*16);
                int dec_samples = adpcm_decode(tag, size, stream_channels, buf, size*8);
                stream_buf_size += dec_samples*2;
                stream_buffer = (char *)realloc(stream_buffer, stream_buf_size);
                memcpy(stream_buffer + old_size, buf, dec_samples*2);
                sound->num_samples += dec_samples;
                free(buf);
            } else
            {
                stream_buf_size += size;
                stream_buffer = (char *)realloc(stream_buffer, stream_buf_size);
                memcpy(stream_buffer + old_size, &tag->data[tag->pos], size);
            }
            swf_SetTagPos(tag, oldTagPos);
        } else if (ST_VIDEOFRAME == tag->id)
        {
            U32 oldTagPos = swf_GetTagPos(tag);
            swf_SetTagPos(tag, 0);
            int vid = swf_GetU16(tag);
            LVGVideo *video = clip->videos + idtable[vid].lvg_id;
            int frame_num = swf_GetU16(tag);
            assert(frame_num < video->num_frames);
            if (frame_num >= video->num_frames)
            {
                video->frames = realloc(video->frames, (frame_num + 1)*sizeof(LVGVideoFrame));
                memset(video->frames + video->num_frames, 0, (frame_num + 1 - video->num_frames)*sizeof(LVGVideoFrame));
                video->num_frames = frame_num + 1;
            }
            LVGVideoFrame *frame = video->frames + frame_num;
            frame->len = tag->len - tag->pos;
            frame->data = malloc(frame->len);
            memcpy(frame->data, tag->data + tag->pos, frame->len);
            swf_SetTagPos(tag, oldTagPos);
        } else if (ST_DOACTION == tag->id)
        {
            LVGMovieClipFrame *frame = group->frames + nframe;
            ActionTAG *actions = swf_ActionGet(tag);
            ActionTAG *atag = actions;
            int nactions = 0;
            while (atag)
            {
                nactions++;
                atag = atag->next;
            }
            frame->actions = realloc(frame->actions, (frame->num_actions + nactions)*sizeof(LVGAction));
            actions_to_lvg(actions, frame->actions + frame->num_actions);
            frame->num_actions += nactions;
            printf("frame %d actions:\n", nframe);
            swf_DumpActions(actions, 0);
            swf_ActionFree(actions);
        } else if (ST_DOABC == tag->id)
        {
            void*abccode = swf_ReadABC(tag);
            swf_DumpABC(stdout, abccode, "");
            swf_FreeABC(abccode);
        } else if (ST_SHOWFRAME == tag->id)
            nframe++;
        else if (ST_END == tag->id)
            break;
        tag = tag->next;
    }
    if (stream_buffer)
    {
        LVGSound *sound = clip->sounds + stream_sound;
        if (0 == stream_format || 1 == stream_format)
        {
            if (stream_bits || 1 == stream_format)
            {
                sound->samples = (short*)stream_buffer;
            } else
            {
                sound->samples = (short*)malloc(stream_buf_size*2);
                for (int i = 0; i < stream_buf_size; i++)
                    sound->samples[i] = stream_buffer[i];
            }
        } else
        if (2 == stream_format)
        {
            sound->samples = lvgLoadMP3Buf(stream_buffer, stream_buf_size, &sound->rate, &sound->channels, &sound->num_samples);
            assert(stream_samples == sound->num_samples);
        }
        if (!((0 == stream_format && stream_bits) || 1 == stream_format))
            free(stream_buffer);
        // add action to start stream sound
        add_playsound_action(group, stream_frame, stream_sound);
    }
}

static void parsePlacements(TAG *firstTag, character_t *idtable, LVGMovieClip *clip, LVGMovieClipGroup *group, int version)
{
    group->num_frames = 0;
    SWFPLACEOBJECT *placements = (SWFPLACEOBJECT*)calloc(1, sizeof(SWFPLACEOBJECT)*65536);
    int i, j;
    for (i = 0; i < 65536; i++)
    {
        swf_GetPlaceObject(0, placements + i, version);
#define INVALID_ID 65535
        placements[i].id = INVALID_ID;
    }
    TAG *tag = firstTag;
    while (tag)
    {
        if (swf_isPlaceTag(tag))
        {
            SWFPLACEOBJECT p;
            int flags = swf_GetPlaceObject(tag, &p, version);
            swf_PlaceObjectFree(&p);
            if (!(flags & PF_CHAR))
                p.id = INVALID_ID;
            SWFPLACEOBJECT *target = &placements[p.depth];
            if (INVALID_ID == p.id)
                p.id = target->id;
            assert(INVALID_ID != p.id);
            target->id = p.id;
            target->depth = p.depth;
            if (flags & PF_MATRIX) target->matrix = p.matrix;
            if (flags & PF_CXFORM) target->cxform = p.cxform;
            if (flags & PF_RATIO) target->ratio = p.ratio;
            if (flags & PF_CLIPDEPTH) target->clipdepth = p.clipdepth;
            if (p.actions)
            {
                swf_DumpActions(p.actions, 0); fflush(stdout);
                swf_ActionFree(p.actions);
            }
        } else if (ST_DEFINESPRITE == tag->id)
        {
            swf_UnFoldSprite(tag);
            parsePlacements(tag->next, idtable, clip, &clip->groups[clip->num_groups], version);
            swf_FoldSprite(tag);
            clip->num_groups++;
        } else if (ST_STARTSOUND == tag->id || ST_STARTSOUND2 == tag->id)
        {
            int id = swf_GetU16(tag);
            add_playsound_action(group, group->num_frames, idtable[id].lvg_id);
        } else if (ST_REMOVEOBJECT == tag->id || ST_REMOVEOBJECT2 == tag->id)
        {
            U32 oldTagPos = swf_GetTagPos(tag);
            swf_SetTagPos(tag, 0);
#ifndef NDEBUG
            int id;
#endif
            if (ST_REMOVEOBJECT == tag->id)
#ifndef NDEBUG
                id =
#endif
                swf_GetU16(tag);
            int depth = swf_GetU16(tag);
            if (ST_REMOVEOBJECT == tag->id)
            {
                assert(placements[depth].id == id);
            }
            swf_GetPlaceObject(0, placements + depth, version);
            swf_SetTagPos(tag, oldTagPos);
        } else if (ST_FRAMELABEL == tag->id)
        {
            group->labels = realloc(group->labels, (group->num_labels + 1)*sizeof(LVGFrameLabel));
            LVGFrameLabel *l = group->labels + group->num_labels++;
            l->name = strdup(tag->data);
            l->frame_num = group->num_frames;
        } else if (ST_SHOWFRAME == tag->id)
        {
            int numplacements;
do_show_frame:
            numplacements = 0;
            for (i = 0; i < 65536; i++)
            {
                SWFPLACEOBJECT *p = &placements[i];
                if (INVALID_ID == p->id)
                    continue;
                numplacements++;
            }
            group->frames[group->num_frames].num_objects = numplacements;
            group->frames[group->num_frames].objects = calloc(1, sizeof(LVGObject)*numplacements);
            for (i = 0, j = 0; i < 65536; i++)
            {
                SWFPLACEOBJECT *p = &placements[i];
                if (INVALID_ID == p->id || p->clipdepth)
                    continue;
                MATRIX *m = &p->matrix;
                CXFORM *cx = &p->cxform;
                LVGObject *o = &group->frames[group->num_frames].objects[j++];
                character_t *c = &idtable[p->id];
                o->id = c->lvg_id;
                o->type = c->type;
                o->depth = p->depth;
                o->ratio = p->ratio;
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
            if (ST_END == tag->id)
                break;
        } else if (ST_END == tag->id)
        {
            if (!group->num_frames) // no SHOWFRAME tag at end of the sprite
                goto do_show_frame;
            break;
        }
        tag = tag->next;
    }
    free(placements);
}

LVGMovieClip *swf_ReadObjects(SWF *swf)
{
    swf_OptimizeTagOrder(swf);
    swf_FoldAll(swf);
    swf_RemoveJPEGTables(swf);

    character_t *idtable = (character_t*)calloc(1, sizeof(character_t)*65536);
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
    int sound_stream_found = 0;
    while (tag)
    {
        if (swf_isDefiningTag(tag))
        {
            if (swf_isShapeTag(tag))
                clip->num_shapes++;
            else if (swf_isImageTag(tag))
                clip->num_images++;
            else if (ST_DEFINESPRITE == tag->id)
            {
                clip->num_groups++;
                clip->num_sounds++; // hack: reserve sound for sprite (can contain ST_SOUNDSTREAMBLOCK)
            } else if (ST_DEFINESOUND == tag->id)
                clip->num_sounds++;
        } else if ((ST_SOUNDSTREAMHEAD == tag->id || ST_SOUNDSTREAMHEAD2 == tag->id || ST_SOUNDSTREAMBLOCK == tag->id) && !sound_stream_found)
        {
            sound_stream_found = 1;
            clip->num_sounds++;
        }
        tag = tag->next;
    }
    clip->shapes = calloc(1, sizeof(NSVGshape)*clip->num_shapes);
    clip->images = calloc(1, sizeof(int)*clip->num_images);
    clip->groups = calloc(1, sizeof(LVGMovieClipGroup)*clip->num_groups);
    clip->sounds = calloc(1, sizeof(LVGSound)*clip->num_sounds);

    clip->num_shapes = 0;
    clip->num_images = 0;
    clip->num_groups = 1;
    clip->num_sounds = 0;
    parseGroup(swf->firstTag, idtable, clip, clip->groups);
    clip->num_groups = 1;
    parsePlacements(swf->firstTag, idtable, clip, clip->groups, swf->fileVersion);
    free(idtable);
    assert(clip->groups->num_frames == swf->frameCount);
    clip->last_time = g_time;
    return clip;
}

LVGMovieClip *lvgLoadSWFBuf(char *b, size_t file_size, int free_buf)
{
    SWF swf;
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
        if (free_buf)
            free(b);
        free_buf = 1;
        b = u_data;
        file_size = uncompressedSize;
    }
    reader_init_memreader(&reader, (void*)b, file_size);
    int ret = swf_ReadSWF2(&reader, &swf);
    if (free_buf)
        free(b);
    if (ret < 0)
    {
        printf("error: could not open swf.\n");
        return 0;
    }
    LVGMovieClip *clip = swf_ReadObjects(&swf);
    swf_FreeTags(&swf);
    reader.dealloc(&reader);
    return clip;
}

LVGMovieClip *lvgLoadSWF(const char *file)
{
    char *b;
    uint32_t file_size;
    if (!(b = lvgGetFileContents(file, &file_size)))
    {
        printf("error: could not open swf.\n");
        return 0;
    }
    return lvgLoadSWFBuf(b, file_size, 1);
}
