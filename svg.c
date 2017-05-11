#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>
#define GL_GLEXT_PROTOTYPES
#ifdef EMSCRIPTEN
#include <emscripten.h>
#define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#if !defined(EMSCRIPTEN) && defined(DEBUG)
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

#include "nanovg.h"
#ifdef EMSCRIPTEN
#define NANOVG_GLES2_IMPLEMENTATION
#else
#include <libtcc.h>
#define NANOVG_GL2_IMPLEMENTATION
#endif
#include "nanovg_gl.h"

#include "lunzip.h"
#include "all.h"
#include "all_lib.h"
#include "lvg_header.h"
#include "lvg.h"
#include <SDL2/SDL.h>

static LVGMovieClip *g_clip;
static zip_t g_zip;
static GLFWwindow *window;
NVGcontext *vg = NULL;
NVGcolor g_bgColor;
int winWidth, winHeight;
int width = 0, height = 0;
double mx = 0, my = 0;
double g_time;
int mkeys = 0;
static const char *g_main_script;
static int is_swf;
static int tcc_buf_pos;
static size_t tcc_buf_size;
static char *tcc_buf;

#ifndef EMSCRIPTEN
void (*onInit)();
void (*onFrame)();
#else
extern void onInit();
extern void onFrame();
#endif

int open_wrapper(const char *pathname, int flags)
{
    if (!strcmp(pathname, "./lib/libtcc1.a"))
    {
        tcc_buf_pos =0;
        tcc_buf_size = sizeof(lib_libtcc1_a);
        tcc_buf = lib_libtcc1_a;
        return INT_MAX;
    }
    char *e = strrchr(pathname, '.');
    if (e && !strcmp(e, ".h"))
    {
        uint32_t size;
        char *file = lvgGetFileContents(pathname, &size);
        if (!file)
            return -1;
        tcc_buf_pos =0;
        tcc_buf_size = size;
        tcc_buf = file;
        return INT_MAX - 1;
    }
    return open(pathname, flags);
}

void close_wrapper(int fd)
{
    if (INT_MAX == fd)
        return;
    if ((INT_MAX - 1) == fd)
    {
        free(tcc_buf);
        tcc_buf = 0;
        return;
    }
    close(fd);
}

ssize_t read_wrapper(int fd, void *buf, size_t count)
{
    if (INT_MAX != fd && (INT_MAX - 1) != fd)
        return read(fd, buf, count);
    size_t rest = tcc_buf_size - tcc_buf_pos;
    size_t to_read = count < rest ? count : rest;
    memcpy(buf, tcc_buf + tcc_buf_pos, to_read);
    tcc_buf_pos += to_read;
    return to_read;
}

off_t lseek_wrapper(int fd, off_t offset, int whence)
{
    if (INT_MAX != fd && (INT_MAX - 1) != fd)
        return lseek(fd, offset, whence);
    switch (whence)
    {
    case SEEK_SET: tcc_buf_pos = offset; break;
    case SEEK_CUR: tcc_buf_pos += offset; break;
    case SEEK_END: tcc_buf_pos = tcc_buf_size + offset; break;
    }
    return tcc_buf_pos;
}

NVGpaint nvgLinearGradientTCC(NVGcontext* ctx,
    float sx, float sy, float ex, float ey,
    float ir, float ig, float ib, float ia, float or, float og, float ob, float oa)
{
    float m = 1.0/255.0;
    return nvgLinearGradient(ctx, sx, sy, ex, ey, nvgRGBAf(ir*m, ig*m, ib*m, ia*m), nvgRGBAf(or*m, og*m, ob*m, oa*m));
}

char *lvgGetFileContents(const char *fname, uint32_t *size)
{
    uint32_t idx;
    if ((idx = lvgZipNameLocate(&g_zip, fname)) == (int32_t)-1)
        return 0;
    return lvgZipDecompress(&g_zip, idx, size);
}

void lvgFree(void *buf)
{
    free(buf);
}

