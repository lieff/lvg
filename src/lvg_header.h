#pragma once
#ifndef __TINYC__
#include <stddef.h>
#include <stdint.h>
#endif
#ifdef EMSCRIPTEN
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#endif

enum LVG_OBJECT_TYPE { LVG_OBJ_EMPTY = 0, LVG_OBJ_SHAPE, LVG_OBJ_IMAGE, LVG_OBJ_VIDEO, LVG_OBJ_GROUP, LVG_OBJ_BUTTON, LVG_OBJ_FONT, LVG_OBJ_TEXT };
enum LVG_PLAY_STATE { LVG_PLAYING = 0, LVG_STOPPED };

typedef struct LVGActionCtx LVGActionCtx;

typedef struct LVGColorf
{
    union {
        float rgba[4];
        struct {
            float r, g, b, a;
        };
    };
} LVGColorf;

typedef struct LVGColorTransform
{
    float mul[4];
    float add[4];
} LVGColorTransform;

typedef struct LVGObjectLabel
{
    const char *name;
    int type, id;
} LVGObjectLabel;

typedef struct LVGObject
{
#ifdef LVG_INTERPOLATE
    struct LVGObject *interpolate_obj;
#endif
    int id, type, depth, ratio, flags, blend_mode;
    float t[6];
    LVGColorTransform cxform;
} LVGObject;

typedef struct LVGMovieClipFrame
{
    LVGObject *objects;
    unsigned char *actions;
    LVGObjectLabel *obj_labels; // to access objects from action script
    int num_objects, num_labels;
} LVGMovieClipFrame;

typedef struct LVGFrameLabel
{
    const char *name;
    int frame_num;
} LVGFrameLabel;

typedef struct LVGTimer
{
    uint8_t *func;
    double last_time;
    double timeout;
    int id;
} LVGTimer;

typedef struct LVGStreamSound
{
    int sound_id, start_frame, end_frame;
} LVGStreamSound;

typedef struct LVGMovieClipGroup
{
    LVGMovieClipFrame *frames;
    LVGFrameLabel *labels;
    LVGStreamSound *ssounds;
    // action script
    uint8_t *events[19];  // sprite swf events
    int num_frames, num_labels, num_ssounds;
} LVGMovieClipGroup;

typedef struct LVGMovieClipGroupState
{
    LVGTimer *timers;     // action script timers
    void *movieclip;      // MovieClip class instance
    int group_num, cur_frame, last_acton_frame, play_state, num_timers, events_initialized;
} LVGMovieClipGroupState;

typedef struct LVGShapeCollection LVGShapeCollection;
typedef struct NSVGshape NSVGshape;
typedef struct NSVGimage NSVGimage;
typedef struct platform_params platform_params;

typedef struct LVGShapeCollection
{
    NSVGshape *shapes;
    LVGShapeCollection *morph;
    float bounds[4];
    int num_shapes;
} LVGShapeCollection;

typedef struct LVGFont
{
    int *glyphs; // shape ids
    int num_chars, version;
} LVGFont;

typedef struct LVGChar
{
    unsigned int idx, x_advance;
} LVGChar;

typedef struct LVGString
{
    LVGChar *chars;
    int num_chars, color, font_id;
    float x, y, height;
} LVGString;

typedef struct LVGText
{
    LVGString *strings;
    float bounds[4];
    float t[6];
    int num_strings;
} LVGText;

typedef struct LVGSound
{
    short *samples;
    int num_samples, orig_rate, rate, channels;
} LVGSound;

typedef struct LVGVideoFrame
{
    void *data;
    int len;
} LVGVideoFrame;

typedef struct LVGVideo
{
    LVGVideoFrame *frames;
    void *vdec; // video decoder instance
    int codec, width, height, num_frames;
    int cur_frame, image;
} LVGVideo;

#define CondKeyPress(f)       (f >> 17)
#define CondOverDownToIdle    256
#define CondIdleToOverDown    128
#define CondOutDownToIdle     64
#define CondOutDownToOverDown 32
#define CondOverDownToOutDown 16
#define CondOverDownToOverUp  8
#define CondOverUpToOverDown  4
#define CondOverUpToIdle      2
#define CondIdleToOverUp      1

typedef struct LVGButtonAction
{
    unsigned char *actions;
    uint16_t flags;
} LVGButtonAction;

#define UP_SHAPE    1
#define OVER_SHAPE  2
#define DOWN_SHAPE  4
#define HIT_SHAPE   8

