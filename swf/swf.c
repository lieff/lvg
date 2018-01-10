#include <assert.h>
#include <limits.h>
#include <rfxswf.h>
#include "stb_image.h"
#include "lunzip.h"
#include "render/render.h"
#include "audio/audio.h"
#include "audio/mp3/minimp3.h"
#include "adpcm.h"
#include "avm1.h"

extern render *g_render;
extern void *g_render_obj;

enum CHARACTER_TYPE {none_type, shape_type, image_type, video_type, sprite_type, button_type, font_type, text_type, edittext_type, sound_type};
typedef struct
{
    TAG *tag;
    SRECT bbox;
    int lvg_id, reset_frame;
    enum CHARACTER_TYPE type;
} character_t;


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

static int compareStops(const void *a, const void *b)
{
    NSVGgradientStop *sa = (NSVGgradientStop *)a, *sb = (NSVGgradientStop *)b;
    if (sa->offset < sb->offset)
        return -1;
    if (sa->offset > sb->offset)
        return 1;
    return 0;
}

static void flushStyleToShape(character_t *idtable, LVGMovieClip *clip, NSVGshape *shape, NSVGshape *shape2, FILLSTYLE *fs, LINESTYLE *ls)
{
    shape->flags |= NSVG_FLAGS_VISIBLE;
    shape->fillRule = NSVG_FILLRULE_EVENODD;
    if (fs)
    {
        if (FILL_SOLID == fs->type)
        {
            shape->fill.type = NSVG_PAINT_COLOR;
            shape->fill.color = RGBA2U32(&fs->color);
        } else if (fs->type == FILL_TILED || fs->type == FILL_CLIPPED || fs->type == (FILL_TILED|2) || fs->type == (FILL_CLIPPED|2))
        {
            shape->fill.type = NSVG_PAINT_IMAGE;
            shape->fill.spread = ((fs->type & ~2) == FILL_CLIPPED) ? NSVG_SPREAD_PAD : NSVG_SPREAD_REPEAT;
            shape->fill.filtered = (fs->type & 2) ? 0 : 1;
            if (fs->id_bitmap != 65535)
                shape->fill.color = clip->images[idtable[fs->id_bitmap].lvg_id];
            float *xf = shape->fill.xform;
            MATRIX *m = &fs->m;
            xf[0] = m->sx/65536.0f;
            xf[1] = m->r0/65536.0f;
            xf[2] = m->r1/65536.0f;
            xf[3] = m->sy/65536.0f;
            xf[4] = m->tx/20.0f;
            xf[5] = m->ty/20.0f;
        } else if (FILL_LINEAR == fs->type || FILL_RADIAL == fs->type || FILL_RADIAL_FOCAL == fs->type)
        {
            assert(fs->gradient.num >= 2);
            assert(fs->gradient.num < 16);
            shape->fill.type = (FILL_LINEAR == fs->type) ? NSVG_PAINT_LINEAR_GRADIENT : NSVG_PAINT_RADIAL_GRADIENT;
            shape->fill.gradient = (NSVGgradient*)calloc(1, sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*(fs->gradient.num - 1));
            shape->fill.gradient->nstops = fs->gradient.num;
            for (int i = 0; i < fs->gradient.num; i++)
            {
                shape->fill.gradient->stops[i].color = RGBA2U32(&fs->gradient.rgba[i]);
                shape->fill.gradient->stops[i].offset = fs->gradient.ratios[i]/255.0f;
            }
            qsort(shape->fill.gradient->stops, shape->fill.gradient->nstops, sizeof(shape->fill.gradient->stops[0]), compareStops);
            float *xf = shape->fill.gradient->xform;
            MATRIX *m = &fs->m;
            xf[0] = m->sx/65536.0f;
            xf[1] = m->r0/65536.0f;
            xf[2] = m->r1/65536.0f;
            xf[3] = m->sy/65536.0f;
            xf[4] = m->tx/20.0f;
            xf[5] = m->ty/20.0f;
            g_render->cache_gradient(g_render_obj, &shape->fill);
        }
        assert(0 == ls);
    }
    if (ls)
    {
        shape->stroke.type  = NSVG_PAINT_COLOR;
        shape->stroke.color = RGBA2U32(&ls->color);
        shape->strokeWidth  = ls->width/20.0f;
        if (shape->strokeWidth < 0.8f)
            shape->strokeWidth = 0.8f; // why thin strokes showed as ~1px?
        shape->miterLimit   = ls->mitterLimit/20.0f;
        int joinStyle = (ls->flags >> 4) & 3;
        if (0 == joinStyle)
            shape->strokeLineJoin = NSVG_JOIN_ROUND;
        else if (1 == joinStyle)
            shape->strokeLineJoin = NSVG_JOIN_BEVEL;
        else if (2 == joinStyle)
            shape->strokeLineJoin = NSVG_JOIN_MITER;
        int startLinecap = ((ls->flags >> 6) & 3);
        //int endLinecap = ((ls->flags >> 8) & 3);
        if (0 == startLinecap)
            shape->strokeLineCap = NSVG_CAP_ROUND;
        else if (1 == startLinecap)
            shape->strokeLineCap = NSVG_CAP_BUTT;
        else if (2 == startLinecap)
            shape->strokeLineCap = NSVG_CAP_SQUARE;
        //int noClose = ls->flags & 4;
        assert(0 == fs);
        fs = (FILLSTYLE*)ls;
    }
    shape->paths = (NSVGpath*)calloc(1, sizeof(NSVGpath));
    if (!shape->paths)
        return;
    if (shape2)
    {
        shape2->paths = (NSVGpath*)calloc(1, sizeof(NSVGpath));
        if (!shape2->paths)
        {
            free(shape->paths);
            return;
        }
    }
    NSVGpath *path = shape->paths, *path2 = shape2 ? shape2->paths : 0;
    int i, alloc_pts = 0, append = 0;
    int start_x = fs->subpaths[0].subpath->x;
    int start_y = fs->subpaths[0].subpath->y;
    int x = start_x, y = start_y;
    for (;;)
    {
        SUBPATH *subpath = 0;
        for (i = 0; i < fs->num_subpaths; i++)
        {
            if (fs->subpaths[i].path_used || x != fs->subpaths[i].subpath->x || y != fs->subpaths[i].subpath->y)
                continue;
            subpath = fs->subpaths + i;
            subpath->path_used = 1;
            break;
        }
        if (!subpath)
        {   // can't find path end - try start new
            if (NSVG_PAINT_NONE != shape->fill.type)
            {
                assert(0);
            }
start_new:
            for (i = 0; i < fs->num_subpaths; i++)
            {
                if (fs->subpaths[i].path_used)
                    continue;
                subpath = fs->subpaths + i;
                subpath->path_used = 1;
                break;
            }
            if (subpath)
            {
                path->next = (NSVGpath*)calloc(1, sizeof(NSVGpath));
                path = path->next;
                if (path2)
                {
                    path2->next = (NSVGpath*)calloc(1, sizeof(NSVGpath));
                    path2 = path2->next;
                }
                alloc_pts = 0;
                append = 0;
                start_x = subpath->subpath->x;
                start_y = subpath->subpath->y;
            }
        }
        if (!subpath)
            break;
        LINE *lines = subpath->subpath;
        assert(subpath->num_lines > 1);
        assert(moveTo == lines->type);
        assert(moveTo != lines[1].type);
        assert(moveTo != lines[subpath->num_lines - 1].type);

        int num_lines = subpath->num_lines - 1;
        alloc_pts += (append ? 0 : 1) + 3*num_lines;
        path->pts = (float*)realloc(path->pts, sizeof(path->pts[0])*alloc_pts*2);

        assert(num_lines);
        expandBBox(shape->bounds, lines->x/20.0f, lines->y/20.0f);
        if (!append)
            path_moveTo(path, lines->x/20.0f, lines->y/20.0f);
        lines++;
        for (i = 0; i < num_lines; i++)
        {
            assert(moveTo != lines[i].type);
            expandBBox(shape->bounds, lines[i].x/20.0f, lines[i].y/20.0f);
            if (lineTo == lines[i].type)
                path_lineTo(path, lines[i].x/20.0f, lines[i].y/20.0f);
            else if (splineTo == lines[i].type)
                path_quadBezTo(path, lines[i].sx/20.0f, lines[i].sy/20.0f, lines[i].x/20.0f, lines[i].y/20.0f);
            else { assert(0); }
        }
        assert(path->npts == alloc_pts);
        x = lines[num_lines - 1].x;
        y = lines[num_lines - 1].y;
        if (path2)
        {
            path2->pts = (float*)realloc(path2->pts, sizeof(path2->pts[0])*alloc_pts*2);
            lines = subpath->subpath2;
            assert(moveTo == lines->type);
            if (!append)
                path_moveTo(path2, lines->x/20.0f, lines->y/20.0f);
            lines++;
            for (i = 0; i < num_lines; i++)
            {
                assert(moveTo != lines[i].type);
                expandBBox(shape2->bounds, lines[i].x/20.0f, lines[i].y/20.0f);
                if (lineTo == lines[i].type)
                    path_lineTo(path2, lines[i].x/20.0f, lines[i].y/20.0f);
                else if (splineTo == lines[i].type)
                    path_quadBezTo(path2, lines[i].sx/20.0f, lines[i].sy/20.0f, lines[i].x/20.0f, lines[i].y/20.0f);
                else { assert(0); }
            }
        }
        if (start_x == x && start_y == y)
        {   // path finished - start new
            path->closed = shape->fill.type != NSVG_PAINT_NONE;
            subpath = 0;
            goto start_new;
        }
        append = 1;
    }
    g_render->cache_shape(g_render_obj, shape);
}