NSVGimage *lvgLoadSVG(const char *file)
{
    char *buf;
    double time = glfwGetTime();
    if (!(buf = lvgGetFileContents(file, 0)))
    {
        printf("error: could not open SVG image.\n");
        return 0;
    }
    double time2 = glfwGetTime();
    printf("zip time: %fs\n", time2 - time);
    NSVGimage *image = nsvgParse(buf, "px", 96.0f);
    free(buf);
    time = glfwGetTime();
    printf("svg load time: %fs\n", time - time2);
#if !defined(EMSCRIPTEN) && defined(DEBUG)
    if (0)
    {
        int w = (int)image->width;
        int h = (int)image->height;
        NSVGrasterizer *rast = nsvgCreateRasterizer();
        if (rast == NULL) {
            printf("Could not init rasterizer.\n");
        }
        unsigned char *img = malloc(w*h*4);
        if (img == NULL) {
            printf("Could not alloc image buffer.\n");
        }
        nsvgRasterize(rast, image, 0, 0, 1, img, w, h, w*4);
        stbi_write_png("svg.png", w, h, 4, img, w*4);
        nsvgDeleteRasterizer(rast);
    }
#endif
    return image;
}

#define READ(p, n) { memcpy(p, buf, n); buf += n; }

char *load_paint(char *buf, NSVGpaint *p)
{
    READ(&p->type, 1);
    if (NSVG_PAINT_COLOR == p->type)
    {
        READ(&p->color, 4);
    } else if (NSVG_PAINT_LINEAR_GRADIENT == p->type || NSVG_PAINT_RADIAL_GRADIENT == p->type)
    {
        int nstops = 0;
        READ(&nstops, 4);
        NSVGgradient *g = p->gradient = (NSVGgradient*)malloc(sizeof(NSVGgradient) + sizeof(NSVGgradientStop)*nstops);
        g->nstops = nstops;
        READ(&g->xform, 4*6);
        READ(&g->fx, 4*2);
        READ(&g->spread, 1);
        READ(g->stops, g->nstops*8);
    }
    return buf;
}

char *load_shape(char *buf, NSVGshape *shape)
{
    int npaths = 0;
    READ(&npaths, 4);
    buf = load_paint(buf, &shape->fill);
    buf = load_paint(buf, &shape->stroke);
    READ(&shape->strokeWidth, 4);
    READ(shape->bounds, 4*4);
    shape->flags |= NSVG_FLAGS_VISIBLE;

    shape->paths = (NSVGpath*)malloc(npaths*sizeof(NSVGpath));
    memset(shape->paths, 0, npaths*sizeof(NSVGpath));
    NSVGpath *path = 0;
    for (int j = 0; j < npaths; j++)
    {
        if (path)
            path->next = &shape->paths[j];
        path = &shape->paths[j];
        READ(&path->npts, 4);
        READ(&path->closed, 1);
        path->pts = (float*)malloc(sizeof(path->pts[0])*path->npts*2);
        READ(path->pts, sizeof(path->pts[0])*path->npts*2);
    }
    return buf;
}

NSVGimage *lvgLoadSVGB(const char *file)
{
    char *buf, *save_buf;
    double time = glfwGetTime();
    if (!(buf = save_buf = lvgGetFileContents(file, 0)))
    {
        printf("error: could not open SVG image.\n");
        return 0;
    }
    double time2 = glfwGetTime();
    printf("zip time: %fs\n", time2 - time);

    NSVGimage *image = (NSVGimage*)calloc(1, sizeof(NSVGimage));

    int nshapes = 0;
    READ(&nshapes, 4);
    READ(&image->width, 4);
    READ(&image->height, 4);
    image->shapes = (NSVGshape*)calloc(1, nshapes*sizeof(NSVGshape));
    NSVGshape *shape = 0;
    for (int i = 0; i < nshapes; i++)
    {
        if (shape)
            shape->next = &image->shapes[i];
        shape = &image->shapes[i];
        buf = load_shape(buf, shape);
    }

    free(save_buf);
    time = glfwGetTime();
    printf("svg load time: %fs\n", time - time2);
    return image;
}

static char *loadShapeCollection(char *buf, LVGShapeCollection *c)
{
    READ(&c->num_shapes, 4);
    READ(c->bounds, 4*4);
    c->shapes = (NSVGshape *)calloc(1, sizeof(NSVGshape)*c->num_shapes);
    for (int i = 0; i < c->num_shapes; i++)
    {
        buf = load_shape(buf, c->shapes + i);
        c->shapes[i].fillRule = NSVG_FILLRULE_EVENODD;
    }
    return buf;
}

static char *loadImage(char *buf, int *image)
{
    int type = 0, len = 0;
    READ(&type, 4);
    READ(&len, 4);
    *image = 0;
    return buf;
}

