#include <stdio.h>
#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#if !defined(EMSCRIPTEN) && defined(DEBUG)
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif


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
    NSVGpath *path;

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
        i = 0;
        for (path = shape->paths; path != NULL; path = path->next)
            i++;
        fwrite(&i, 1, 4, file);
        fwrite(&shape->fill, 1, sizeof(shape->fill), file);
        fwrite(&shape->stroke, 1, sizeof(shape->stroke), file);
        fwrite(&shape->strokeWidth, 1, sizeof(shape->strokeWidth), file);
        fwrite(shape->bounds, 1, sizeof(shape->bounds), file);

        for (path = shape->paths; path != NULL; path = path->next)
        {
            fwrite(&path->npts, 1, 4, file);
            fwrite(&path->closed, 1, 1, file);
            fwrite(path->pts, 1, sizeof(path->pts[0])*path->npts*2, file);
        }
    }

    fclose(file);
    return 0;
}