static void add_playsound_action(LVGMovieClipGroup *group, int frame_num, int sound_id, int flags, int start_sample, int end_sample, int loops)
{
    assert(frame_num >= 0);
    LVGMovieClipFrame *frame = group->frames + frame_num;
#define SND_CMD_SIZE 16
    uint8_t buf[SND_CMD_SIZE];
    buf[0] = ACTION_PLAY_LVG_SOUND;
    *(uint16_t*)(buf + 1) = 13;
    *(uint16_t*)(buf + 3) = sound_id;
    *(uint8_t*)(buf + 5) = flags;
    *(uint32_t*)(buf + 6) = start_sample;
    *(uint32_t*)(buf + 10) = end_sample;
    *(uint16_t*)(buf + 14) = loops;
    if (!frame->actions)
    {
        frame->actions = realloc(frame->actions, 4 + SND_CMD_SIZE);
        *(uint32_t*)frame->actions = SND_CMD_SIZE;
        memcpy(frame->actions + 4, buf, SND_CMD_SIZE);
    } else
    {
        uint32_t size = *(uint32_t*)frame->actions;
        frame->actions = realloc(frame->actions, 4 + size + SND_CMD_SIZE);
        memmove(frame->actions + 4 + SND_CMD_SIZE, frame->actions + 4, size);
        memcpy(frame->actions + 4, buf, SND_CMD_SIZE);
        *(uint32_t*)frame->actions += SND_CMD_SIZE;
    }
}

static void parse_button_record(TAG *tag, LVGButton *b, character_t *idtable)
{
    int state;
    while ((state = swf_GetU8(tag)))
    {
        int cid   = swf_GetU16(tag);
        int depth = swf_GetU16(tag);
#ifndef _TEST
        //printf("  state(0x%x) id=%d depth=%d\n", state, cid, depth);
#endif
        LVGObject *o = 0;
        assert(state < 64); // bits 5-6 unsupported ButtonHasFilterList and ButtonHasBlendMode
        if (state & 15)
        {
            b->btn_shapes = realloc(b->btn_shapes, (b->num_btn_shapes + 1)*sizeof(LVGButtonState));
            LVGButtonState *bs = b->btn_shapes + b->num_btn_shapes++;
            bs->flags = state & 15;
            o = &bs->obj;
            memset(o, 0, sizeof(LVGObject));
        }
        MATRIX m;
        CXFORM cx;
        swf_GetMatrix(tag, &m);
        swf_GetCXForm(tag, &cx, 1);
        if (state & 16)
        {   // ButtonHasFilterList
            int nfilters = swf_GetU8(tag);
            for (int i = 0; i < nfilters; i++)
            {
                FILTER *f = swf_GetFilter(tag);
                swf_DeleteFilter(f);
            }
        }
        int blendmode = 0;
        if (state & 32)
        {   // ButtonHasBlendMode
            blendmode = swf_GetU8(tag);
        }
        if (o)
        {
            o->id    = idtable[cid].lvg_id;
            o->type  = idtable[cid].type;
            o->depth = depth;
            o->blend_mode = blendmode;
            o->t[0] = m.sx/65536.0f;
            o->t[1] = m.r0/65536.0f;
            o->t[2] = m.r1/65536.0f;
            o->t[3] = m.sy/65536.0f;
            o->t[4] = m.tx/20.0f;
            o->t[5] = m.ty/20.0f;
            o->cxform.mul[0] = cx.r0/256.0f;
            o->cxform.mul[1] = cx.g0/256.0f;
            o->cxform.mul[2] = cx.b0/256.0f;
            o->cxform.mul[3] = cx.a0/256.0f;
            o->cxform.add[0] = cx.r1/256.0f;
            o->cxform.add[1] = cx.g1/256.0f;
            o->cxform.add[2] = cx.b1/256.0f;
            o->cxform.add[3] = cx.a1/256.0f;
        }
    }
}

