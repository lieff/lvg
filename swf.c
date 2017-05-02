#include "stb_image.h"
#include "png.h"
#include <rfxswf.h>
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "lunzip.h"
#include "nanovg.h"

extern NVGcontext *vg;

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

    if (b[0]!='F' && b[0]!='C') return -1;
    if (b[1]!='W') return -1;
    if (b[2]!='S') return -1;
    uint32_t uncompressedSize = GET32(&b[4]);

    reader_t reader;
    if (b[0]=='C')
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
