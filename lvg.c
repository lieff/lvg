#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#ifdef __MINGW32__
#include <windows/mman.h>
#else
#include <sys/mman.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>

#ifndef EMSCRIPTEN
#include <libtcc.h>
#endif

#include "lunzip.h"
#include "all.h"
#include "all_lib.h"
#include "stb_image.h"
#include <SDL2/SDL.h>
#include <video/video.h>
#include <render/render.h>
#include "lvg.h"

static LVGMovieClip *g_clip;
static zip_t g_zip;
static GLFWwindow *window;
NVGcolor g_bgColor;
int winWidth, winHeight;
int width = 0, height = 0;
double mx = 0, my = 0;
double g_time;
int mkeys = 0;
static const char *g_main_script;
static int is_swf;
#ifdef EMSCRIPTEN
static int is_gles3;
#endif
static int tcc_buf_pos;
static size_t tcc_buf_size;
static char *tcc_buf;

extern const video_dec ff_decoder;
extern const render nvg_render;
extern const render nvpr_render;
const render *g_render;
void *g_render_obj;

stbi_uc *stbi__resample_row_hv_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_row(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step);
#if defined(__x86_64__) || defined(_M_X64)
stbi_uc *stbi__resample_row_hv_2_simd(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_simd(stbi_uc *out, stbi_uc const *y, stbi_uc const *pcb, stbi_uc const *pcr, int count, int step);
#endif

#ifndef EMSCRIPTEN
void (*onInit)();
void (*onFrame)();
#else
extern void onInit();
extern void onFrame();
#endif

#ifndef EMSCRIPTEN
int open_wrapper(const char *pathname, int flags)
{
    if (!strcmp(pathname, "./lib/libtcc1.a"))
    {
        tcc_buf_pos = 0;
        tcc_buf_size = sizeof(lib_libtcc1_a);
        tcc_buf = (char *)lib_libtcc1_a;
        return INT_MAX;
    }
    char *e = strrchr(pathname, '.');
    if (e && !strcmp(e, ".h"))
    {
        uint32_t size;
        char *file = lvgGetFileContents(pathname, &size);
        if (!file)
            return -1;
        tcc_buf_pos = 0;
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
#endif

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
    if ((idx = lvgZipNameLocate(&g_zip, fname)) != (int32_t)-1)
        return lvgZipDecompress(&g_zip, idx, size);
#ifdef EMSCRIPTEN
    struct stat st;
    int fd = open(fname, O_RDONLY);
    if (fd < 0)
        return 0;
    if (fstat(fd, &st) < 0)
        goto doexit;
    char *file_buf = (char*)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char *buf = malloc(st.st_size + 1);
    memcpy(buf, file_buf, st.st_size);
    buf[st.st_size] = 0;
    munmap(file_buf, st.st_size);
    if (size)
        *size = st.st_size;
doexit:
    close(fd);
    return buf;
#else
    return 0;
#endif
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
    READ(&o->ratio, 4);
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

void lvgDrawShape(NSVGshape *shape, LVGObject *o)
{
    g_render->render_shape(g_render_obj, shape, o);
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
    int frame = group->cur_frame;
    float save_transform[6];
    assert(frame < group->num_frames);
    if (next_frame)
        group->cur_frame = (group->cur_frame + 1) % group->num_frames;
    for (int i = 0; i < group->frames[frame].num_objects; i++)
    {
        LVGObject *o = &group->frames[frame].objects[i];
        g_render->get_transform(g_render_obj, save_transform);
        g_render->set_transform(g_render_obj, o->t, 0);
        if (LVG_OBJ_SHAPE == o->type)
        {
            for (int j = 0; j < clip->shapes[o->id].num_shapes; j++)
                lvgDrawShape(&clip->shapes[o->id].shapes[j], o);
        } else
        if (LVG_OBJ_IMAGE == o->type)
        {
            g_render->render_image(g_render_obj, clip->images[o->id]);
        } else
        if (LVG_OBJ_VIDEO == o->type)
        {
            static void *vdec;
            LVGVideo *video = &clip->videos[o->id];
            if ((!o->ratio || o->ratio != video->cur_frame) && o->ratio < video->num_frames)
            {
                if (!vdec)
                    ff_decoder.init(&vdec, video->codec);
                video_frame out;
                out.planes[0] = NULL;
                if (!o->ratio)
                    video->cur_frame = -1;
                for (;video->cur_frame < o->ratio;)
                {
                    video->cur_frame++;
                    LVGVideoFrame *frame = video->frames + video->cur_frame;
                    ff_decoder.decode(vdec, frame->data, frame->len, &out);
                };
                if (out.planes[0])
                {
                    assert(video->width <= out.width && video->height <= out.height);
                    uint8_t *img = malloc(video->width*video->height*4);
                    uint8_t *pimg = img;
                    uint8_t *rowu = alloca(video->width + 32);
                    uint8_t *rowv = alloca(video->width + 32);
                    uint8_t *py = out.planes[0];
                    uint8_t *pu = out.planes[1];
                    uint8_t *pv = out.planes[2];
                    for(int y = 0; y < video->height/2; y++)
                    {
#if defined(__x86_64__) || defined(_M_X64)
#define stbi__resample_row_hv_2_x stbi__resample_row_hv_2_simd
#define stbi__YCbCr_to_RGB_row_x stbi__YCbCr_to_RGB_simd
#else
#define stbi__resample_row_hv_2_x stbi__resample_row_hv_2
#define stbi__YCbCr_to_RGB_row_x stbi__YCbCr_to_RGB_row
#endif
                        stbi__resample_row_hv_2_x(rowu, pu, pu, video->width/2, 0);
                        stbi__resample_row_hv_2_x(rowv, pv, pv, video->width/2, 0);
                        stbi__YCbCr_to_RGB_row_x(pimg, py, rowu, rowv, video->width, 4);
                        pimg += video->width*4;
                        int last = (y == (video->height/2 - 1));
                        stbi__resample_row_hv_2_x(rowu, pu, pu + (last ? 0 : out.stride[1]), video->width/2, 0);
                        stbi__resample_row_hv_2_x(rowv, pv, pv + (last ? 0 : out.stride[2]), video->width/2, 0);
                        stbi__YCbCr_to_RGB_row_x(pimg, py + out.stride[0], rowu, rowv, video->width, 4);
                        pimg += video->width*4;
                        py += out.stride[0]*2;
                        pu += out.stride[1];
                        pv += out.stride[2];
                    }
                    g_render->update_image(g_render_obj, video->image, img);
                    free(img);
                }
            }
            g_render->render_image(g_render_obj, video->image);
        }
        if (LVG_OBJ_GROUP == o->type)
        {
            lvgDrawClipGroup(clip, clip->groups + o->id, next_frame);
        }
        g_render->set_transform(g_render_obj, save_transform, 1);
    }
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
}

static void deletePaint(NSVGpaint* paint)
{
    if (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT)
    {
        g_render->free_image(paint->gradient->cache);
        free(paint->gradient);
    }
}

void lvgFreeShape(NSVGshape *shape)
{
    NSVGpath *path = shape->paths;
    while (path)
    {
        NSVGpath *next = path->next;
        if (path->pts)
            free(path->pts);
        free(path);
        path = next;
    }
    deletePaint(&shape->fill);
    deletePaint(&shape->stroke);
}

void lvgCloseClip(LVGMovieClip *clip)
{
    int i, j;
    for (i = 0; i < clip->num_shapes; i++)
    {
        LVGShapeCollection *shape = clip->shapes + i;
        for (j = 0; j < shape->num_shapes; j++)
            lvgFreeShape(shape->shapes + j);
        free(shape->shapes);
    }
    for (i = 0; i < clip->num_images; i++)
    {
        g_render->free_image(clip->images[i]);
    }
    for (i = 0; i < clip->num_groups; i++)
    {
        LVGMovieClipGroup *group = clip->groups + i;
        for (j = 0; j < group->num_frames; j++)
        {
            LVGMovieClipFrame *frame = group->frames + j;
            if (frame->objects)
                free(frame->objects);
        }
        free(group->frames);
    }
    for (i = 0; i < clip->num_sounds; i++)
    {
        LVGSound *sound = clip->sounds + i;
        if (sound->samples)
            free(sound->samples);
    }
    for (i = 0; i < clip->num_videos; i++)
    {
        LVGVideo *video = clip->videos + i;
        for (j = 0; j < video->num_frames; j++)
        {
            if (video->frames[j].data)
                free(video->frames[j].data);
        }
        free(video->frames);
        if (video->image)
            g_render->free_image(video->image);
    }
    free(clip->shapes);
    free(clip->images);
    free(clip->groups);
    free(clip->sounds);
    free(clip->videos);
}

void swfOnFrame()
{
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

    g_render->begin_frame(g_render_obj, g_clip, winWidth, winHeight, width, height);

    g_time = glfwGetTime();
#ifdef EMSCRIPTEN
    if (is_swf)
        swfOnFrame();
    if (g_main_script)
#else
    if (onFrame)
#endif
        onFrame();

    g_render->end_frame(g_render_obj);
    glfwSwapBuffers(window);
}

#if !defined(EMSCRIPTEN) && !defined(__MINGW32__)

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
    { "lvgLoadSWF", lvgLoadSWF },
    { "lvgLoadClip", lvgLoadClip },
    { "lvgGetFileContents", lvgGetFileContents },
    { "lvgFree", lvgFree },
    { "lvgStartAudio", lvgStartAudio },
    { "lvgPlaySound", lvgPlaySound },
    { "lvgLoadMP3", lvgLoadMP3 },
    { "lvgLoadMP3Buf", lvgLoadMP3Buf },
    { "printf", printf },

    { "malloc", malloc },
    { "calloc", calloc },
    { "realloc", realloc },
    { "free", free },

    { "atof", atof },

    { "time", time },
    { "localtime", localtime },

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
    { "glGetShaderInfoLog", glGetShaderInfoLog },
    { "glAttachShader", glAttachShader },
    { "glLinkProgram", glLinkProgram },
    { "glGetProgramiv", glGetProgramiv },
    { "glGetUniformLocation", glGetUniformLocation },
    { "glGetAttribLocation", glGetAttribLocation },
    { "glGenBuffers", glGenBuffers },
#ifndef __APPLE__
    { "glGenVertexArrays", glGenVertexArrays },
    { "glBindVertexArray", glBindVertexArray },
#endif
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
#ifndef __APPLE__
    { "glMapBufferRange", glMapBufferRange },
#endif
    { "glUnmapBuffer", glUnmapBuffer },
    { "glScissor", glScissor },
    { "glDrawElements", glDrawElements },
    { "glPushAttrib", glPushAttrib },
    { "glViewport", glViewport },
    { "glMatrixMode", glMatrixMode },
    { "glPushMatrix", glPushMatrix },
    { "glLoadIdentity", glLoadIdentity },
    { "glOrtho", glOrtho },
    { "glEnableClientState", glEnableClientState },

    //{ "vg", &vg },
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
    source = malloc(strlen((char *)allh_h) + strlen(buf) + 1);
    source[0] = 0;
    strcat(source, (char *)allh_h);
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
#ifdef EMSCRIPTEN
    char *buf;
    if (!(buf = lvgGetFileContents("main.js", 0)))
    {
        printf("error: could not open JS script.\n");
        return -1;
    }
    g_main_script = buf;
    if ((buf = lvgGetFileContents("features", 0)))
    {
        is_gles3 = NULL != strstr(buf, "gles3");
        free(buf);
    }
    return 0;
#elif !defined(__MINGW32__)
    return loadScript();
#else
    return 0;
#endif
}

#ifdef __MINGW32__
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc;
    LPWSTR *argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, argvw[1], -1, NULL, 0, NULL, NULL);
    char *arg = alloca(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, argvw[1], -1, arg, size_needed, NULL, NULL);
    char *argv[3] = { (char*)"", arg, 0 };
#else
int main(int argc, char **argv)
{
#endif
    char *file_name = argc > 1 ? argv[1] : "main.lvg";
    char *e = strrchr(file_name, '.');
    is_swf = e && !strcmp(e, ".swf");
    if (!is_swf && open_lvg(file_name))
    {
        printf("error: could not open lvg file\n");
        is_swf = 1;
        //return -1;
    }
#if defined(EMSCRIPTEN)
    if (is_gles3)
    {
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.enableExtensionsByDefault = 1;
        attrs.majorVersion = 2;
        attrs.minorVersion = 0;
        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(0, &attrs);
        if (!context)
        {
            printf("error: this clip requires GLES3, but WebGL 2 is not supported\n");
            return -1;
        }
        emscripten_webgl_make_context_current(context);
    }
#endif

    if (!glfwInit())
    {
        printf("error: glfw init failed\n");
        return -1;
    }
    if (SDL_Init(/*SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC)*/SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "error: sdl2 init failed: %s\n", SDL_GetError());
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_SAMPLES, 8);
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

#ifndef EMSCRIPTEN
    g_render = &nvpr_render;
    if (!g_render->init(&g_render_obj))
#endif
    {
        g_render = &nvg_render;
        if (!g_render->init(&g_render_obj))
        {
            printf("error: could not open render\n");
            return -1;
        }
    }

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
    lvgStopAudio();
    if (g_clip)
        lvgCloseClip(g_clip);
    g_render->release(g_render_obj);
    lvgZipClose(&g_zip);
    glfwTerminate();
    SDL_Quit();
    return 0;
}