static char *loadObject(char *buf, LVGObject *o)
{
    READ(&o->id, 4);
    READ(&o->type, 4);
    READ(&o->depth, 4);
    READ(&o->flags, 4);
    READ(&o->t, 4*6);
    READ(&o->color_mul, 4*4);
    READ(&o->color_add, 4*4);
    return buf;
}

static char *loadFrame(char *buf, LVGMovieClipFrame *frame)
{
    READ(&frame->num_objects, 4);
    frame->objects = (LVGObject *)calloc(1, sizeof(LVGObject)*frame->num_objects);
    for (int i = 0; i < frame->num_objects; i++)
        buf = loadObject(buf, frame->objects + i);
    return buf;
}

static char *loadGroup(char *buf, LVGMovieClipGroup *group)
{
    READ(&group->num_frames, 4);
    group->frames = (LVGMovieClipFrame *)calloc(1, sizeof(LVGMovieClipFrame)*group->num_frames);
    for (int i = 0; i < group->num_frames; i++)
        buf = loadFrame(buf, group->frames + i);
    return buf;
}

LVGMovieClip *lvgLoadClip(const char *file)
{
    char *buf, *save_buf;
    double time = glfwGetTime();
    if (!(buf = save_buf = lvgGetFileContents(file, 0)))
    {
        printf("error: could not open movie clip.\n");
        return 0;
    }
    double time2 = glfwGetTime();
    printf("zip time: %fs\n", time2 - time);

    LVGMovieClip *clip = 0;
    uint32_t tag;
    READ(&tag, 4);
    if (tag != *(uint32_t*)"LVGC")
        goto fail;
    clip = (LVGMovieClip*)calloc(1, sizeof(LVGMovieClip));
    READ(clip->bounds, 4*4);
    READ(clip->bgColor.rgba, 4*4);
    READ(&clip->num_shapes, 4);
    READ(&clip->num_images, 4);
    READ(&clip->num_groups, 4);
    READ(&clip->fps, 4);
    clip->shapes = (LVGShapeCollection*)calloc(1, sizeof(LVGShapeCollection)*clip->num_shapes);
    clip->images = (int*)calloc(1, sizeof(int)*clip->num_images);
    clip->groups = (LVGMovieClipGroup*)calloc(1, sizeof(LVGMovieClipGroup)*clip->num_groups);
    int i;
    for (i = 0; i < clip->num_shapes; i++)
        buf = loadShapeCollection(buf, clip->shapes + i);
    for (i = 0; i < clip->num_images; i++)
        buf = loadImage(buf, clip->images + i);
    for (i = 0; i < clip->num_groups; i++)
        buf = loadGroup(buf, clip->groups + i);
fail:
    free(save_buf);
    time = glfwGetTime();
    printf("movie clip load time: %fs\n", time - time2);
    clip->last_time = g_time;
    return clip;
}

static inline NVGcolor nvgColorU32(uint32_t c)
{
    return nvgRGBA(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff, 255);
}

static NVGcolor transformColor(NVGcolor color, LVGObject *o)
{
    if (!o)
        return color;
    color = nvgRGBAf(color.r*o->color_mul[0], color.g*o->color_mul[1], color.b*o->color_mul[2], color.a*o->color_mul[3]);
    color = nvgRGBAf(color.r + o->color_add[0], color.g + o->color_add[1], color.b + o->color_add[2], color.a + o->color_add[3]);
    color = nvgRGBAf(fmax(0.0f, fmin(color.r, 1.0f)), fmax(0.0f, fmin(color.g, 1.0f)), fmax(0.0f, fmin(color.b, 1.0f)), fmax(0.0f, fmin(color.a, 1.0f)));
    return color;
}