static void parseShape(TAG *tag, character_t *idtable, LVGMovieClip *clip, SHAPE2 *swf_shape, LVGShapeCollection *shape)
{
    shape->shapes = (NSVGshape*)calloc(1, /*(swf_shape->numfillstyles + swf_shape->numlinestyles)*/65536*sizeof(NSVGshape));

    swf_ResetReadBits(tag);
    int fillbits = swf_GetBits(tag, 4);
    int linebits = swf_GetBits(tag, 4);
    // TODO: optimize
    LINE *path = (LINE*)calloc(1, sizeof(LINE)*/*nlines*/65536);
    LINE *ppath = path;
    int i, fill0 = 0, fill1 = 0, line = 0, start_x = 0, start_y = 0, x = 0, y = 0;

    while(1)
    {
        int flags = swf_GetBits(tag, 1);
        if (!flags)
        {   // style change
            flags = swf_GetBits(tag, 5);

            int subpath_size = ppath - path;
            if (subpath_size)
            {
                LINE *p = path;
                /*printf("{ %.2f, %.2f }", start_x/20.0, start_y/20.0);
                for (i = 0; i < subpath_size; i++)
                {
                    if (splineTo == p[i].type)
                        printf("{ %d %.2f, %.2f }", p[i].type, p[i].x/20.0, p[i].y/20.0);
                    else
                        printf("{ %d %.2f, %.2f }", p[i].type, p[i].x/20.0, p[i].y/20.0);
                }
                printf("\n"); fflush(stdout);*/
                if (fill0)
                {
                    FILLSTYLE *fs = &swf_shape->fillstyles[fill0 - 1];
                    fs->subpaths = realloc(fs->subpaths, (fs->num_subpaths + 1)*sizeof(SUBPATH));
                    SUBPATH *subpath = fs->subpaths + fs->num_subpaths++;
                    subpath->num_lines = subpath_size + 1;
                    subpath->path_used = 0;
                    subpath->subpath   = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath2  = 0;
                    subpath->subpath[0].type = moveTo;
                    subpath->subpath[0].x = start_x;
                    subpath->subpath[0].y = start_y;
                    subpath->subpath[0].sx = 0;
                    subpath->subpath[0].sy = 0;
                    memcpy(subpath->subpath + 1, p, subpath_size*sizeof(LINE));
                    assert(moveTo == subpath->subpath->type);
                    assert(moveTo != subpath->subpath[1].type);
                    assert(moveTo != subpath->subpath[subpath->num_lines - 1].type);
                }
                if (fill1)
                {
                    ppath = p;
                    FILLSTYLE *fs = &swf_shape->fillstyles[fill1 - 1];
                    // CCW used for normal shapes - add with reverse order
                    fs->subpaths = realloc(fs->subpaths, (fs->num_subpaths + 1)*sizeof(SUBPATH));
                    SUBPATH *subpath = fs->subpaths + fs->num_subpaths++;
                    subpath->num_lines = subpath_size + 1;
                    subpath->path_used = 0;
                    subpath->subpath   = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath2  = 0;
                    LINE *pline = subpath->subpath + subpath_size;
                    subpath->subpath[0].type = moveTo;
                    subpath->subpath[0].x = p[subpath_size - 1].x;
                    subpath->subpath[0].y = p[subpath_size - 1].y;
                    subpath->subpath[0].sx = 0;
                    subpath->subpath[0].sy = 0;
                    int _x = start_x, _y = start_y;
                    for (i = 0; i < subpath_size; i++)
                    {
                        if (lineTo == ppath->type)
                        {
                            pline[-i].x = _x;
                            pline[-i].y = _y;
                            pline[-i].sx = 0;
                            pline[-i].sy = 0;
                            pline[-i].type = ppath->type;
                        } else if (splineTo == ppath->type)
                        {
                            pline[-i].x = _x;
                            pline[-i].y = _y;
                            pline[-i].sx = ppath->sx;
                            pline[-i].sy = ppath->sy;
                            pline[-i].type = ppath->type;
                        } else
                        {
                            assert(0);
                        }
                        _x = ppath->x;
                        _y = ppath->y;
                        ppath++;
                    }
                    assert(moveTo == subpath->subpath[0].type);
                    assert(moveTo != subpath->subpath[1].type);
                    assert(moveTo != subpath->subpath[subpath_size].type);
                }
                if (line)
                {
                    LINESTYLE *ls = &swf_shape->linestyles[line - 1];
                    ls->subpaths = realloc(ls->subpaths, (ls->num_subpaths + 1)*sizeof(SUBPATH));
                    SUBPATH *subpath = ls->subpaths + ls->num_subpaths++;
                    subpath->num_lines = subpath_size + 1;
                    subpath->path_used = 0;
                    subpath->subpath   = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath2  = 0;
                    subpath->subpath[0].type = moveTo;
                    subpath->subpath[0].x = start_x;
                    subpath->subpath[0].y = start_y;
                    subpath->subpath[0].sx = 0;
                    subpath->subpath[0].sy = 0;
                    memcpy(subpath->subpath + 1, p, subpath_size*sizeof(LINE));
                    assert(moveTo == subpath->subpath->type);
                    assert(moveTo != subpath->subpath[1].type);
                    assert(moveTo != subpath->subpath[subpath->num_lines - 1].type);
                }
                ppath = path;
            }
            if (!flags || (flags & 16))
            {   // new styles or end, we must flush all shape parts here, all filled shapes must be closed
                //printf("flush\n"); fflush(stdout);
                for (i = 0; i < swf_shape->numfillstyles; i++)
                {
                    FILLSTYLE *fs = swf_shape->fillstyles + i;
                    if (!fs->num_subpaths)
                        continue;
                    memcpy(shape->shapes[shape->num_shapes].bounds, shape->bounds, sizeof(shape->bounds));
                    NSVGshape *s = shape->shapes + shape->num_shapes++;
                    flushStyleToShape(idtable, clip, s, 0, fs, 0);
                }
                for (i = 0; i < swf_shape->numlinestyles; i++)
                {
                    LINESTYLE *ls = swf_shape->linestyles + i;
                    if (!ls->num_subpaths)
                        continue;
                    memcpy(shape->shapes[shape->num_shapes].bounds, shape->bounds, sizeof(shape->bounds));
                    NSVGshape *s = shape->shapes + shape->num_shapes++;
                    flushStyleToShape(idtable, clip, s, 0, 0, ls);
                }
                swf_ShapeFreeSubpaths(swf_shape);
            }

            if (!flags)
                break;
            if (flags & 1)
            {   // move
                int n = swf_GetBits(tag, 5);
                x = swf_GetSBits(tag, n);
                y = swf_GetSBits(tag, n);
            }
            if (flags & 2)
                fill0 = swf_GetBits(tag, fillbits);
            if (flags & 4)
                fill1 = swf_GetBits(tag, fillbits);
            if (flags & 8)
                line  = swf_GetBits(tag, linebits);
            if (flags & 16)
            {
                if (swf_shape->fillstyles)
                {
                    for (i = 0; i < swf_shape->numfillstyles; i++)
                    {
                        FILLSTYLE *f = &swf_shape->fillstyles[i];
                        if (FILL_LINEAR == f->type || FILL_RADIAL == f->type)
                        {
                            free(f->gradient.rgba);
                            free(f->gradient.ratios);
                        }
                    }
                    free(swf_shape->fillstyles);
                }
                if (swf_shape->linestyles)
                    free(swf_shape->linestyles);
                swf_shape->fillstyles = 0;
                swf_shape->linestyles = 0;
                swf_shape->numlinestyles = 0;
                swf_shape->numfillstyles = 0;
                if (!parseFillStyleArray(tag, swf_shape))
                    break;
                fillbits = swf_GetBits(tag, 4);
                linebits = swf_GetBits(tag, 4);
            }
            start_x = x;
            start_y = y;
        } else
        {
            flags = swf_GetBits(tag, 1);
            if (flags)
            {   // straight edge
                int n = swf_GetBits(tag, 4) + 2;
                if (swf_GetBits(tag, 1))
                {   // line flag
                    x += swf_GetSBits(tag, n); //delta x
                    y += swf_GetSBits(tag, n); //delta y
                } else
                {
                    int v = swf_GetBits(tag, 1);
                    int d = swf_GetSBits(tag, n); //vert/horz
                    if (v)
                        y += d;
                    else
                        x += d;
                }
                ppath->sx = 0;
                ppath->sy = 0;
                ppath->type = lineTo;
            } else
            {   // curved edge
                int n = swf_GetBits(tag, 4) + 2;
                x += swf_GetSBits(tag, n);
                y += swf_GetSBits(tag, n);
                ppath->sx = x;
                ppath->sy = y;
                x += swf_GetSBits(tag, n);
                y += swf_GetSBits(tag, n);
                ppath->type = splineTo;
            }
            ppath->x = x;
            ppath->y = y;
            ppath++;
        }
    }
    shape->shapes = (NSVGshape*)realloc(shape->shapes, shape->num_shapes*sizeof(NSVGshape));
    assert(shape->num_shapes);
    free(path);
}

