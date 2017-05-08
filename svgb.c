#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef UTILITY
#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#endif
#include "nanovg.h"
#include "nanosvg.h"

#if !defined(EMSCRIPTEN) && defined(DEBUG) && defined(UTILITY)
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif
#include "lvg_header.h"

static void save_gradient(FILE *file, NSVGgradient *g)
{
    fwrite(&g->nstops, 1, 4, file);
    fwrite(&g->xform, 1, 4*6, file);
    fwrite(&g->fx, 1, 4*2, file);
    fwrite(&g->spread, 1, 1, file);
    fwrite(g->stops, 1, g->nstops*8, file);
}

static void save_paint(FILE *file, NSVGpaint *p)
{
    fwrite(&p->type, 1, 1, file);
    if (NSVG_PAINT_COLOR == p->type)
        fwrite(&p->color, 1, 4, file);
    else if (NSVG_PAINT_LINEAR_GRADIENT == p->type || NSVG_PAINT_RADIAL_GRADIENT == p->type)
        save_gradient(file, p->gradient);
}

static void save_shape(FILE *file, NSVGshape *shape)
{
    NSVGpath *path;
    int i = 0;
    for (path = shape->paths; path != NULL; path = path->next)
        i++;
    fwrite(&i, 1, 4, file);
    save_paint(file, &shape->fill);
    save_paint(file, &shape->stroke);
    fwrite(&shape->strokeWidth, 1, 4, file);
    fwrite(shape->bounds, 1, 4*4, file);

    for (path = shape->paths; path != NULL; path = path->next)
    {
        fwrite(&path->npts, 1, 4, file);
        fwrite(&path->closed, 1, 1, file);
        fwrite(path->pts, 1, sizeof(path->pts[0])*path->npts*2, file);
    }
}

static void saveShapeCollection(FILE *file, LVGShapeCollection *c)
{
    fwrite(&c->num_shapes, 1, 4, file);
    fwrite(c->bounds, 1, 4*4, file);
    for (int i = 0; i < c->num_shapes; i++)
        save_shape(file, c->shapes + i);
}

static void saveImage(FILE *file, int image)
{
    int type = 0, len = 0;
    fwrite(&type, 1, 4, file);
    fwrite(&len, 1, 4, file);
}

static void saveObject(FILE *file, LVGObject *o)
{
    fwrite(&o->id, 1, 4, file);
    fwrite(&o->type, 1, 4, file);
    fwrite(&o->depth, 1, 4, file);
    fwrite(&o->flags, 1, 4, file);
    fwrite(&o->t, 1, 4*6, file);
    fwrite(&o->color_mul, 1, 4*4, file);
    fwrite(&o->color_add, 1, 4*4, file);
}

static void saveFrame(FILE *file, LVGMovieClipFrame *frame)
{
    fwrite(&frame->num_objects, 1, 4, file);
    for (int i = 0; i < frame->num_objects; i++)
        saveObject(file, frame->objects + i);
}

static void saveGroup(FILE *file, LVGMovieClipGroup *group)
{
    fwrite(&group->num_frames, 1, 4, file);
    for (int i = 0; i < group->num_frames; i++)
        saveFrame(file, group->frames + i);
}

void lvgSaveSWF(const char *file_name, LVGMovieClip *clip)
{
    FILE *file = fopen(file_name, "wb");
    if (!file)
        return;
    fwrite("LVGC", 1, 4, file);
    fwrite(clip->bounds, 1, 4*4, file);
    fwrite(clip->bgColor.rgba, 1, 4*4, file);
    fwrite(&clip->num_shapes, 1, 4, file);
    fwrite(&clip->num_images, 1, 4, file);
    fwrite(&clip->num_groups, 1, 4, file);
    fwrite(&clip->fps, 1, 4, file);
    int i;
    for (i = 0; i < clip->num_shapes; i++)
        saveShapeCollection(file, clip->shapes + i);
    for (i = 0; i < clip->num_images; i++)
        saveImage(file, clip->images[i]);
    for (i = 0; i < clip->num_groups; i++)
        saveGroup(file, clip->groups + i);
    fclose(file);
}

#ifdef UTILITY
int main(int argc, char **argv)
{
    FILE *file = fopen(argv[1], "rb");
    if (!file)
        return 1;
    long inputBufferSize = 0;
    if (fseek(file, 0, SEEK_END) == 0)
        inputBufferSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    void *buf = malloc(inputBufferSize);
    if (!buf)
    {
        fclose(file);
        return 1;
    }
    int readed = fread(buf, 1, inputBufferSize, file);
    fclose(file);
    if (readed != inputBufferSize)
        return 1;

    NSVGimage *image = nsvgParse(buf, "px", 96.0f);
    file = fopen(argv[2], "wb");
    if (!file)
        return 1;

    NSVGshape *shape;

    int i = 0;
    for (shape = image->shapes; shape != NULL; shape = shape->next)
    {
        if (!(shape->flags & NSVG_FLAGS_VISIBLE))
            continue;
        i++;
    }
    fwrite(&i, 1, 4, file);
    fwrite(&image->width, 1, 4, file);
    fwrite(&image->height, 1, 4, file);

    for (shape = image->shapes; shape != NULL; shape = shape->next)
    {
        if (!(shape->flags & NSVG_FLAGS_VISIBLE))
            continue;
        save_shape(shape);
    }

    fclose(file);
    return 0;
}
#endif