static void nvgSVGLinearGrad(struct NVGcontext *vg, struct NSVGshape *shape, LVGObject *o, int is_fill)
{
    struct NSVGgradient *grad = shape->fill.gradient;
    float sx = shape->bounds[0];
    float sy = shape->bounds[1];
    float ex = shape->bounds[0];
    float ey = shape->bounds[3];
    NVGcolor cs = transformColor(nvgColorU32(grad->stops[0].color), o);
    NVGcolor ce = transformColor(nvgColorU32(grad->stops[grad->nstops - 1].color), o);

    NVGpaint p = nvgLinearGradient(vg, sx, sy, ex, ey, cs, ce);
    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

static void nvgSVGRadialGrad(struct NVGcontext *vg, struct NSVGshape *shape, LVGObject *o, int is_fill)
{
    struct NSVGgradient *grad = shape->fill.gradient;
    float cx = (shape->bounds[0] + shape->bounds[2])/2.0;
    float cy = (shape->bounds[1] + shape->bounds[3])/2.0;
    NVGcolor cs = transformColor(nvgColorU32(grad->stops[0].color), o);
    NVGcolor ce = transformColor(nvgColorU32(grad->stops[grad->nstops - 1].color), o);

    NVGpaint p = nvgRadialGradient(vg, cx, cy, 0, cx, cs, ce);
    if (is_fill)
        nvgFillPaint(vg, p);
    else
        nvgStrokePaint(vg, p);
}

void lvgDrawShape(NSVGshape *shape, LVGObject *o)
{
    int i;
    NSVGpath *path;
    nvgBeginPath(vg);
    for (path = shape->paths; path != NULL; path = path->next)
    {
        nvgMoveTo(vg, path->pts[0], path->pts[1]);
        int l = path->npts - 1;
        //l = (int)(l*g_time*0.4) % l;
        for (i = 0; i < l; i += 3)
        {
            float *p = &path->pts[i*2];
            nvgBezierTo(vg, p[2], p[3], p[4], p[5], p[6], p[7]);
        }
        if (path->closed)
            nvgLineTo(vg, path->pts[0], path->pts[1]);
    }
    if (NSVG_PAINT_COLOR == shape->fill.type)
    {
        nvgFillColor(vg, transformColor(nvgColorU32(shape->fill.color), o));
    } else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
        nvgSVGLinearGrad(vg, shape, o, 1);
    else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
        nvgSVGRadialGrad(vg, shape, o, 1);
    else if (NSVG_PAINT_IMAGE == shape->fill.type)
    {
        int w = shape->bounds[2] - shape->bounds[0], h = shape->bounds[3] - shape->bounds[1];
        NVGpaint imgPaint = nvgImagePattern(vg, shape->bounds[0], shape->bounds[1], w, h, 0, shape->fill.color, 1.0f);
        nvgFillPaint(vg, imgPaint);
    }
    if (NSVG_PAINT_NONE != shape->fill.type)
    {
        if (NSVG_FILLRULE_EVENODD == shape->fillRule)
            nvgPathWinding(vg, NVG_HOLE);
        nvgFill(vg);
    }
    if (NSVG_PAINT_NONE != shape->stroke.type)
    {
        if (NSVG_PAINT_COLOR == shape->stroke.type)
            nvgStrokeColor(vg, transformColor(nvgColorU32(shape->stroke.color), o));
        else if (NSVG_PAINT_LINEAR_GRADIENT == shape->fill.type)
            nvgSVGLinearGrad(vg, shape, o, 0);
        else if (NSVG_PAINT_RADIAL_GRADIENT == shape->fill.type)
            nvgSVGRadialGrad(vg, shape, o, 0);
        nvgStrokeWidth(vg, shape->strokeWidth);
        nvgStroke(vg);
    }
}

void lvgDrawSVG(NSVGimage *image)
{
    NSVGshape *shape;
    for (shape = image->shapes; shape != NULL; shape = shape->next)
    {
        if (!(shape->flags & NSVG_FLAGS_VISIBLE))
            continue;
        lvgDrawShape(shape, 0);
    }
}

static void lvgDrawClipGroup(LVGMovieClip *clip, LVGMovieClipGroup *group, int next_frame)
{
    int w, h;
    int frame = group->cur_frame;
    assert(frame < group->num_frames);
    if (next_frame)
        group->cur_frame = (group->cur_frame + 1) % group->num_frames;
    for (int i = 0; i < group->frames[frame].num_objects; i++)
    {
        LVGObject *o = &group->frames[frame].objects[i];
        nvgSave(vg);
        nvgTransform(vg, o->t[0], o->t[1], o->t[2], o->t[3], o->t[4], o->t[5]);
        if (LVG_OBJ_SHAPE == o->type)
        {
            for (int j = 0; j < clip->shapes[o->id].num_shapes; j++)
                lvgDrawShape(&clip->shapes[o->id].shapes[j], o);
        } else
        if (LVG_OBJ_IMAGE == o->type)
        {
            int w, h;
            nvgImageSize(vg, clip->images[o->id], &w, &h);
            NVGpaint imgPaint = nvgImagePattern(vg, 0, 0, w, h, 0, clip->images[o->id], 1.0f);
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, w, h);
            nvgFillPaint(vg, imgPaint);
            nvgFill(vg);
        } else
        if (LVG_OBJ_GROUP == o->type)
        {
            lvgDrawClipGroup(clip, clip->groups + o->id, next_frame);
        }
        nvgRestore(vg);
    }
}