static void parseMorphShape(TAG *tag, character_t *idtable, LVGMovieClip *clip, SHAPE2 *swf_shape, LVGShapeCollection *shape)
{
    shape->shapes = (NSVGshape*)calloc(1, 65536*sizeof(NSVGshape));
    shape->morph = calloc(1, sizeof(LVGShapeCollection));
    shape->morph->shapes = (NSVGshape*)calloc(1, 65536*sizeof(NSVGshape));
    LVGShapeCollection *morph_shape = shape->morph;

    swf_ResetReadBits(tag);
    int fillbits = swf_GetBits(tag, 4);
    int linebits = swf_GetBits(tag, 4);
    //if (!fillbits && !linebits)
    //    return;

    TAG tag2 = *tag;
    tag2.pos = swf_shape->endEdgesOffset;
    int fillbits2 = swf_GetBits(&tag2, 4);
    int linebits2 = swf_GetBits(&tag2, 4);
    assert(!fillbits2 && !linebits2);

    LINE *path  = (LINE*)calloc(1, sizeof(LINE)*65536);
    LINE *path2 = (LINE*)calloc(1, sizeof(LINE)*65536);
    LINE *ppath = path, *ppath2 = path2;
    int i, fill0 = 0, fill1 = 0, line = 0, start_x = 0, start_y = 0, x = 0, y = 0;
    int start_x2 = 0, start_y2 = 0, x2 = 0, y2 = 0;

    while (1)
    {
        int flags = swf_GetBits(tag, 1);
        if (!flags)
        {   // style change
            flags = swf_GetBits(tag, 5);

            int subpath_size = ppath - path;
            if (subpath_size)
            {
                assert(subpath_size == (ppath2 - path2));
                if (fill0)
                {
                    FILLSTYLE *fs = &swf_shape->fillstyles[fill0 - 1];
                    fs->subpaths  = realloc(fs->subpaths, (fs->num_subpaths + 1)*sizeof(SUBPATH));
                    SUBPATH *subpath = fs->subpaths + fs->num_subpaths++;
                    subpath->num_lines = subpath_size + 1;
                    subpath->path_used = 0;
                    subpath->subpath   = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath2  = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath[0].type = moveTo;
                    subpath->subpath[0].x = start_x;
                    subpath->subpath[0].y = start_y;
                    subpath->subpath[0].sx = 0;
                    subpath->subpath[0].sy = 0;
                    subpath->subpath2[0].type = moveTo;
                    subpath->subpath2[0].x = start_x2;
                    subpath->subpath2[0].y = start_y2;
                    subpath->subpath2[0].sx = 0;
                    subpath->subpath2[0].sy = 0;
                    memcpy(subpath->subpath + 1, path, subpath_size*sizeof(LINE));
                    memcpy(subpath->subpath2 + 1, path2, subpath_size*sizeof(LINE));
                    assert(moveTo == subpath->subpath->type);
                    assert(moveTo != subpath->subpath[1].type);
                    assert(moveTo != subpath->subpath[subpath->num_lines - 1].type);
                }
                if (fill1)
                {
                    ppath  = path;
                    ppath2 = path2;
                    FILLSTYLE *fs = &swf_shape->fillstyles[fill1 - 1];
                    // CCW used for normal shapes - add with reverse order
                    fs->subpaths = realloc(fs->subpaths, (fs->num_subpaths + 1)*sizeof(SUBPATH));
                    SUBPATH *subpath = fs->subpaths + fs->num_subpaths++;
                    subpath->num_lines = subpath_size + 1;
                    subpath->path_used = 0;
                    subpath->subpath   = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath2  = malloc((subpath_size + 1)*sizeof(LINE));
                    LINE *pline  = subpath->subpath + subpath_size;
                    LINE *pline2 = subpath->subpath2 + subpath_size;
                    subpath->subpath[0].type = moveTo;
                    subpath->subpath[0].x = path[subpath_size - 1].x;
                    subpath->subpath[0].y = path[subpath_size - 1].y;
                    subpath->subpath[0].sx = 0;
                    subpath->subpath[0].sy = 0;
                    subpath->subpath2[0].type = moveTo;
                    subpath->subpath2[0].x = path2[subpath_size - 1].x;
                    subpath->subpath2[0].y = path2[subpath_size - 1].y;
                    subpath->subpath2[0].sx = 0;
                    subpath->subpath2[0].sy = 0;
                    int _x = start_x, _y = start_y, _x2 = start_x2, _y2 = start_y2;
                    for (i = 0; i < subpath_size; i++)
                    {
                        if (lineTo == ppath->type)
                        {
                            pline[-i].x = _x;
                            pline[-i].y = _y;
                            pline[-i].sx = 0;
                            pline[-i].sy = 0;
                            pline[-i].type = ppath->type;
                        } else if (splineTo == ppath->type)
                        {
                            pline[-i].x = _x;
                            pline[-i].y = _y;
                            pline[-i].sx = ppath->sx;
                            pline[-i].sy = ppath->sy;
                            pline[-i].type = ppath->type;
                        } else
                        {
                            assert(0);
                        }
                        if (lineTo == ppath2->type)
                        {
                            pline2[-i].x = _x2;
                            pline2[-i].y = _y2;
                            pline2[-i].sx = 0;
                            pline2[-i].sy = 0;
                            pline2[-i].type = ppath2->type;
                        } else if (splineTo == ppath2->type)
                        {
                            pline2[-i].x = _x2;
                            pline2[-i].y = _y2;
                            pline2[-i].sx = ppath2->sx;
                            pline2[-i].sy = ppath2->sy;
                            pline2[-i].type = ppath2->type;
                        } else
                        {
                            assert(0);
                        }
                        _x = ppath->x;
                        _y = ppath->y;
                        _x2 = ppath2->x;
                        _y2 = ppath2->y;
                        ppath++;
                        ppath2++;
                    }
                    assert(moveTo == subpath->subpath[0].type);
                    assert(moveTo != subpath->subpath[1].type);
                    assert(moveTo != subpath->subpath[subpath_size].type);
                }
                if (line)
                {
                    LINESTYLE *ls = &swf_shape->linestyles[line - 1];
                    ls->subpaths = realloc(ls->subpaths, (ls->num_subpaths + 1)*sizeof(SUBPATH));
                    SUBPATH *subpath = ls->subpaths + ls->num_subpaths++;
                    subpath->num_lines = subpath_size + 1;
                    subpath->path_used = 0;
                    subpath->subpath   = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath2  = malloc((subpath_size + 1)*sizeof(LINE));
                    subpath->subpath[0].type = moveTo;
                    subpath->subpath[0].x = start_x;
                    subpath->subpath[0].y = start_y;
                    subpath->subpath[0].sx = 0;
                    subpath->subpath[0].sy = 0;
                    subpath->subpath2[0].type = moveTo;
                    subpath->subpath2[0].x = start_x2;
                    subpath->subpath2[0].y = start_y2;
                    subpath->subpath2[0].sx = 0;
                    subpath->subpath2[0].sy = 0;
                    memcpy(subpath->subpath + 1, path, subpath_size*sizeof(LINE));
                    memcpy(subpath->subpath2 + 1, path2, subpath_size*sizeof(LINE));
                    assert(moveTo == subpath->subpath->type);
                    assert(moveTo != subpath->subpath[1].type);
                    assert(moveTo != subpath->subpath[subpath->num_lines - 1].type);
                }
                ppath  = path;
                ppath2 = path2;
            }
            if (!flags || (flags & 16))
            {   // new styles or end, we must flush all shape parts here, all filled shapes must be closed
                for (i = 0; i < swf_shape->numfillstyles; i++)
                {
                    FILLSTYLE *fs = swf_shape->fillstyles + i;
                    if (!fs->num_subpaths)
                        continue;
                    memcpy(shape->shapes[shape->num_shapes].bounds, shape->bounds, sizeof(shape->bounds));
                    flushStyleToShape(idtable, clip, shape->shapes + shape->num_shapes++, morph_shape->shapes + morph_shape->num_shapes++, fs, 0);
                }
                for (i = 0; i < swf_shape->numlinestyles; i++)
                {
                    LINESTYLE *ls = swf_shape->linestyles + i;
                    if (!ls->num_subpaths)
                        continue;
                    memcpy(shape->shapes[shape->num_shapes].bounds, shape->bounds, sizeof(shape->bounds));
                    flushStyleToShape(idtable, clip, shape->shapes + shape->num_shapes++, morph_shape->shapes + morph_shape->num_shapes++, 0, ls);
                }
                swf_ShapeFreeSubpaths(swf_shape);
            }

            if (!flags)
                break;
            if (flags & 1)
            {   // move
                int n = swf_GetBits(tag, 5);
                x = swf_GetSBits(tag, n);
                y = swf_GetSBits(tag, n);
            }
            if (flags & 2)
                fill0 = swf_GetBits(tag, fillbits);
            if (flags & 4)
                fill1 = swf_GetBits(tag, fillbits);
            if (flags & 8)
                line  = swf_GetBits(tag, linebits);
            if (flags & 16)
            {
                if (swf_shape->fillstyles)
                {
                    for (i = 0; i < swf_shape->numfillstyles; i++)
                    {
                        FILLSTYLE *f = &swf_shape->fillstyles[i];
                        if (FILL_LINEAR == f->type || FILL_RADIAL == f->type)
                        {
                            free(f->gradient.rgba);
                            free(f->gradient.ratios);
                        }
                    }
                    free(swf_shape->fillstyles);
                }
                if (swf_shape->linestyles)
                    free(swf_shape->linestyles);
                swf_shape->fillstyles = 0;
                swf_shape->linestyles = 0;
                swf_shape->numlinestyles = 0;
                swf_shape->numfillstyles = 0;
                if (!parseFillStyleArray(tag, swf_shape))
                    break;
                fillbits = swf_GetBits(tag, 4);
                linebits = swf_GetBits(tag, 4);
            }
            start_x = x;
            start_y = y;
        } else
        {
            flags = swf_GetBits(tag, 1);
            if (flags)
            {   // straight edge
                int n = swf_GetBits(tag, 4) + 2;
                if (swf_GetBits(tag, 1))
                {   // line flag
                    x += swf_GetSBits(tag, n); //delta x
                    y += swf_GetSBits(tag, n); //delta y
                } else
                {
                    int v = swf_GetBits(tag, 1);
                    int d = swf_GetSBits(tag, n); //vert/horz
                    if (v)
                        y += d;
                    else
                        x += d;
                }
                ppath->sx = 0;
                ppath->sy = 0;
                ppath->type = lineTo;
            } else
            {   // curved edge
                int n = swf_GetBits(tag, 4) + 2;
                x += swf_GetSBits(tag, n);
                y += swf_GetSBits(tag, n);
                ppath->sx = x;
                ppath->sy = y;
                x += swf_GetSBits(tag, n);
                y += swf_GetSBits(tag, n);
                ppath->type = splineTo;
            }
            ppath->x = x;
            ppath->y = y;
            ppath++;
        }
        // now parse morph part
        ptrdiff_t subpath_size = ppath - path;
        while ((ppath2 - path2) < subpath_size)
        {
            flags = swf_GetBits(&tag2, 1);
            if (!flags)
            {   // style change
                flags = swf_GetBits(&tag2, 5);

                if (flags & 1)
                {   // move
                    int n = swf_GetBits(&tag2, 5);
                    x2 = swf_GetSBits(&tag2, n);
                    y2 = swf_GetSBits(&tag2, n);
                }
                if ((flags & 2) && fillbits2)
                    swf_GetBits(&tag2, fillbits2);
                if ((flags & 4) && fillbits2)
                    swf_GetBits(&tag2, fillbits2);
                if ((flags & 8) && linebits2)
                    swf_GetBits(&tag2, linebits2);
                if (flags & 16)
                {
                    assert(0);
                }
                start_x2 = x2;
                start_y2 = y2;
            } else
            {
                flags = swf_GetBits(&tag2, 1);
                if (flags)
                {   // straight edge
                    int n = swf_GetBits(&tag2, 4) + 2;
                    if (swf_GetBits(&tag2, 1))
                    {   // line flag
                        x2 += swf_GetSBits(&tag2, n); //delta x
                        y2 += swf_GetSBits(&tag2, n); //delta y
                    } else
                    {
                        int v = swf_GetBits(&tag2, 1);
                        int d = swf_GetSBits(&tag2, n); //vert/horz
                        if (v)
                            y2 += d;
                        else
                            x2 += d;
                    }
                    ppath2->sx = 0;
                    ppath2->sy = 0;
                    ppath2->type = lineTo;
                } else
                {   // curved edge
                    int n = swf_GetBits(&tag2, 4) + 2;
                    x2 += swf_GetSBits(&tag2, n);
                    y2 += swf_GetSBits(&tag2, n);
                    ppath2->sx = x2;
                    ppath2->sy = y2;
                    x2 += swf_GetSBits(&tag2, n);
                    y2 += swf_GetSBits(&tag2, n);
                    ppath2->type = splineTo;
                }
                ppath2->x = x2;
                ppath2->y = y2;
                ppath2++;
            }
        }
    }
    shape->shapes = (NSVGshape*)realloc(shape->shapes, shape->num_shapes*sizeof(NSVGshape));
    shape->morph->shapes = (NSVGshape*)realloc(shape->morph->shapes, shape->morph->num_shapes*sizeof(NSVGshape));
    assert(shape->num_shapes);
    assert(shape->morph->num_shapes == shape->num_shapes);
    free(path);
    free(path2);
}

