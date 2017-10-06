#include <stdlib.h>
#include <stdio.h>
#include "../rfxswf.h"

char *filtername[] = {
    "dropshadow", "blur", "glow", "bevel", "gradientglow",
    "convolution", "colormatrix", "gradientbevel", 0
};

FILTER *swf_GetFilter(TAG *tag)
{
    U8 type = swf_GetU8(tag);

    if (type == FILTERTYPE_BLUR)
    {
        FILTER_BLUR *f = (FILTER_BLUR *)calloc(1, sizeof(FILTER_BLUR));
        f->type = type;
        f->blurx = swf_GetFixed(tag);
        f->blury = swf_GetFixed(tag);
        U8 flags = swf_GetU8(tag);
        f->passes = (flags & 0xFF) >> 3;
        return (FILTER *)f;
    } else if (type == FILTERTYPE_GLOW)
    {
        FILTER_GLOW *f = (FILTER_GLOW *)calloc(1, sizeof(FILTER_GLOW));
        f->type = type;
        swf_GetRGBA(tag, &f->rgba);
        f->blurx = swf_GetFixed(tag);
        f->blury = swf_GetFixed(tag);
        f->strength = swf_GetFixed8(tag);
        U8 flags = swf_GetU8(tag);
        f->passes = flags & 0x1F;
        f->innerglow = (flags >> 7) & 1;
        f->knockout = (flags >> 6) & 1;
        f->composite = (flags >> 5) & 1;
        return (FILTER *)f;
    } else if (type == FILTERTYPE_DROPSHADOW)
    {
        FILTER_DROPSHADOW *f = (FILTER_DROPSHADOW *)calloc(1, sizeof(FILTER_DROPSHADOW));
        f->type = type;
        swf_GetRGBA(tag, &f->color);
        f->blurx = swf_GetFixed(tag);
        f->blury = swf_GetFixed(tag);
        f->angle = swf_GetFixed(tag);
        f->distance = swf_GetFixed(tag);
        f->strength = swf_GetFixed8(tag);
        U8 flags = swf_GetU8(tag);
        f->passes = flags & 0x1F;
        f->innershadow = (flags >> 7) & 1;
        f->knockout = (flags >> 6) & 1;
        f->composite = (flags >> 5) & 1;
        return (FILTER *)f;
    } else if (type == FILTERTYPE_GRADIENTGLOW)
    {
        FILTER_GRADIENTGLOW *f = (FILTER_GRADIENTGLOW *)calloc(1, sizeof(FILTER_GRADIENTGLOW));
        f->type = type;
        f->gradient = (GRADIENT *)calloc(1, sizeof(GRADIENT));
        f->gradient->num = swf_GetU8(tag);
        f->gradient->rgba = (RGBA *)calloc(1, sizeof(RGBA) * f->gradient->num);
        f->gradient->ratios = (U8 *)calloc(1, sizeof(U8) * f->gradient->num);

        int s;
        for (s = 0; s < f->gradient->num; s++)
            swf_GetRGBA(tag, &f->gradient->rgba[s]);
        for (s = 0; s < f->gradient->num; s++)
            f->gradient->ratios[s] = swf_GetU8(tag);

        f->blurx = swf_GetFixed(tag);
        f->blury = swf_GetFixed(tag);
        f->angle = swf_GetFixed(tag);
        f->distance = swf_GetFixed(tag);
        f->strength = swf_GetFixed8(tag);
        U8 flags = swf_GetU8(tag);
        f->passes = flags & 0x0F;
        f->innershadow = (flags >> 7) & 1;
        f->knockout = (flags >> 6) & 1;
        f->composite = (flags >> 5) & 1;
        f->ontop = (flags >> 4) & 1;
        return (FILTER *)f;
    } else if (type == FILTERTYPE_BEVEL)
    {
        FILTER_BEVEL *f = (FILTER_BEVEL *)calloc(1, sizeof(FILTER_BEVEL));
        f->type = type;
        swf_GetRGBA(tag, &f->shadow);
        swf_GetRGBA(tag, &f->highlight);
        f->blurx = swf_GetFixed(tag);
        f->blury = swf_GetFixed(tag);
        f->angle = swf_GetFixed(tag);
        f->distance = swf_GetFixed(tag);
        f->strength = swf_GetFixed8(tag);
        U8 flags = swf_GetU8(tag);
        f->passes = flags & 0x0F;
        f->innershadow = (flags >> 7) & 1;
        f->knockout = (flags >> 6) & 1;
        f->composite = (flags >> 5) & 1;
        f->ontop = (flags >> 4) & 1;
        return (FILTER *)f;
    } else if (type == FILTERTYPE_GRADIENTBEVEL)
    {
        FILTER_GRADIENTBEVEL *f = (FILTER_GRADIENTBEVEL *)calloc(1, sizeof(FILTER_GRADIENTBEVEL));
        f->type = type;
        f->gradient = (GRADIENT *)calloc(1, sizeof(GRADIENT));
        f->gradient->num = swf_GetU8(tag);
        f->gradient->rgba = (RGBA *)calloc(1, sizeof(RGBA) * f->gradient->num);
        f->gradient->ratios = (U8 *)calloc(1, sizeof(U8) * f->gradient->num);

        int s;
        for (s = 0; s < f->gradient->num; s++)
            swf_GetRGBA(tag, &f->gradient->rgba[s]);
        for (s = 0; s < f->gradient->num; s++)
            f->gradient->ratios[s] = swf_GetU8(tag);

        f->blurx = swf_GetFixed(tag);
        f->blury = swf_GetFixed(tag);
        f->angle = swf_GetFixed(tag);
        f->distance = swf_GetFixed(tag);
        f->strength = swf_GetFixed8(tag);
        U8 flags = swf_GetU8(tag);
        f->passes = flags & 0x0F;
        f->innershadow = (flags >> 7) & 1;
        f->knockout = (flags >> 6) & 1;
        f->composite = (flags >> 5) & 1;
        f->ontop = (flags >> 4) & 1;
        return (FILTER *)f;
    } else if (type == FILTERTYPE_CONVOLUTION)
    {
        FILTER_CONVOLUTION *f = (FILTER_CONVOLUTION *)calloc(1, sizeof(FILTER_CONVOLUTION));
        f->type = type;
        f->matrixx = swf_GetU8(tag);
        f->matrixy = swf_GetU8(tag);
        f->divisor = swf_GetFloat(tag);
        f->bias = swf_GetFloat(tag);
        f->matrix = (float *)calloc(1, f->matrixx * f->matrixy * sizeof (float));

        int s;
        for (s = 0; s < f->matrixx * f->matrixy; s++)
            f->matrix[s] = swf_GetFloat(tag);

        swf_GetRGBA(tag, &f->defaultcolor);
        U8 flags = swf_GetU8(tag);
        f->clampmode = (flags >> 1) & 1;
        f->preservealpha = flags & 1;
        return (FILTER *)f;
    } else if (type == FILTERTYPE_COLORMATRIX)
    {
        FILTER_COLORMATRIX *f = (FILTER_COLORMATRIX *)calloc(1, sizeof(FILTER_COLORMATRIX));
        f->type = type;

        int s;
        for (s = 0; s < 20; s++)
            f->matrix[s] = swf_GetFloat(tag);
        return (FILTER *)f;
    } else
    {
#ifdef _DEBUG
        printf("Reading of filter type %02x not supported yet\n", type);
#endif
    }

    return NULL;
}