SDL_AudioSpec have;
int cvt_needed;
SDL_AudioCVT cvt;
void fill_audio(void *udata, Uint8 *stream, int len)
{
    LVGSound *sound = (LVGSound *)udata;
    int rest = sound->num_samples*2 - sound->cur_play_byte;
    //printf("len=%d, rest=%d\n", len, rest);
    if (!rest)
    {
silence:
        memset(stream, 0, len);
        return;
    }
    char *buf = (char *)sound->samples + sound->cur_play_byte;
    if (cvt.needed)
    {
        cvt.len = ((int)(len/cvt.len_ratio) + 1) & (~1);
        if (!cvt.buf)
            cvt.buf = malloc(cvt.len*cvt.len_mult);
        while (len)
        {
            cvt.len = MIN(cvt.len, rest);
            if (!cvt.len_cvt && cvt.len)
            {
                memcpy(cvt.buf, buf, cvt.len);
                buf += cvt.len;
                sound->cur_play_byte += cvt.len;
                rest -= cvt.len;
                SDL_ConvertAudio(&cvt);
            }
            if (!cvt.len_cvt)
                goto silence;
            //printf("len=%d, rest=%d, cvt.len=%d, cvt.len_cvt=%d\n", len, rest, cvt.len, cvt.len_cvt);
            int to_copy = MIN(cvt.len_cvt, len);
            memcpy(stream, cvt.buf, to_copy);
            stream += to_copy;
            cvt.len_cvt -= to_copy;
            len -= to_copy;
            memmove(cvt.buf, cvt.buf + to_copy, cvt.len_cvt);
        }
    } else
    {
        len = MIN(len, rest);
        memcpy(stream, buf, len);
        //SDL_MixAudio(stream, cvt.buf, len, SDL_MIX_MAXVOLUME);
        sound->cur_play_byte += len;
    }
}

void lvgPlaySound(LVGSound *sound)
{
    sound->cur_play_byte = 0;
    if (SDL_AUDIO_STOPPED != SDL_GetAudioStatus())
        return;
    SDL_AudioSpec wanted;
    memset(&wanted, 0, sizeof(wanted));
    wanted.freq = sound->rate;
    wanted.format = AUDIO_S16;
    wanted.channels = 1;
    wanted.samples = 4096;
    wanted.callback = fill_audio;
    wanted.userdata = sound;
#ifdef SDL2
    int dev = SDL_OpenAudioDevice(NULL, 0, &wanted, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (dev < 0)
    {
        printf("error: couldn't open audio: %s\n", SDL_GetError());
        return;
    }
#else
    if (SDL_OpenAudio(&wanted, &have) < 0)
    {
        printf("error: couldn't open audio: %s\n", SDL_GetError());
        return;
    }
#endif
    int ret = SDL_BuildAudioCVT(&cvt, wanted.format, wanted.channels, wanted.freq, have.format, have.channels, have.freq);
    if (ret < 0)
    {
        printf("error: couldn't open converter: %s\n", SDL_GetError());
        return;
    }
    //printf("info: rate=%d, channels=%d, format=%x, change=%d\n", have.freq, have.channels, have.format, cvt.needed); fflush(stdout);
    cvt.len_cvt = 0;
#ifdef SDL2
    SDL_PauseAudioDevice(dev, 0);
#else
    SDL_PauseAudio(0);
#endif
}

void lvgDrawClip(LVGMovieClip *clip)
{
    int next_frame = 0;
    if ((g_time - clip->last_time) > (1.0/clip->fps))
    {
        if (0 == clip->groups->cur_frame && clip->num_sounds)
            lvgPlaySound(clip->sounds);
        next_frame = 1;
        clip->last_time += (1.0/clip->fps);
    }
    lvgDrawClipGroup(clip, clip->groups, next_frame);
/*#ifdef DEBUG
    int w, h;
    nvgImageSize(vg, clip->images[clip->num_images - 1], &w, &h);
    NVGpaint imgPaint = nvgImagePattern(vg, 600, 0, w, h, 0, clip->images[clip->num_images - 1], 1.0f);
    nvgBeginPath(vg);
    nvgRect(vg, 600, 0, w, h);
    nvgFillPaint(vg, imgPaint);
    nvgFill(vg);
#endif*/
}

void swfOnFrame()
{
    float clip_w = g_clip->bounds[2] - g_clip->bounds[0], clip_h = g_clip->bounds[3] - g_clip->bounds[1];
    float scalex = width/clip_w;
    float scaley = height/clip_h;
    float scale = scalex < scaley ? scalex : scaley;

    nvgTranslate(vg, -(clip_w*scale - width)/2, -(clip_h*scale - height)/2);
    nvgScale(vg, scale, scale);
    lvgDrawClip(g_clip);
}

void drawframe()
{
    //SDL_Event event;
    //SDL_PollEvent(&event);
    glfwPollEvents();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &width, &height);
    glfwGetCursorPos(window, &mx, &my);
    mkeys = 0;
    mkeys |= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? 1 : 0;
    mkeys |= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ? 2 : 0;
    mkeys |= glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ? 4 : 0;

    glViewport(0, 0, width, height);
    glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(vg, winWidth, winHeight, (float)width / (float)winWidth);
    //nvgSave(vg);

    g_time = glfwGetTime();
#ifdef EMSCRIPTEN
    if (is_swf)
        swfOnFrame();
    if (g_main_script)
#else
    if (onFrame)
#endif
        onFrame();

    //nvgRestore(vg);
    nvgEndFrame(vg);
    glfwSwapBuffers(window);
}