typedef struct LVGButtonState
{
    LVGObject obj;
    uint16_t flags;
} LVGButtonState;

typedef struct LVGButton
{
    LVGButtonState *btn_shapes;
    LVGButtonAction *btnactions;
    void *button_obj;  // Button class instance
    int num_btn_shapes, num_btnactions;
    int prev_mousehit;
} LVGButton;

typedef struct LVGMovieClip
{
    LVGShapeCollection *shapes;
    int *images;
    LVGMovieClipGroup *groups;
    LVGMovieClipGroupState *groupstates;
    LVGFont *fonts;
    LVGText *texts;
    LVGSound *sounds;
    LVGVideo *videos;
    LVGButton *buttons;
    LVGActionCtx *vm;        // action script vm
    float bounds[4];
    LVGColorf bgColor;
    int num_shapes, num_images, num_groups, num_groupstates, num_fonts, num_texts, num_sounds, num_videos, num_buttons, as_version;
    float fps;
    double last_time;
} LVGMovieClip;

typedef struct LVGShader
{
    void *shaders;
    int num_shaders;
} LVGShader;

typedef struct LVGEngine LVGEngine;

/* LVG API */
platform_params *lvgGetParams(LVGEngine *e);
char *lvgGetFileContents(LVGEngine *e, const char *fname, uint32_t *size);
void lvgFree(void *buf);
void lvgTranslate(LVGEngine *e, float x, float y);
void lvgScale(LVGEngine *e, float x, float y);
void lvgViewport(LVGEngine *e, int width, int heigth);
/* Image */
int lvgImageLoad(LVGEngine *e, const char *file);
int lvgImageLoadBuf(LVGEngine *e, const unsigned char *buf, uint32_t buf_size);
void lvgImageFree(LVGEngine *e, int image);
/* Video */
LVGVideo *lvgVideoLoad(LVGEngine *e, const char *file);
int lvgVideoDecodeToFrame(LVGEngine *e, LVGVideo *video, int frame);
void lvgVideoFree(LVGEngine *e, LVGVideo *video);
/* Shader */
LVGShader *lvgShaderLoadJSON(const char *file);
LVGShader *lvgShaderLoadSPIRV(const char *file);
void lvgShaderTraget(LVGShader *shader, int fbo_tex);
void lvgShaderRun(LVGShader *shader, float x1, float y1, float x2, float y2);
int lvgShaderCompile(const char *ps, const char *vs);
/* SVG */
LVGShapeCollection *lvgShapeLoad(LVGEngine *e, const char *file);
void lvgShapeDraw(LVGEngine *e, LVGShapeCollection *svg);
void lvgShapeFree(LVGEngine *e, LVGShapeCollection *svg);
void lvgShapeGetBounds(LVGShapeCollection *svg, double *bounds);
/* SWF */
LVGMovieClip *lvgClipLoad(LVGEngine *e, const char *file);
LVGMovieClip *lvgClipLoadBuf(LVGEngine *e, char *b, size_t file_size, int free_buf);
void lvgClipDraw(LVGEngine *e, LVGMovieClip *clip);
void lvgClipFree(LVGEngine *e, LVGMovieClip *clip);
/* Audio */
int lvgStartAudio(int samplerate, int channels, int format, int buffer, int is_capture, void (*callback)(void *userdata, char *stream, int len), void *userdata);
short *lvgLoadMP3(LVGEngine *e, const char *file_name, int *rate, int *channels, int *num_samples);
short *lvgLoadMP3Buf(const unsigned char *buf, uint32_t buf_size, int *rate, int *channels, int *nsamples);
void lvgPlaySound(LVGEngine *e, LVGSound *sound, int flags, int start_sample, int end_sample, int loops);
void lvgStopAudio(LVGEngine *e);
// action block begins with 32bit size, functions begins with 16bit size
void lvgExecuteActions(LVGActionCtx *ctx, uint8_t *actions, LVGMovieClipGroupState *groupstate, int is_function);
void lvgInitVM(LVGActionCtx *ctx, LVGEngine *e, LVGMovieClip *clip);
void lvgFreeVM(LVGActionCtx *ctx);

extern LVGColorf g_bgColor;

void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
void free(void *);

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

time_t time(time_t *t);
char *asctime(const struct tm *tm);
char *ctime(const time_t *timep);
struct tm *gmtime(const time_t *timep);
struct tm *localtime(const time_t *timep);
time_t mktime(struct tm *tm);
#endif
