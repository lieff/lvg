#include "stb_image.h"
#include "png.h"
#include <rfxswf.h>
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "lunzip.h"
#include "nanovg.h"
#include "nanosvg.h"
#include "lvg_header.h"

extern NVGcontext *vg;

enum CHARACTER_TYPE {none_type, shape_type, image_type, text_type, edittext_type, font_type, sprite_type};
typedef struct
{
    TAG *tag;
    SRECT bbox;
    int lvg_id;
    enum CHARACTER_TYPE type;
} character_t;

character_t *idtable;
SWFPLACEOBJECT* placements;

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

static inline uint32_t RGBA2U32(RGBA *c)
{
    return c->r | (c->g << 8) | (c->b << 16) | (c->a << 24);
}


LVGMovieClip *swf_ReadObjects(SWF *swf)
{
    swf_OptimizeTagOrder(swf);
    swf_FoldAll(swf);

    idtable = (character_t*)rfx_calloc(sizeof(character_t)*65536);
    LVGMovieClip *clip = calloc(1, sizeof(LVGMovieClip));
    clip->bounds[0] = swf->movieSize.xmin;
    clip->bounds[1] = swf->movieSize.ymin;
    clip->bounds[2] = swf->movieSize.xmax;
    clip->bounds[3] = swf->movieSize.ymax;

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
        } else if (swf_isPlaceTag(tag))
            clip->num_objects++;
        else if (tag->id == ST_SHOWFRAME || tag->id == ST_END)
            break;
        tag = tag->next;
    }
    clip->shapes = calloc(1, sizeof(NSVGshape)*clip->num_shapes);
    clip->images = calloc(1, sizeof(int)*clip->num_images);
    clip->objects = calloc(1, sizeof(LVGObject)*clip->num_objects);
    placements = (SWFPLACEOBJECT*)rfx_calloc(sizeof(SWFPLACEOBJECT)*clip->num_objects);

    clip->num_shapes = 0;
    clip->num_images = 0;
    tag = swf->firstTag;
    while (tag)
    {
        if (swf_isDefiningTag(tag))
        {
            int id = swf_GetDefineID(tag);
            idtable[id].tag = tag;
            idtable[id].bbox = swf_GetDefineBBox(tag);

            if (swf_isShapeTag(tag))
            {
                SHAPE2 *swf_shape = (SHAPE2*)rfx_calloc(sizeof(SHAPE2));
                swf_ParseDefineShape(tag, swf_shape);
                NSVGshape *shape = clip->shapes + clip->num_shapes;
                shape->flags |= NSVG_FLAGS_VISIBLE;
                shape->paths = (NSVGpath*)calloc(1, sizeof(NSVGpath));
                shape->bounds[0] = idtable[id].bbox.xmin;
                shape->bounds[1] = idtable[id].bbox.ymin;
                shape->bounds[2] = idtable[id].bbox.xmax;
                shape->bounds[3] = idtable[id].bbox.ymax;
                if (swf_shape->numfillstyles)
                {
                    FILLSTYLE *f = swf_shape->fillstyles;
                    shape->fill.type = NSVG_PAINT_COLOR;
                    shape->fill.color = RGBA2U32(&f->color);
                }
                NSVGpath *path = shape->paths;
                struct _SHAPELINE * lines = swf_shape->lines;
                while (lines)
                {
                    if (!path->npts)
                        path->npts += 2;
                    else
                        path->npts += 6;
                    lines = lines->next;
                }
                path->closed = 1;
                path->pts = (float*)malloc(sizeof(path->pts[0])*path->npts*2);
                path->npts = 0;
                lines = swf_shape->lines;
                while (lines)
                {
                    if (!path->npts)
                        path_moveTo(path, lines->x/20.0f, lines->y/20.0f);
                    else
                        path_lineTo(path, lines->x/20.0f, lines->y/20.0f);
                    lines = lines->next;
                }

                idtable[id].type = shape_type;
                idtable[id].lvg_id = clip->num_shapes++;
            } else if(swf_isImageTag(tag))
            {
                int width, height;
                RGBA *data = swf_ExtractImage(tag, &width, &height);
                *(clip->images + clip->num_images) = nvgCreateImageRGBA(vg, width, height, 0, (const unsigned char *)data);
                idtable[id].type = image_type;
                idtable[id].lvg_id = clip->num_images++;
                free(data);
            }
        } else if (tag->id == ST_SHOWFRAME || tag->id == ST_END)
            break;
        tag = tag->next;
    }

    int i, numplacements = 0;
    tag = swf->firstTag;
    while (tag)
    {
        if (swf_isPlaceTag(tag))
        {
            SWFPLACEOBJECT p;
            swf_GetPlaceObject(tag, &p);
            placements[numplacements++] = p;
            swf_PlaceObjectFree(&p);
        }
        if (tag->id == ST_SHOWFRAME || tag->id == ST_END)
            break;
        tag = tag->next;
    }
    qsort(placements, numplacements, sizeof(SWFPLACEOBJECT), compare_placements);

    for (int i = 0; i < numplacements; i++)
    {
        LVGObject *o = &clip->objects[i];
        character_t *c = &idtable[placements[i].id];
        o->id = c->lvg_id;
        o->depth = placements[i].depth;
        o->type = c->type;
    }
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
    return swf_ReadObjects(&swf);

    /*RENDERBUF buf;
    swf_Render_Init(&buf, 0,0, (swf.movieSize.xmax - swf.movieSize.xmin) / 20,
                   (swf.movieSize.ymax - swf.movieSize.ymin) / 20, 2, 1);
    swf_RenderSWF(&buf, &swf);
    RGBA* img = swf_Render(&buf);
    //stbi_write_png("svg.png", buf.width, buf.height, 4, img, buf.width*4);
    //png_write("output.png", (unsigned char*)img, buf.width, buf.height);
    swf_Render_Delete(&buf);
    return nvgCreateImageRGBA(vg, buf.width, buf.height, 0, (const unsigned char *)img);*/
}
