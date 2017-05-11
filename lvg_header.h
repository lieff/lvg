#ifdef EMSCRIPTEN
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>
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

typedef struct LVGSound
{
    short *samples;
    int num_samples, rate, cur_play_byte;
} LVGSound;

typedef struct LVGMovieClip
{
    LVGShapeCollection *shapes;
    int *images;
    LVGMovieClipGroup *groups;
    LVGSound *sounds;
    float bounds[4];
    NVGcolor bgColor;
    int num_shapes, num_images, num_groups, num_sounds;
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

double atof(const char*);
int atoi(const char*);
long int atol(const char*);

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

#ifdef __TINYC__
#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255

#ifdef __CHAR_UNSIGNED__
#define CHAR_MIN 0
#define CHAR_MAX UCHAR_MAX
#else
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#endif

#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN (-INT_MAX - 1)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U

#if __WORDSIZE == 64
#define LONG_MAX 9223372036854775807L
#else
#define LONG_MAX 2147483647L
#endif
#define LONG_MIN (-LONG_MAX - 1L)

#if __WORDSIZE == 64
#define ULONG_MAX 18446744073709551615UL
#else
#define ULONG_MAX 4294967295UL
#endif

# define LLONG_MAX 9223372036854775807LL
# define LLONG_MIN (-LLONG_MAX - 1LL)
# define ULLONG_MAX 18446744073709551615ULL

typedef long int time_t;

struct timespec
{
  long int tv_sec;
  long int tv_nsec;
};

struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  long int tm_gmtoff;
  const char *tm_zone;
};
#endif