static TAG *skip_sprite(TAG *tag)
{
    do {
        tag = tag->next;
    } while (tag && ST_END != tag->id);
    return tag;
}

static void flush_stream_sound(LVGMovieClip *clip, LVGMovieClipGroup *group, const unsigned char *stream_buffer, int stream_buf_size, int stream_sound, int stream_format, int stream_bits, int stream_frame, int end_frame)
{
    group->ssounds = realloc(group->ssounds, (group->num_ssounds + 1)*sizeof(group->ssounds[0]));
    LVGStreamSound *ssound = group->ssounds + group->num_ssounds++;
    ssound->sound_id = stream_sound;
    ssound->start_frame = stream_frame;
    ssound->end_frame = end_frame;
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
    }
    if (!((0 == stream_format && stream_bits) || 1 == stream_format))
        free(stream_buffer);
    // add action to start stream sound
    add_playsound_action(group, stream_frame, stream_sound, 0, 0, sound->num_samples, 0);
}

static void skipActions(TAG *tag)
{
    int op;
    do {
        op = swf_GetU8(tag);
        if (op >= 0x80)
            tag->pos += swf_GetU16(tag);
    } while (op);
}

static TAG *parseGroup(TAG *firstTag, character_t *idtable, LVGMovieClip *clip, LVGMovieClipGroup *group)
{
    static const int rates[4] = { 5500, 11025, 22050, 44100 };
    int stream_sound = -1, stream_buf_size = 0, stream_samples = 0, stream_format = 0, stream_bits = 0, stream_channels = 0, stream_rate = 0, stream_frame = -1, sound_block_frame = 0;
    char *stream_buffer = 0;
    group->num_frames = 0;
    TAG *tag = firstTag;
    while (tag)
    {
        if (ST_END == tag->id)
            break;
        if (ST_DEFINESPRITE == tag->id)
        {
            if (!(tag = skip_sprite(tag)))
                goto done;
        } else
        if (ST_SHOWFRAME == tag->id)
            group->num_frames++;
        tag = tag->next;
    }
done:
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
                //printf("id=%d\n", id);
                SHAPE2 *swf_shape = (SHAPE2*)calloc(1, sizeof(SHAPE2));
                swf_ParseDefineShape(tag, swf_shape);
                LVGShapeCollection *shapecol = clip->shapes + clip->num_shapes;
                shapecol->bounds[2] = idtable[id].bbox.xmin/20.0f; // invert bbox for individual shape bbox calculation
                shapecol->bounds[3] = idtable[id].bbox.ymin/20.0f;
                shapecol->bounds[0] = idtable[id].bbox.xmax/20.0f;
                shapecol->bounds[1] = idtable[id].bbox.ymax/20.0f;
                if (ST_DEFINEMORPHSHAPE == tag->id || ST_DEFINEMORPHSHAPE2 == tag->id)
                {
                    parseMorphShape(tag, idtable, clip, swf_shape, shapecol);
                } else
                    parseShape(tag, idtable, clip, swf_shape, shapecol);
                shapecol->bounds[0] = idtable[id].bbox.xmin/20.0f;
                shapecol->bounds[1] = idtable[id].bbox.ymin/20.0f;
                shapecol->bounds[2] = idtable[id].bbox.xmax/20.0f;
                shapecol->bounds[3] = idtable[id].bbox.ymax/20.0f;

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
            } else if (ST_DEFINESPRITE == tag->id)
            {
                tag = parseGroup(tag->next, idtable, clip, &clip->groups[clip->num_groups]);
                idtable[id].type = sprite_type;
                idtable[id].lvg_id = clip->num_groups++;
            } else if (ST_DEFINEFONT == tag->id || ST_DEFINEFONT2 == tag->id || ST_DEFINEFONT3 == tag->id)
            {
                int t;
                LVGFont *font = clip->fonts + clip->num_fonts;
                font->version = (tag->id == ST_DEFINEFONT3) ? 3 : 2;
                SWFFONT *swffont = (SWFFONT *) calloc(1, sizeof(SWFFONT));
                if (ST_DEFINEFONT == tag->id)
                    swf_FontExtract_DefineFont(0, swffont, tag);
                else
                    swf_FontExtract_DefineFont2(0, swffont, tag);
                font->num_chars = swffont->numchars;
                font->glyphs = (int*)calloc(1, sizeof(font->glyphs[0])*font->num_chars);
                for (t = 0; t < font->num_chars; t++)
                {
                    static const RGBA color_white = { 255, 255, 255, 255 };
                    SHAPE *shape = swffont->glyph[t].shape;
                    if (!shape->fillstyle.n)
                        swf_ShapeAddSolidFillStyle(shape, (RGBA*)&color_white);
                    SHAPE2 *swf_shape = swf_ShapeToShape2(shape);
                    LVGShapeCollection *shapecol = clip->shapes + clip->num_shapes;
                    shapecol->bounds[0] = 1e6f;
                    shapecol->bounds[1] = 1e6f;
                    shapecol->bounds[2] = -1e6f;
                    shapecol->bounds[3] = -1e6f;
                    TAG _tag;
                    TAG* tag2 = &_tag;
                    memset(tag2, 0, sizeof(TAG));
                    tag2->data = shape->data;
                    tag2->len = tag2->memsize = (shape->bitlen + 7)/8;
                    if (tag2->len > 14)
                        parseShape(tag2, idtable, clip, swf_shape, shapecol);
                    font->glyphs[t] = clip->num_shapes++;
                    swf_Shape2Free(swf_shape);
                    free(swf_shape);
                }
                swf_FontFree(swffont);
                idtable[id].type = font_type;
                idtable[id].lvg_id = clip->num_fonts++;
            } else if (ST_DEFINETEXT == tag->id || ST_DEFINETEXT2 == tag->id)
            {
                SRECT r;
                MATRIX m;
                int fid = -1;
                RGBA color;
                int x = 0, y = 0, fontsize = 0;
                memset(&color, 0, sizeof(color));
                swf_SetTagPos(tag, 0);

#define TF_TEXTCONTROL  0x80
#define TF_HASFONT      0x08
#define TF_HASCOLOR     0x04
#define TF_HASYOFFSET   0x02
#define TF_HASXOFFSET   0x01
                /*cid = */swf_GetU16(tag);
                swf_GetRect(tag, &r);
                swf_GetMatrix(tag, &m);
                int gbits = swf_GetU8(tag);
                int abits = swf_GetU8(tag);
                idtable[id].type = text_type;
                idtable[id].lvg_id = clip->num_texts;
                clip->texts = realloc(clip->texts, (clip->num_texts + 1)*sizeof(clip->texts[0]));
                LVGText *text = clip->texts + clip->num_texts++;
                memset(text, 0, sizeof(LVGText));
                text->t[0] = m.sx/65536.0f;
                text->t[1] = m.r0/65536.0f;
                text->t[2] = m.r1/65536.0f;
                text->t[3] = m.sy/65536.0f;
                text->t[4] = m.tx/20.0f;
                text->t[5] = m.ty/20.0f;
                text->bounds[0] = r.xmin/20.0f;
                text->bounds[1] = r.ymin/20.0f;
                text->bounds[2] = r.xmax/20.0f;
                text->bounds[3] = r.ymax/20.0f;

                while (1)
                {
                    int flags = swf_GetU8(tag);
                    if (!flags)
                        break;
                    if (flags & TF_TEXTCONTROL)
                    {
                        if (flags & TF_HASFONT)
                            fid = swf_GetU16(tag);
                        if (flags & TF_HASCOLOR)
                        {
                            color.r = swf_GetU8(tag);
                            color.g = swf_GetU8(tag);
                            color.b = swf_GetU8(tag);
                            if (ST_DEFINETEXT2 == tag->id)
                                color.a = swf_GetU8(tag);
                            else
                                color.a = 255;
                        }
                        if (flags & TF_HASXOFFSET)
                            x = swf_GetS16(tag);
                        if (flags & TF_HASYOFFSET)
                            y = swf_GetS16(tag);
                        if (flags & TF_HASFONT)
                            fontsize = swf_GetU16(tag);
                    }

                    int num = swf_GetU8(tag);
                    if (!num)
                        break;
                    text->strings = realloc(text->strings, (text->num_strings + 1)*sizeof(text->strings[0]));
                    LVGString *str = text->strings + text->num_strings++;
                    str->num_chars = num;
                    str->color = RGBA2U32(&color);
                    str->font_id = (fid < 0) ? fid : idtable[fid].lvg_id;
                    str->height = fontsize/20.0f;
                    str->x = x/20.0f;
                    str->y = y/20.0f;
                    str->chars = calloc(1, num*sizeof(LVGChar));

                    for (int i = 0; i < num; i++)
                    {
                        LVGChar *c = str->chars + i;
                        c->idx = swf_GetBits(tag, gbits);
                        c->x_advance = swf_GetBits(tag, abits);
                        x += c->x_advance;
                    }
                }
            } else if (ST_DEFINESOUND == tag->id)
            {
                LVGSound *sound = clip->sounds + clip->num_sounds;
                uint32_t oldTagPos = swf_GetTagPos(tag);
                swf_SetTagPos(tag, 0);
                id = swf_GetU16(tag);
                int format = swf_GetBits(tag, 4);
                sound->rate = rates[swf_GetBits(tag, 2)];
                /*int bits = */swf_GetBits(tag, 1);
                int stereo = swf_GetBits(tag, 1);
                int num_samples = swf_GetU32(tag);
                sound->channels = stereo ? 2 : 1;
                assert(1 == format || 2 == format); // adpcm, mp3
                unsigned char *buf = &tag->data[tag->pos];
                int buf_size = tag->len - tag->pos;
                if (1 == format)
                {
                    sound->samples = (short*)malloc(num_samples*2*sound->channels);
                    int dec_samples = adpcm_decode(tag, buf_size, stereo + 1, sound->samples, num_samples);
                    assert(dec_samples == num_samples*sound->channels);
                    sound->num_samples = dec_samples/sound->channels;
                } else
                if (2 == format)
                {
                    int rate, channels;
                    sound->samples = lvgLoadMP3Buf(buf, buf_size, &rate, &channels, &sound->num_samples);
                    assert(num_samples == sound->num_samples && sound->channels == channels && sound->rate == rate);
                    sound->channels = channels;
                    sound->rate     = rate;
                }
                swf_SetTagPos(tag, oldTagPos);
                idtable[id].type = sound_type;
                idtable[id].lvg_id = clip->num_sounds++;
            } else if (ST_DEFINEVIDEOSTREAM == tag->id)
            {
                uint32_t oldTagPos = swf_GetTagPos(tag);
                swf_SetTagPos(tag, 0);
                id = swf_GetU16(tag);
                idtable[id].type = video_type;
                idtable[id].lvg_id = clip->num_videos++;
                clip->videos = realloc(clip->videos, clip->num_videos*sizeof(LVGVideo));
                LVGVideo *video = clip->videos + idtable[id].lvg_id;
                video->vdec = 0;
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
                uint32_t oldTagPos = swf_GetTagPos(tag);
                id = swf_GetU16(tag);
#ifndef _TEST
                //printf("button(%d) actions:\n", id);
#endif
                idtable[id].type = button_type;
                idtable[id].lvg_id = clip->num_buttons;
                clip->buttons = realloc(clip->buttons, (clip->num_buttons + 1)*sizeof(LVGButton));
                LVGButton *b = clip->buttons + clip->num_buttons++;
                memset(b, 0, sizeof(LVGButton));

                parse_button_record(tag, b, idtable);
                int pos = tag->pos;
                skipActions(tag);
                int size = tag->pos - pos;
                b->btnactions = realloc(b->btnactions, (b->num_btnactions + 1)*sizeof(LVGButtonAction));
                LVGButtonAction *ba = b->btnactions + b->num_btnactions++;
                ba->actions = malloc(size + 4);
                *(uint32_t*)ba->actions = size;
                memcpy(ba->actions + 4, tag->data + pos, size);
                ba->flags = CondOverDownToOverUp;
                assert(0 == ba->actions[4 + size - 1]);
                swf_SetTagPos(tag, oldTagPos);
            } else if (ST_DEFINEBUTTON2 == tag->id)
            {
                uint32_t oldTagPos = swf_GetTagPos(tag);
                id = swf_GetU16(tag);
#ifndef _TEST
                //printf("button2(%d) actions:\n", id);
#endif
                idtable[id].type = button_type;
                idtable[id].lvg_id = clip->num_buttons;
                clip->buttons = realloc(clip->buttons, (clip->num_buttons + 1)*sizeof(LVGButton));
                LVGButton *b = clip->buttons + clip->num_buttons++;
                memset(b, 0, sizeof(LVGButton));
                /*int flags = */swf_GetU8(tag);  // flags: 0 = track as normal button; 1 = track as menu button

                uint32_t offsetpos = swf_GetU16(tag);

                parse_button_record(tag, b, idtable);
                while (offsetpos)
                {
                    if (tag->pos >= tag->len)
                        break;
                    offsetpos = swf_GetU16(tag);
                    uint32_t condition = swf_GetU16(tag);                // condition
                    int pos = tag->pos;
                    skipActions(tag);
                    int size = tag->pos - pos;
                    b->btnactions = realloc(b->btnactions, (b->num_btnactions + 1)*sizeof(LVGButtonAction));
                    LVGButtonAction *ba = b->btnactions + b->num_btnactions++;
                    ba->actions = malloc(size + 4);
                    *(uint32_t*)ba->actions = size;
                    memcpy(ba->actions + 4, tag->data + pos, size);
                    ba->flags = condition;
                    assert(0 == ba->actions[4 + size - 1]);
                }
                swf_SetTagPos(tag, oldTagPos);
            }
        } else if (ST_SOUNDSTREAMHEAD == tag->id || ST_SOUNDSTREAMHEAD2 == tag->id)
        {
            uint32_t oldTagPos = swf_GetTagPos(tag);
            swf_SetTagPos(tag, 0);
            /*int reserve = */swf_GetBits(tag, 4);
            /*stream_rate = rates[*/swf_GetBits(tag, 2)/*]*/;
            /*stream_bits = */swf_GetBits(tag, 1);
            /*stream_channels = */swf_GetBits(tag, 1)/* ? 2 : 1*/;
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
            uint32_t oldTagPos = swf_GetTagPos(tag);
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
                sound->num_samples += dec_samples/stream_channels;
                free(buf);
            } else
            {
                stream_buf_size += size;
                stream_buffer = (char *)realloc(stream_buffer, stream_buf_size);
                memcpy(stream_buffer + old_size, &tag->data[tag->pos], size);
            }
            sound_block_frame = 1;
            swf_SetTagPos(tag, oldTagPos);
        } else if (ST_VIDEOFRAME == tag->id)
        {
            uint32_t oldTagPos = swf_GetTagPos(tag);
            swf_SetTagPos(tag, 0);
            int vid = swf_GetU16(tag);
            if (vid >= clip->num_videos)
            {
                tag = tag->next;
                continue;
            }
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
            int pos = tag->pos;
            skipActions(tag);
            int size = tag->pos - pos;
            frame->actions = realloc(frame->actions, size + 4);
            *(uint32_t*)frame->actions = size;
            memcpy(frame->actions + 4, tag->data + pos, size);
            assert(0 == frame->actions[4 + size - 1]);
        } else if (ST_DOABC == tag->id)
        {
#ifndef EMSCRIPTEN
            void*abccode = swf_ReadABC(tag);
#ifndef _TEST
            swf_DumpABC(stdout, abccode, "");
#endif
            swf_FreeABC(abccode);
#endif
        } else if (ST_SHOWFRAME == tag->id)
        {
            if (stream_buffer && !sound_block_frame)
            {
                flush_stream_sound(clip, group, stream_buffer, stream_buf_size, stream_sound, stream_format, stream_bits, stream_frame, nframe);
                stream_buffer = NULL;
                stream_samples = stream_buf_size = 0;
                stream_sound = stream_frame = -1;
            }
            sound_block_frame = 0;
            nframe++;
        } else if (ST_END == tag->id)
            break;
        tag = tag->next;
    }
    if (stream_buffer)
    {
        flush_stream_sound(clip, group, stream_buffer, stream_buf_size, stream_sound, stream_format, stream_bits, stream_frame, nframe);
#ifndef _TEST
        assert(stream_samples == clip->sounds[stream_sound].num_samples);
#endif
    }
    assert(tag && ST_END == tag->id);
    return tag;
}

