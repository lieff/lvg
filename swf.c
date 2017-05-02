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
    union {
        NSVGshape *shape;
        //font_t *font;
        int image;
    } obj;
    enum CHARACTER_TYPE type;
} character_t;

character_t *idtable;
int numplacements;
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


int swf_ReadObjects(SWF *swf)
{
    int img;
    numplacements = 0;
    swf_OptimizeTagOrder(swf);
    swf_FoldAll(swf);

    idtable = (character_t*)rfx_calloc(sizeof(character_t)*65536);

    RGBA bg = swf_GetSWFBackgroundColor(swf);
    g_bgColor = nvgRGBA(bg.r, bg.g, bg.b, bg.a);

    TAG *tag = swf->firstTag;
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
                NSVGshape *shape = (NSVGshape*)calloc(1, sizeof(NSVGshape));
                shape->flags |= NSVG_FLAGS_VISIBLE;
                shape->paths = (NSVGpath*)calloc(1, sizeof(NSVGpath));
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
                        path_moveTo(path, lines->x, lines->y);
                    else
                        path_lineTo(path, lines->x, lines->y);
                    lines = lines->next;
                }

                idtable[id].type = shape_type;
                idtable[id].obj.shape = shape;
            } else if(swf_isImageTag(tag))
            {
                int width, height;
                RGBA *data = swf_ExtractImage(tag, &width, &height);
                idtable[id].type = image_type;
                idtable[id].obj.image = nvgCreateImageRGBA(vg, width, height, 0, (const unsigned char *)data);
                img = idtable[id].obj.image;
                free(data);
            }/* else if(tag->id == ST_DEFINEFONT || tag->id == ST_DEFINEFONT2)
            {
                int t;
                SWFFONT*swffont;
                font_t*font = (font_t*)rfx_calloc(sizeof(font_t));
                idtable[id].obj.font = font;
                swf_FontExtract(swf,id,&swffont);
                font->numchars = swffont->numchars;
                font->glyphs = (SHAPE2**)rfx_calloc(sizeof(SHAPE2*)*font->numchars);
                for(t=0;t<font->numchars;t++) {
                    if(!swffont->glyph[t].shape->fillstyle.n) {
                        // the actual fill color will be overwritten while rendering
                        swf_ShapeAddSolidFillStyle(swffont->glyph[t].shape, &color_white);
                    }
                    font->glyphs[t] = swf_ShapeToShape2(swffont->glyph[t].shape);
                }
                swf_FontFree(swffont);
                idtable[id].type = font_type;
            } else if(tag->id == ST_DEFINEFONTINFO || tag->id == ST_DEFINEFONTINFO2)
            {
                idtable[id].type = font_type;
            } else if(tag->id == ST_DEFINETEXT || tag->id == ST_DEFINETEXT2)
            {
                idtable[id].type = text_type;
            } else if(tag->id == ST_DEFINESPRITE)
            {
                idtable[id].type = sprite_type;
            } else if(tag->id == ST_DEFINEEDITTEXT)
            {
                idtable[id].type = edittext_type;
            }*/
        } else if (tag->id == ST_PLACEOBJECT || tag->id == ST_PLACEOBJECT2)
            numplacements++;
        else if (tag->id == ST_SHOWFRAME || tag->id == ST_END)
            break;
        tag = tag->next;
    }

    placements = (SWFPLACEOBJECT*)rfx_calloc(sizeof(SWFPLACEOBJECT)*numplacements);
    numplacements = 0;

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

    return img;
}


int lvgLoadSWF(const char *file)
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
        return -1;
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

    RENDERBUF buf;
    swf_Render_Init(&buf, 0,0, (swf.movieSize.xmax - swf.movieSize.xmin) / 20,
                   (swf.movieSize.ymax - swf.movieSize.ymin) / 20, 2, 1);
    swf_RenderSWF(&buf, &swf);
    RGBA* img = swf_Render(&buf);
    //stbi_write_png("svg.png", buf.width, buf.height, 4, img, buf.width*4);
    //png_write("output.png", (unsigned char*)img, buf.width, buf.height);
    swf_Render_Delete(&buf);
    return nvgCreateImageRGBA(vg, buf.width, buf.height, 0, (const unsigned char *)img);
}
