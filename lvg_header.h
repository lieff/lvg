#ifdef EMSCRIPTEN
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#endif

enum LVG_OBJECT_TYPE { LVG_OBJ_EMPTY = 0, LVG_OBJ_SHAPE, LVG_OBJ_IMAGE, LVG_OBJ_GROUP };

typedef struct LVGObject
{
    int id, type, depth, flags;
    float t[6];
    float color_mul[4];
    float color_add[4];
} LVGObject;

typedef struct LVGMovieClipFrame
{
    LVGObject *objects;
    int num_objects;
} LVGMovieClipFrame;

typedef struct LVGMovieClipGroup
{
    LVGMovieClipFrame *frames;
    int num_frames, cur_frame;
} LVGMovieClipGroup;

typedef struct LVGShapeCollection
{
    NSVGshape *shapes;
    float bounds[4];
    int num_shapes;
} LVGShapeCollection;

typedef struct LVGMovieClip
{
    LVGShapeCollection *shapes;
    int *images;
    LVGMovieClipGroup *groups;
    float bounds[4];
    NVGcolor bgColor;
    int num_shapes, num_images, num_groups;
    float fps;
    double last_time;
} LVGMovieClip;

NVGpaint nvgLinearGradientTCC(NVGcontext* ctx, float sx, float sy, float ex, float ey,
    float ir, float ig, float ib, float ia, float or, float og, float ob, float oa);

char *lvgGetFileContents(const char *fname, uint32_t *size);
void lvgFree(void *buf);
void lvgDrawSVG(NSVGimage *image);
void lvgDrawClip(LVGMovieClip *clip);
NSVGimage *lvgLoadSVG(const char *file);
NSVGimage *lvgLoadSVGB(const char *file);
LVGMovieClip *lvgLoadSWF(const char *file);
LVGMovieClip *lvgLoadClip(const char *file);

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

double sin(double);
double cos(double);
double tan(double);
double sinh(double);
double cosh(double);
double tanh(double);
double asin(double);
double acos(double);
double atan(double);
double atan2(double, double);
double exp(double);
double log(double);
double log10(double);
double pow(double, double);
double sqrt(double);
double ceil(double);
double floor(double);
double fabs(double);
double ldexp(double, int);
double frexp(double, int*);
double modf(double, double*);
double fmod(double, double);

float sinf(float);
float cosf(float);
float tanf(float);
float sinhf(float);
float coshf(float);
float tanhf(float);
float asinf(float);
float acosf(float);
float atanf(float);
float atan2f(float, float);
float expf(float);
float logf(float);
float log10f(float);
float powf(float, float);
float sqrtf(float);
float ceilf(float);
float floorf(float);
float fabsf(float);
float ldexpf(float, int);
float frexpf(float, int*);
float modff(float, float*);
float fmodf(float, float);