static TAG *parsePlacements(TAG *firstTag, character_t *idtable, LVGMovieClip *clip, LVGMovieClipGroup *group, int version)
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
            if (!(flags & PF_CHAR))
                p.id = INVALID_ID;
            if (INVALID_ID != p.id && sprite_type == idtable[p.id].type)
            {
                int group_num = idtable[p.id].lvg_id;
                p.id = 65536 + clip->num_groupstates;
                idtable[p.id].lvg_id = clip->num_groupstates;
                idtable[p.id].type = sprite_type;
                idtable[p.id].reset_frame = group->num_frames;
                clip->groupstates = realloc(clip->groupstates, (clip->num_groupstates + 1)*sizeof(clip->groupstates[0]));
                LVGMovieClipGroupState *groupstate = clip->groupstates + clip->num_groupstates++;
                memset(groupstate, 0, sizeof(LVGMovieClipGroupState));
                groupstate->group_num = group_num;
            }
            SWFPLACEOBJECT *target = &placements[p.depth];
            if (INVALID_ID == p.id)
                p.id = target->id;
            assert(INVALID_ID != p.id);
            target->id = p.id;
            target->depth = p.depth;
            target->blendmode = p.blendmode;
            if (flags & PF_MATRIX) target->matrix = p.matrix;
            if (flags & PF_CXFORM) target->cxform = p.cxform;
            if (flags & PF_RATIO) target->ratio = p.ratio;
            if (flags & PF_CLIPDEPTH) target->clipdepth = p.clipdepth;
            if (flags & PF_NAME)
            {   // name to access objects in action script
                if (target->name)
                    free(target->name);
                target->name = p.name;
                assert(sprite_type == idtable[p.id].type || button_type == idtable[p.id].type || text_type == idtable[p.id].type);
            }
            for (i = 0; i < 19; i++)
                if (p.actions[i])
                {
                    assert(sprite_type == idtable[p.id].type);
                    LVGMovieClipGroupState *groupstate = clip->groupstates + idtable[p.id].lvg_id;
                    LVGMovieClipGroup *g = clip->groups + groupstate->group_num;
#ifndef _TEST
                    //printf("place id=%d have action in event %i\n", p.id, i);
#endif
                    if (g->events[i])
                        free(g->events[i]);
                    g->events[i] = p.actions[i];
                }
        } else if (ST_DEFINESPRITE == tag->id)
        {
            tag = parsePlacements(tag->next, idtable, clip, &clip->groups[clip->num_groups], version);
            clip->num_groups++;
        } else if (ST_STARTSOUND == tag->id/* || ST_STARTSOUND2 == tag->id*/)
        {
            int id = swf_GetU16(tag);
            int flags = swf_GetU8(tag);
            assert(sound_type == idtable[id].type);
            LVGSound *sound = clip->sounds + idtable[id].lvg_id;
            int start_sample = 0, end_sample = sound->num_samples, loops = 0;
            if (flags & PLAY_HasInPoint)
                start_sample = swf_GetU32(tag);
            if (flags & PLAY_HasOutPoint)
                end_sample = swf_GetU32(tag);
            if (flags & PLAY_HasLoops)
                loops = swf_GetU16(tag);
            add_playsound_action(group, group->num_frames, idtable[id].lvg_id, flags, start_sample, end_sample, loops);
        } else if (ST_REMOVEOBJECT == tag->id || ST_REMOVEOBJECT2 == tag->id)
        {
            uint32_t oldTagPos = swf_GetTagPos(tag);
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
            l->name = strdup((char *)tag->data);
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
            LVGMovieClipFrame *frame = group->frames + group->num_frames;
            frame->num_objects = numplacements;
            frame->objects = calloc(1, sizeof(LVGObject)*numplacements);
            for (i = 0, j = 0; i < 65536; i++)
            {
                SWFPLACEOBJECT *p = &placements[i];
                if (INVALID_ID == p->id || p->clipdepth)
                    continue;
                MATRIX *m = &p->matrix;
                CXFORM *cx = &p->cxform;
                LVGObject *o = &frame->objects[j++];
                character_t *c = &idtable[p->id];
                o->id = c->lvg_id;
                o->type = c->type;
                o->depth = p->depth;
                o->ratio = p->ratio;
                o->blend_mode = p->blendmode ? p->blendmode - 1 : 0;
                o->t[0] = m->sx/65536.0f;
                o->t[1] = m->r0/65536.0f;
                o->t[2] = m->r1/65536.0f;
                o->t[3] = m->sy/65536.0f;
                o->t[4] = m->tx/20.0f;
                o->t[5] = m->ty/20.0f;
                o->cxform.mul[0] = cx->r0/256.0f;
                o->cxform.mul[1] = cx->g0/256.0f;
                o->cxform.mul[2] = cx->b0/256.0f;
                o->cxform.mul[3] = cx->a0/256.0f;
                o->cxform.add[0] = cx->r1/256.0f;
                o->cxform.add[1] = cx->g1/256.0f;
                o->cxform.add[2] = cx->b1/256.0f;
                o->cxform.add[3] = cx->a1/256.0f;
                if (c->reset_frame == group->num_frames)
                    o->flags |= 1;
                if (p->name)
                {
                    frame->obj_labels = realloc(frame->obj_labels, (frame->num_labels + 1)*sizeof(frame->obj_labels[0]));
                    LVGObjectLabel *l = frame->obj_labels + frame->num_labels++;
                    l->name = p->name;
                    l->type = o->type;
                    l->id   = o->id;
                    p->name = 0;
                }
#ifdef LVG_INTERPOLATE
                if (group->num_frames)
                {
                    LVGMovieClipFrame *prev = group->frames + group->num_frames - 1;
                    for (int k = 0; k < prev->num_objects; k++)
                    {
                        LVGObject *prev_o = prev->objects + k;
                        if (prev_o->depth == o->depth && prev_o->id == o->id)
                        {
                            if (prev_o->ratio != o->ratio || memcmp(prev_o->t, o->t, sizeof(o->t)))
                            {   // have previous object and it's moving
                                if (abs(prev_o->ratio - o->ratio) > 16384 ||
                                   (fabs(prev_o->t[4] - o->t[4]) > 200) || (fabs(prev_o->t[5] - o->t[5]) > 200) ||
                                   (fabs(prev_o->t[0] - o->t[0]) > 0.3) || (fabs(prev_o->t[3] - o->t[3]) > 0.3))
                                    break;
                                prev_o->interpolate_obj = o;
                            }
                            break;
                        }
                    }
                }
#endif
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
    for (i = 0; i < 65536; i++)
        if (placements[i].name)
            free(placements[i].name);
    free(placements);
    assert(tag && ST_END == tag->id);
    return tag;
}

LVGMovieClip *swf_ReadObjects(SWF *swf)
{
    swf_RemoveJPEGTables(swf);

    character_t *idtable = (character_t*)calloc(1, sizeof(character_t)*65536*2);
    LVGMovieClip *clip = calloc(1, sizeof(LVGMovieClip));
    clip->bounds[0] = swf->movieSize.xmin/20.0f;
    clip->bounds[1] = swf->movieSize.ymin/20.0f;
    clip->bounds[2] = swf->movieSize.xmax/20.0f;
    clip->bounds[3] = swf->movieSize.ymax/20.0f;
    clip->num_groups = 1;
    clip->fps = swf->frameRate/256.0;

    RGBA bg;
    bg.r = bg.b = bg.g = bg.a = 255;

    TAG *tag = swf->firstTag;
    int sound_block_found = 0, sound_block_frame = 0;
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
                int s_sound_block_found = 0, s_sound_block_frame = 0;
                do {
                    tag = tag->next;
                    if (!tag)
                        goto done;
                    if (ST_SOUNDSTREAMBLOCK == tag->id)
                    {
                        s_sound_block_found = 1;
                        s_sound_block_frame = 1;
                    } else if (ST_SHOWFRAME == tag->id)
                    {
                        if (s_sound_block_found && !s_sound_block_frame)
                        {   // stream sound finished
                            clip->num_sounds++;
                            s_sound_block_found = 0;
                        }
                        s_sound_block_frame = 0;
                    }
                } while (ST_END != tag->id);
                if (s_sound_block_found)
                    clip->num_sounds++;
            } else if (ST_DEFINEFONT == tag->id || ST_DEFINEFONT2 == tag->id || ST_DEFINEFONT3 == tag->id)
            {
                SWFFONT *swffont = (SWFFONT *) calloc(1, sizeof(SWFFONT));
                if (ST_DEFINEFONT == tag->id)
                    swf_FontExtract_DefineFont(0, swffont, tag);
                else
                    swf_FontExtract_DefineFont2(0, swffont, tag);
                clip->num_shapes += swffont->numchars;
                clip->num_fonts++;
                swf_FontFree(swffont);
            } else if (ST_DEFINESOUND == tag->id)
                clip->num_sounds++;
        } else if (ST_SOUNDSTREAMBLOCK == tag->id)
        {
            sound_block_found = 1;
            sound_block_frame = 1;
        } else if (ST_SETBACKGROUNDCOLOR == tag->id)
        {
            swf_SetTagPos(tag, 0);
            bg.r = swf_GetU8(tag);
            bg.g = swf_GetU8(tag);
            bg.b = swf_GetU8(tag);
        } else if (ST_SHOWFRAME == tag->id)
        {
            if (sound_block_found && !sound_block_frame)
            {   // stream sound finished
                clip->num_sounds++;
                sound_block_found = 0;
            }
            sound_block_frame = 0;
        }
        tag = tag->next;
    }
