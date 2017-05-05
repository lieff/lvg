#include <assert.h>
#include "stb_image.h"
#include "png.h"
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

static SHAPELINE *parseShape(character_t *idtable, LVGMovieClip *clip, NSVGshape *shape,SHAPELINE *fromLine, FILLSTYLE *fills, LINESTYLE *lineStyles, int *x, int *y)
{
    int fillStyle = fromLine->fillstyle0, lineStyle = fromLine->linestyle;
    shape->flags |= NSVG_FLAGS_VISIBLE;
    shape->paths = (NSVGpath*)calloc(1, sizeof(NSVGpath));
    if (fillStyle)
    {
        FILLSTYLE *f = fills + fillStyle - 1;
        if (FILL_SOLID == f->type)
        {
            shape->fill.type = NSVG_PAINT_COLOR;
            shape->fill.color = RGBA2U32(&f->color);
        } else if(f->type == FILL_TILED || f->type == FILL_CLIPPED || f->type == (FILL_TILED|2) || f->type == (FILL_CLIPPED|2))
        {
            shape->fill.type = NSVG_PAINT_IMAGE;
            if (f->id_bitmap != 65535)
                shape->fill.color = clip->images[idtable[f->id_bitmap].lvg_id];
        } else if(f->type == FILL_LINEAR || f->type == FILL_RADIAL)
        {
            shape->fill.type = NSVG_PAINT_COLOR;
            shape->fill.color = RGBA2U32(&f->color);
        }
    }
    if (lineStyle)
    {
        lineStyles += lineStyle - 1;
        shape->stroke.type = NSVG_PAINT_COLOR;
        shape->stroke.color = RGBA2U32(&lineStyles->color);
        shape->strokeWidth = lineStyles->width/20.0f;
    }
    NSVGpath *path = shape->paths;
    SHAPELINE *lines = fromLine;
    path->npts = 2;
    if (moveTo == lines->type)
        lines = lines->next;
    while (lines)
    {
        if (fillStyle != lines->fillstyle0 || lineStyle != lines->linestyle || moveTo == lines->type)
            break;
        path->npts += 6;
        lines = lines->next;
    }
    path->closed = shape->fill.type != NSVG_PAINT_NONE;
    path->pts = (float*)malloc(sizeof(path->pts[0])*path->npts*2);
    path->npts = 0;

    lines = fromLine;
    if (moveTo == lines->type)
    {
        lines = lines->next;
        path_moveTo(path, lines->x/20.0f, lines->y/20.0f);
    } else
        path_moveTo(path, *x/20.0f, *y/20.0f);
    while (lines)
    {
        if (fillStyle != lines->fillstyle0 || lineStyle != lines->linestyle || moveTo == lines->type)
            break;
        if (lineTo == lines->type)
            path_lineTo(path, lines->x/20.0f, lines->y/20.0f);
        else if (splineTo == lines->type)
            path_quadBezTo(path, lines->sx/20.0f, lines->sy/20.0f, lines->x/20.0f, lines->y/20.0f);
        *x = lines->x;
        *y = lines->y;
        lines = lines->next;
    }
    return lines;
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
            assert(1 == clip->num_groups);
            idtable[id].tag = tag;
            idtable[id].bbox = swf_GetDefineBBox(tag);

            if (swf_isShapeTag(tag))
            {
                SHAPE2 *swf_shape = (SHAPE2*)rfx_calloc(sizeof(SHAPE2));
                swf_ParseDefineShape(tag, swf_shape);

                int fillStyle = 0, lineStyle = 0;
                SHAPELINE *lines = swf_shape->lines;
                LVGShapeCollection *shape = clip->shapes + clip->num_shapes;
                while (lines)
                {
                    if (fillStyle != lines->fillstyle0 || lineStyle != lines->linestyle || moveTo == lines->type)
                    {
                        fillStyle = lines->fillstyle0;
                        lineStyle = lines->linestyle;
                        shape->num_shapes++;
                    }
                    lines = lines->next;
                }

                shape->shapes = (NSVGshape*)calloc(1, shape->num_shapes*sizeof(NSVGshape));
                shape->bounds[0] = idtable[id].bbox.xmin/20.0f;
                shape->bounds[1] = idtable[id].bbox.ymin/20.0f;
                shape->bounds[2] = idtable[id].bbox.xmax/20.0f;
                shape->bounds[3] = idtable[id].bbox.ymax/20.0f;

                int x = 0, y = 0, nshapes = 0;
                lines = swf_shape->lines;
                while (lines)
                {
                    lines = parseShape(idtable, clip, shape->shapes + nshapes++, lines, swf_shape->fillstyles, swf_shape->linestyles, &x, &y);
                }
                assert(nshapes == shape->num_shapes);

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
        } else if (swf_isPlaceTag(tag))
            group->frames[group->num_frames].num_objects++;
        else if (tag->id == ST_SHOWFRAME)
            group->num_frames++;
        else if (tag->id == ST_END)
            break;
        tag = tag->next;
    }
}

