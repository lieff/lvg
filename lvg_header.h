#ifdef EMSCRIPTEN
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#endif
#include <math.h>

enum LVG_OBJECT_TYPE { LVG_OBJ_EMPTY = 0, LVG_OBJ_SHAPE, LVG_OBJ_IMAGE, LVG_OBJ_GROUP };

typedef struct LVGObject
{
    int id, type, depth, flags;
    float t[6];
} LVGObject;

typedef struct LVGMovieClipFrame
{
    LVGObject *objects;
    int num_objects;
} LVGMovieClipFrame;

typedef struct LVGMovieClipGroup
{
    LVGMovieClipFrame *frames;
    int num_frames;
} LVGMovieClipGroup;

typedef struct LVGMovieClip
{
    NSVGshape *shapes;
    int *images;
    LVGMovieClipGroup *groups;
    float bounds[4];
    NVGcolor bgColor;
    int num_shapes, num_images, num_groups;
} LVGMovieClip;

char *lvgGetFileContents(const char *fname, uint32_t *size);
void lvgFree(void *buf);
void lvgDrawSVG(NSVGimage *image);
void lvgDrawClip(LVGMovieClip *clip);
NSVGimage *lvgLoadSVG(const char *file);
NSVGimage *lvgLoadSVGB(const char *file);
LVGMovieClip *lvgLoadSWF(const char *file);

extern NVGcontext *vg;
extern NVGcolor g_bgColor;
extern int winWidth;
extern int winHeight;
extern int width;
extern int height;
extern int mkeys;
extern double mx;
extern double my;
extern double g_time;