#ifndef EMSCRIPTEN

struct SYM
{
    const char *m_name;
    void *m_sym;
};

const struct SYM g_syms[] = {
    { "lvgDrawSVG", lvgDrawSVG },
    { "lvgDrawClip", lvgDrawClip },
    { "lvgLoadSVG", lvgLoadSVG },
    { "lvgLoadSVGB", lvgLoadSVGB },
#ifndef EMSCRIPTEN
    { "lvgLoadSWF", lvgLoadSWF },
#endif
    { "lvgLoadClip", lvgLoadClip },
    { "lvgGetFileContents", lvgGetFileContents },
    { "lvgFree", lvgFree },
    { "sin", sin },
    { "cos", cos },
    { "tan", tan },
    { "sinh", sinh },
    { "cosh", cosh },
    { "tanh", tanh },
    { "asin", asin },
    { "acos", acos },
    { "atan", atan },
    { "atan2", atan2 },
    { "exp", exp },
    { "log", log },
    { "log10", log10 },
    { "pow", pow },
    { "sqrt", sqrt },
    { "ceil", ceil },
    { "floor", floor },
    { "fabs", fabs },
    { "ldexp", ldexp },
    { "frexp", frexp },
    { "modf", modf },
    { "fmod", fmod },

    { "sinf", sinf },
    { "cosf", cosf },
    { "tanf", tanf },
    { "sinhf", sinhf },
    { "coshf", coshf },
    { "tanhf", tanhf },
    { "asinf", asinf },
    { "acosf", acosf },
    { "atanf", atanf },
    { "atan2f", atan2f },
    { "expf", expf },
    { "logf", logf },
    { "log10f", log10f },
    { "powf", powf },
    { "sqrtf", sqrtf },
    { "ceilf", ceilf },
    { "floorf", floorf },
    { "fabsf", fabsf },
    { "ldexpf", ldexpf },
    { "frexpf", frexpf },
    { "modff", modff },
    { "fmodf", fmodf },

    { "nvgScale", nvgScale },
    { "nvgSave", nvgSave },
    { "nvgBeginPath", nvgBeginPath },
    { "nvgRoundedRect", nvgRoundedRect },
    { "nvgRGB", nvgRGB },
    { "nvgRGBA", nvgRGBA },
    { "nvgFillColor", nvgFillColor },
    { "nvgFill", nvgFill },
    { "nvgBoxGradient", nvgBoxGradient },
    { "nvgRect", nvgRect },
    { "nvgPathWinding", nvgPathWinding },
    { "nvgFillPaint", nvgFillPaint },
    { "nvgLinearGradient", nvgLinearGradient },
    { "nvgLinearGradientTCC", nvgLinearGradientTCC },
    { "nvgMoveTo", nvgMoveTo },
    { "nvgLineTo", nvgLineTo },
    { "nvgStrokeColor", nvgStrokeColor },
    { "nvgStroke", nvgStroke },
    { "nvgFontSize", nvgFontSize },
    { "nvgFontFace", nvgFontFace },
    { "nvgTextAlign", nvgTextAlign },
    { "nvgFontBlur", nvgFontBlur },
    { "nvgText", nvgText },
    { "nvgRestore", nvgRestore },
    { "nvgTextBounds", nvgTextBounds },
    { "nvgRadialGradient", nvgRadialGradient },
    { "nvgCircle", nvgCircle },
    { "nvgEllipse", nvgEllipse },
    { "nvgBezierTo", nvgBezierTo },
    { "nvgStrokeWidth", nvgStrokeWidth },
    { "nvgArc", nvgArc },
    { "nvgClosePath", nvgClosePath },
    { "nvgScissor", nvgScissor },
    { "nvgTranslate", nvgTranslate },
    { "nvgImageSize", nvgImageSize },
    { "nvgImagePattern", nvgImagePattern },
    { "nvgHSLA", nvgHSLA },
    { "nvgRotate", nvgRotate },
    { "nvgLineCap", nvgLineCap },
    { "nvgLineJoin", nvgLineJoin },
    { "nvgCreateImage", nvgCreateImage },
    { "nvgCreateImageMem", nvgCreateImageMem },
    { "nvgCreateFont", nvgCreateFont },
    { "nvgCreateFontMem", nvgCreateFontMem },
    { "nvgDeleteImage", nvgDeleteImage },
    { "nvgTextMetrics", nvgTextMetrics },
    { "nvgTextBreakLines", nvgTextBreakLines },
    { "nvgTextGlyphPositions", nvgTextGlyphPositions },
    { "nvgTextLineHeight", nvgTextLineHeight },
    { "nvgTextBoxBounds", nvgTextBoxBounds },
    { "nvgGlobalAlpha", nvgGlobalAlpha },
    { "nvgTextBox", nvgTextBox },
    { "nvgDegToRad", nvgDegToRad },
    { "nvgResetScissor", nvgResetScissor },
    { "nvgIntersectScissor", nvgIntersectScissor },

    { "glCreateProgram", glCreateProgram },
    { "glCreateShader", glCreateShader },
    { "glShaderSource", glShaderSource },
    { "glCompileShader", glCompileShader },
    { "glGetShaderiv", glGetShaderiv },
    { "glAttachShader", glAttachShader },
    { "glLinkProgram", glLinkProgram },
    { "glGetProgramiv", glGetProgramiv },
    { "glGetUniformLocation", glGetUniformLocation },
    { "glGetAttribLocation", glGetAttribLocation },
    { "glGenBuffers", glGenBuffers },
    { "glGenVertexArrays", glGenVertexArrays },
    { "glBindVertexArray", glBindVertexArray },
    { "glBindBuffer", glBindBuffer },
    { "glEnableVertexAttribArray", glEnableVertexAttribArray },
    { "glVertexAttribPointer", glVertexAttribPointer },
    { "glBindTexture", glBindTexture },
    { "glGenTextures", glGenTextures },
    { "glTexParameteri", glTexParameteri },
    { "glTexImage2D", glTexImage2D },
    { "glDetachShader", glDetachShader },
    { "glDeleteShader", glDeleteShader },
    { "glDeleteProgram", glDeleteProgram },
    { "glDeleteTextures", glDeleteTextures },
    { "glDeleteBuffers", glDeleteBuffers },
    { "glEnable", glEnable },
    { "glBlendEquation", glBlendEquation },
    { "glBlendFunc", glBlendFunc },
    { "glDisable", glDisable },
    { "glActiveTexture", glActiveTexture },
    { "glUseProgram", glUseProgram },
    { "glUniform1i", glUniform1i },
    { "glUniformMatrix4fv", glUniformMatrix4fv },
    { "glViewport", glViewport },
    { "glBufferData", glBufferData },
    { "glMapBuffer", glMapBuffer },
    { "glUnmapBuffer", glUnmapBuffer },
    { "glScissor", glScissor },
    { "glDrawElements", glDrawElements },

    { "vg", &vg },
    { "g_bgColor", &g_bgColor },
    { "winWidth", &winWidth },
    { "winHeight", &winHeight },
    { "width", &width },
    { "height", &height },
    { "mkeys", &mkeys },
    { "mx", &mx },
    { "my", &my },
    { "g_time", &g_time }
};