FILTER *swf_NewFilter(U8 type)
{
    FILTER *f = 0;

    if (type == FILTERTYPE_BLUR) {
        f = (FILTER *)calloc(1, sizeof(FILTER_BLUR));
        memset(f, 0, sizeof (FILTER_BLUR));
    } else if (type == FILTERTYPE_GLOW) {
        f = (FILTER *)calloc(1, sizeof(FILTER_GLOW));
        memset(f, 0, sizeof (FILTER_GLOW));
    } else if (type == FILTERTYPE_DROPSHADOW) {
        f = (FILTER *)calloc(1, sizeof(FILTER_DROPSHADOW));
        memset(f, 0, sizeof (FILTER_DROPSHADOW));
    } else if (type == FILTERTYPE_GRADIENTGLOW) {
        f = (FILTER *)calloc(1, sizeof(FILTER_GRADIENTGLOW));
        memset(f, 0, sizeof (FILTER_GRADIENTGLOW));
    } else if (type == FILTERTYPE_BEVEL) {
        f = (FILTER *)calloc(1, sizeof(FILTER_BEVEL));
        memset(f, 0, sizeof (FILTER_BEVEL));
    } else if (type == FILTERTYPE_GRADIENTBEVEL) {
        f = (FILTER *)calloc(1, sizeof(FILTER_GRADIENTBEVEL));
        memset(f, 0, sizeof (FILTER_GRADIENTBEVEL));
    } else if (type == FILTERTYPE_CONVOLUTION) {
        f = (FILTER *)calloc(1, sizeof(FILTER_CONVOLUTION));
        memset(f, 0, sizeof (FILTER_CONVOLUTION));
    } else if (type == FILTERTYPE_COLORMATRIX) {
        f = (FILTER *)calloc(1, sizeof(FILTER_COLORMATRIX));
        memset(f, 0, sizeof (FILTER_COLORMATRIX));
    } else {
#ifdef _DEBUG
        printf("Creation of filter type %02x not supported yet\n", type);
#endif
    }

    if (f)
        f->type = type;

    return f;
}

void swf_DeleteFilter(FILTER *f)
{
    if (f->type == FILTERTYPE_GRADIENTGLOW && ((FILTER_GRADIENTGLOW *)f)->gradient) {
        if (((FILTER_GRADIENTGLOW *)f)->gradient->rgba)
            free(((FILTER_GRADIENTGLOW *)f)->gradient->rgba);
        if (((FILTER_GRADIENTGLOW *)f)->gradient->ratios)
            free(((FILTER_GRADIENTGLOW *)f)->gradient->ratios);
        free(((FILTER_GRADIENTGLOW *)f)->gradient);
    }
    if (f->type == FILTERTYPE_GRADIENTBEVEL && ((FILTER_GRADIENTBEVEL *)f)->gradient) {
        if (((FILTER_GRADIENTBEVEL *)f)->gradient->rgba)
            free(((FILTER_GRADIENTBEVEL *)f)->gradient->rgba);
        if (((FILTER_GRADIENTBEVEL *)f)->gradient->ratios)
            free(((FILTER_GRADIENTBEVEL *)f)->gradient->ratios);
        free(((FILTER_GRADIENTBEVEL *)f)->gradient);
    }
    if (f->type == FILTERTYPE_CONVOLUTION && ((FILTER_CONVOLUTION *)f)->matrix)
        free(((FILTER_CONVOLUTION *)f)->matrix);
    free(f);
}