static void parsePlacements(TAG *firstTag, character_t *idtable, LVGMovieClip *clip, LVGMovieClipGroup *group)
{
    group->num_frames = 0;
    SWFPLACEOBJECT *placements = (SWFPLACEOBJECT*)rfx_calloc(sizeof(SWFPLACEOBJECT)*65536);
    int i, numplacements = 0;
    TAG *tag = firstTag;
    while (tag)
    {
        if (swf_isPlaceTag(tag))
        {
            SWFPLACEOBJECT p;
            swf_GetPlaceObject(tag, &p);
            placements[numplacements++] = p;
            swf_PlaceObjectFree(&p);
        } else if (tag->id == ST_DEFINESPRITE)
        {
            swf_UnFoldSprite(tag);
            parsePlacements(tag->next, idtable, clip, &clip->groups[clip->num_groups]);
            swf_FoldSprite(tag);
            clip->num_groups++;
        } else if (tag->id == ST_REMOVEOBJECT || tag->id == ST_REMOVEOBJECT2)
        {
        } else if (tag->id == ST_SHOWFRAME)
        {
            qsort(placements, numplacements, sizeof(SWFPLACEOBJECT), compare_placements);
            assert(numplacements == group->frames[group->num_frames].num_objects);
            group->frames[group->num_frames].objects = calloc(1, sizeof(LVGObject)*numplacements);
            for (int i = 0; i < numplacements; i++)
            {
                SWFPLACEOBJECT *p = &placements[i];
                MATRIX *m = &p->matrix;
                LVGObject *o = &group->frames[group->num_frames].objects[i];
                character_t *c = &idtable[placements[i].id];
                o->id = c->lvg_id;
                o->depth = p->depth;
                o->type = c->type;
                o->t[0] = m->sx/65536.0f;
                o->t[1] = m->r1/65536.0f;
                o->t[2] = m->r0/65536.0f;
                o->t[3] = m->sy/65536.0f;
                o->t[4] = m->tx/20.0f;
                o->t[5] = m->ty/20.0f;
            }
            group->num_frames++;
            numplacements = 0;
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

    character_t *idtable;
    SWFPLACEOBJECT* placements;
    idtable = (character_t*)rfx_calloc(sizeof(character_t)*65536);
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
#ifdef DEBUG
    RENDERBUF buf;
    swf_Render_Init(&buf, 0,0, (swf.movieSize.xmax - swf.movieSize.xmin)/20, (swf.movieSize.ymax - swf.movieSize.ymin)/20, 2, 1);
    swf_RenderSWF(&buf, &swf);
    RGBA *img = swf_Render(&buf);
    //stbi_write_png("svg.png", buf.width, buf.height, 4, img, buf.width*4);
    swf_Render_Delete(&buf);
    clip->num_images++;
    clip->images = realloc(clip->images, clip->num_images*sizeof(int));
    clip->images[clip->num_images - 1] = nvgCreateImageRGBA(vg, buf.width, buf.height, 0, (const unsigned char *)img);
#endif
    return clip;
}