void (tcc_error_func)(void *opaque, const char *msg)
{
    printf("%s\n", msg);
    fflush(stdout);
}

int loadScript()
{
    TCCState *s;
    char *buf, *source;
    int size, i;

    if (!(buf = lvgGetFileContents("main.c", 0)))
    {
        printf("error: could not open C script.\n");
        return -1;
    }
    source = malloc(strlen(allh_h) + strlen(buf) + 1);
    source[0] = 0;
    strcat(source, allh_h);
    strcat(source, buf);
    free(buf);

    s = tcc_new();
    tcc_set_error_func(s, 0, tcc_error_func);
    //tcc_set_options(s, "-g");
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
    tcc_set_lib_path(s, "./lib");

    if (tcc_compile_string(s, source) == -1)
        goto error;

    for (i = 0; i < sizeof(g_syms)/sizeof(g_syms[0]); i++)
        tcc_add_symbol(s, g_syms[i].m_name, g_syms[i].m_sym);

    size = tcc_relocate(s, TCC_RELOCATE_AUTO);
    if (size == -1)
        goto error;

    onInit  = tcc_get_symbol(s, "onInit");
    onFrame = tcc_get_symbol(s, "onFrame");
    free(source);
    return 0;
error:
    free(source);
    return -1;
}
#endif