done:
    if (sound_block_found)
        clip->num_sounds++;
    clip->bgColor = nvgRGBA(bg.r, bg.g, bg.b, bg.a);
    clip->shapes = calloc(1, sizeof(LVGShapeCollection)*clip->num_shapes);
    clip->images = calloc(1, sizeof(int)*clip->num_images);
    clip->groups = calloc(1, sizeof(LVGMovieClipGroup)*clip->num_groups);
    clip->fonts  = calloc(1, sizeof(LVGFont)*clip->num_fonts);
    clip->sounds = calloc(1, sizeof(LVGSound)*clip->num_sounds);

    clip->num_shapes = 0;
    clip->num_images = 0;
    clip->num_groups = 1;
    clip->num_fonts  = 0;
    clip->num_sounds = 0;
    parseGroup(swf->firstTag, idtable, clip, clip->groups);
    clip->num_groups = 1;
    clip->num_groupstates = 1;
    clip->groupstates = calloc(1, sizeof(LVGMovieClipGroupState));
    parsePlacements(swf->firstTag, idtable, clip, clip->groups, swf->fileVersion);
    free(idtable);
#ifndef _TEST
    assert(clip->groups->num_frames == swf->frameCount);
    //assert(clip->num_groups <= clip->num_groupstates);
#endif
    //clip->last_time = 0.0;
    clip->as_version = swf->fileVersion;
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