int open_swf(const char *file_name)
{
    struct stat64 st;
    int fd = open(file_name, O_RDONLY);
    if (fd < 0 || fstat64(fd, &st) < 0)
        return -1;
    char *buf = (char*)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    g_clip = lvgLoadSWFBuf(buf, st.st_size, 0);
    munmap(buf, st.st_size);
    close(fd);
    if (!g_clip)
        return -1;
#ifndef EMSCRIPTEN
    onFrame = swfOnFrame;
#endif
    g_bgColor = g_clip->bgColor;
    return 0;
}

int open_lvg(const char *file_name)
{
    if (lvgZipOpen(file_name, &g_zip))
        return -1;
    g_main_script = 0;
    char *buf;
#ifdef EMSCRIPTEN
    if (!(buf = lvgGetFileContents("main.js", 0)))
        printf("error: could not open JS script.\n");
    else
        g_main_script = buf;
    return 0;
#else
    return loadScript();
#endif
}

int main(int argc, char **argv)
{
    if (!glfwInit())
    {
        printf("error: glfw init failed\n");
        return -1;
    }
    if (SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC)) < 0)
    {
        fprintf(stderr, "error: sdl2 init failed: %s\n", SDL_GetError());
        return -1;
    }
    char *file_name = argc > 1 ? argv[1] : "main.lvg";
    char *e = strrchr(file_name, '.');
    is_swf = e && !strcmp(e, ".swf");
    if (!is_swf && open_lvg(file_name))
    {
        printf("error: could not open lvg file\n");
        is_swf = 1;
        //return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, 1);
#ifdef EMSCRIPTEN
    width = 1024, height = 800;
#else
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    width = mode->width - 40, height = mode->height - 80;
    if (width > 1024)
        width = 1024;
    if (height > 800)
        height = 800;
#endif
    window = glfwCreateWindow(width, height, "LVG Player", NULL, NULL);
    if (!window)
    {
        printf("error: could not open window\n");
        return -1;
    }

#ifndef EMSCRIPTEN
    glfwSwapInterval(1);
#endif
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

#ifdef EMSCRIPTEN
    vg = nvgCreateGLES2(/*NVG_ANTIALIAS | NVG_STENCIL_STROKES*/0);
#else
    vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES
#ifdef DEBUG
        | NVG_DEBUG
#endif
        );
#endif

    if (is_swf && open_swf(file_name))
    {
        printf("error: could not open swf file\n");
        return -1;
    }
#ifdef EMSCRIPTEN
    if (g_main_script)
    {
        EM_ASM_({
            var src = Pointer_stringify($0);
            Runtime.loadDynamicLibrarySrc(src);
        }, g_main_script);
    }
    if (g_main_script)
        onInit();
    glfwSetTime(0);
    emscripten_set_main_loop(drawframe, 0, 1);
#else
    if (onInit)
        onInit();
    glfwSetTime(0);
    while (!glfwWindowShouldClose(window))
    {
        drawframe();
    }
#endif
#ifdef EMSCRIPTEN
    nvgDeleteGLES2(vg);
#else
    nvgDeleteGL2(vg);
#endif
    lvgZipClose(&g_zip);
    glfwTerminate();
    SDL_Quit();
    return 0;
}
