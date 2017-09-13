#include <config.h>
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
#ifdef __MINGW32__
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#include "lunzip.h"
#include "stb_image.h"
#include <SDL2/SDL.h>
#include <video/video.h>
#include <render/render.h>
#include "lvg.h"
#include "swf/avm1.h"

LVGMovieClip *g_clip;
static zip_t g_zip;
static GLFWwindow *window;
NVGcolor g_bgColor;
int winWidth, winHeight;
int width = 0, height = 0;
double mx = 0, my = 0;
double g_time;
int mkeys = 0;
int last_mkeys;
static const char *g_main_script;
static int is_swf, b_no_actionscript;
#ifdef EMSCRIPTEN
static int is_gles3;
#endif

#if ENABLE_VIDEO && VIDEO_FFMPEG
extern const video_dec ff_decoder;
#endif
extern const render nvg_render;
extern const render nvpr_render;
extern const render null_render;
const render *g_render;
void *g_render_obj;

stbi_uc *stbi__resample_row_hv_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_row(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step);
#if defined(__x86_64__) || defined(_M_X64)
stbi_uc *stbi__resample_row_hv_2_simd(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_simd(stbi_uc *out, stbi_uc const *y, stbi_uc const *pcb, stbi_uc const *pcr, int count, int step);
#endif

#if ENABLE_SCRIPT
int loadScript();
#endif

#ifndef EMSCRIPTEN
void (*onInit)();
void (*onFrame)();
#else
extern void onInit();
extern void onFrame();
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
    READ(&o->cxform.mul, 4*4);
    READ(&o->cxform.add, 4*4);
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

void lvgDrawShape(NSVGshape *shape, LVGColorTransform *cxform)
{
    g_render->render_shape(g_render_obj, shape, cxform);
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

static void combine_cxform(LVGColorTransform *newcxform, LVGColorTransform *cxform)
{
    newcxform->add[0] += cxform->add[0]; newcxform->add[1] += cxform->add[1]; newcxform->add[2] += cxform->add[2]; newcxform->add[3] += cxform->add[3];
    newcxform->mul[0] *= cxform->mul[0]; newcxform->mul[1] *= cxform->mul[1]; newcxform->mul[2] *= cxform->mul[2]; newcxform->mul[3] *= cxform->mul[3];
}

static void lvgDrawClipGroup(LVGMovieClip *clip, LVGMovieClipGroup *group, LVGColorTransform *cxform, int next_frame)
{
    int i, j, nframe = group->cur_frame;
    LVGMovieClipFrame *frame = group->frames + nframe;
    int cur_frame = group->cur_frame;
    if (!b_no_actionscript)
    {
        if (!group->vm)
        {
            group->vm = malloc(sizeof(LVGActionCtx));
            lvgInitVM(group->vm, clip, group);
            for (i = 0; i < clip->num_groups; i++)
            {
                LVGMovieClipGroup *g = clip->groups + i;
                if (!g->movieclip)
                {
                    ASClass *cls = create_instance(&g_movieclip);
                    g->movieclip = cls;
                    cls->priv = (void*)(size_t)i;
                }
            }
        }
        g_properties[0].val.str = (char *)group->movieclip;        // this
        g_properties[1].val.str = (char *)clip->groups->movieclip; // _root
        g_properties[2].val.str = (char *)clip->groups->movieclip; // _level0
        ASVal *val = find_class_member(group->movieclip, "_totalframes"); SET_INT(val, group->num_frames);
        val = find_class_member(group->movieclip, "_framesloaded"); SET_INT(val, group->num_frames);

        for (i = 0; i < clip->num_groups; i++)
        {
            LVGMovieClipGroup *g = clip->groups + i;
            for (j = 0; j < g->num_group_labels; j++)
            {
                LVGGroupLabel *gl = &g->group_labels[j];
                ASVal *v = create_local(THIS, gl->name);
                SET_CLASS(v, (clip->groups + gl->group_num)->movieclip);
            }
        }
        if ((group->cur_frame + 1) != group->last_acton_frame)
        {   // call frame actions only once
            group->last_acton_frame = group->cur_frame + 1;
            lvgExecuteActions(group->vm, frame->actions, 0);
        }
        for (i = 0; i < group->num_timers; i++)
        {
            LVGTimer *t = group->timers + i;
            double time = g_time - t->last_time;
            if (time > t->timeout/1000.0)
            {
                t->last_time = g_time;
                lvgExecuteActions(group->vm, t->func, 1);
            }
        }
    }

    float save_transform[6];
    assert(nframe < group->num_frames);
    for (i = 0; i < group->frames[nframe].num_objects; i++)
    {
        LVGObject *o = &group->frames[nframe].objects[i];
        g_render->get_transform(g_render_obj, save_transform);
        g_render->set_transform(g_render_obj, o->t, 0);
        if (LVG_OBJ_SHAPE == o->type)
        {
            LVGColorTransform newcxform = *cxform;
            combine_cxform(&newcxform, &o->cxform);
            for (j = 0; j < clip->shapes[o->id].num_shapes; j++)
                lvgDrawShape(&clip->shapes[o->id].shapes[j], &newcxform);
        } else
        if (LVG_OBJ_IMAGE == o->type)
        {
            g_render->render_image(g_render_obj, clip->images[o->id]);
        } else
        if (LVG_OBJ_VIDEO == o->type)
        {
            LVGVideo *video = &clip->videos[o->id];
#if ENABLE_VIDEO && VIDEO_FFMPEG
            if ((!o->ratio || o->ratio != video->cur_frame) && o->ratio < video->num_frames)
            {
                static void *vdec;
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
#endif
            g_render->render_image(g_render_obj, video->image);
        } else
        if (LVG_OBJ_GROUP == o->type)
        {
            LVGColorTransform newcxform = *cxform;
            combine_cxform(&newcxform, &o->cxform);
            lvgDrawClipGroup(clip, clip->groups + o->id, &newcxform, next_frame);
        } else
        if (LVG_OBJ_BUTTON == o->type)
        {
            LVGButton *b = clip->buttons + o->id;
            int mouse_hit = 0;
            float t[6];
            g_render->get_transform(g_render_obj, t);
            for (j = 0; j < b->num_hit_shapes; j++)
            {
                LVGShapeCollection *col = &clip->shapes[b->hit_shapes->id];
                for (int k = 0; k < col->num_shapes; k++)
                {
                    NSVGshape *s = col->shapes + k;
                    float x  = s->bounds[0], y  = s->bounds[1];
                    float x1 = s->bounds[2], y1 = s->bounds[3];
                    x  = t[0]*x  + t[2]*y  + t[4];
                    y  = t[1]*x  + t[3]*y  + t[5];
                    x1 = t[0]*x1 + t[2]*y1 + t[4];
                    y1 = t[1]*x1 + t[3]*y1 + t[5];
                    if (mx >= x && mx <=x1 && my >= y && my <=y1)
                    {
                        mouse_hit = 1;
                        break;
                    }
                }
            }
            LVGObject *ob = b->up_shapes;
            int nshapes = b->num_up_shapes;
            if (mouse_hit && b->num_over_shapes)
            {
                ob = b->over_shapes;
                nshapes = b->num_over_shapes;
            }
            if (mouse_hit && (mkeys & 1) && b->num_down_shapes)
            {
                ob = b->down_shapes;
                nshapes = b->num_down_shapes;
            }
            for (j = 0; j < b->num_btnactions; j++)
            {
                LVGButtonAction *ba = b->btnactions + j;
                int flags = 0, keypressed = (mkeys & 1), waspressed = (last_mkeys & 1);
                flags |= (!b->prev_mousehit && mouse_hit) ? CondIdleToOverUp : 0;
                flags |= (b->prev_mousehit && !mouse_hit) ? CondOverUpToIdle : 0;
                flags |= (mouse_hit && !waspressed && keypressed) ? CondOverUpToOverDown : 0;
                flags |= (mouse_hit && waspressed && !keypressed) ? CondOverDownToOverUp : 0;
                flags |= (!mouse_hit && waspressed) ? CondOverDownToOutDown : 0;
                flags |= (!b->prev_mousehit && mouse_hit && keypressed) ? CondOutDownToOverDown : 0;
                //flags |= (!b->prev_mousehit && waspressed && !keypressed) ? CondOutDownToIdle : 0;
                flags |= (!b->prev_mousehit && mouse_hit && waspressed && keypressed) ? CondIdleToOverDown : 0;
                //flags |= (mouse_hit && !waspressed && keypressed) ? CondOverDownToIdle : 0;
                if ((ba->flags & flags) && group->vm)
                {
                    lvgExecuteActions(group->vm, ba->actions, 0);
                }
                b->prev_mousehit = mouse_hit;
            }
            for (j = 0; j < nshapes; j++)
            {
                LVGColorTransform newcxform = *cxform;
                combine_cxform(&newcxform, &ob->cxform);
                if (LVG_OBJ_SHAPE != ob->type)
                    continue;
                for (int k = 0; k < clip->shapes[ob->id].num_shapes; k++)
                    lvgDrawShape(&clip->shapes[ob->id].shapes[k], &newcxform);
                ob++;
            }
        }
        g_render->set_transform(g_render_obj, save_transform, 1);
    }
    if (next_frame && LVG_PLAYING == group->play_state && cur_frame == group->cur_frame/*not changed by as*/)
        group->cur_frame = (group->cur_frame + 1) % group->num_frames;
    if (!b_no_actionscript)
    {   // execute sprite events after frame advance
        ASVal *_currentframe = find_class_member(group->movieclip, "_currentframe");
        SET_INT(_currentframe, group->cur_frame + 1);
        if (!group->events_initialized && group->events[0])
        {
            group->events_initialized = 1;
            lvgExecuteActions(group->vm, group->events[0], 0);
        }
        if (group->events[1])
            lvgExecuteActions(group->vm, group->events[1], 0);
    }
}

void lvgDrawClip(LVGMovieClip *clip)
{
#ifndef _TEST
    int next_frame = 0;
    if ((g_time - clip->last_time) > (1.0/clip->fps))
    {
        next_frame = 1;
        clip->last_time += (1.0/clip->fps);
    }
#else
    int next_frame = 1;
#endif
    LVGColorTransform startcxform;
    memset(&startcxform, 0, sizeof(startcxform));
    startcxform.mul[0] = startcxform.mul[1] = startcxform.mul[2] = startcxform.mul[3] = 1.0f;
    lvgDrawClipGroup(clip, clip->groups, &startcxform, next_frame);
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
            if (frame->actions)
                free(frame->actions);
        }
        for (j = 0; j < group->num_labels; j++)
            free((void*)group->labels[j].name);
        for (j = 0; j < group->num_group_labels; j++)
            free((void*)group->group_labels[j].name);
        free(group->frames);
        free(group->labels);
        free(group->group_labels);
        for (j = 0; j < sizeof(group->events)/sizeof(group->events[0]); j++)
            if (group->events[j])
                free(group->events[j]);
        if (group->vm)
        {
            lvgFreeVM(group->vm);
            free(group->vm);
        }
        if (group->movieclip)
            free_instance(group->movieclip);
        if (group->timers)
            free(group->timers);
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
    last_mkeys = mkeys;
}

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
#elif ENABLE_SCRIPT
    return loadScript();
#else
    return 0;
#endif
}

#ifdef __MINGW32__
#include <windows.h>
#include <shellapi.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc = 0;
    LPWSTR *argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
    char *argv[3] = { (char*)"", 0, 0 };
    if (argvw && argc > 1)
    {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, argvw[1], -1, NULL, 0, NULL, NULL);
        argv[1] = alloca(size_needed + 1);
        WideCharToMultiByte(CP_UTF8, 0, argvw[1], -1, argv[1], size_needed, NULL, NULL);
        argv[1][size_needed] = 0;
        LocalFree(argvw);
    }
#else
int main(int argc, char **argv)
{
#endif
    // check switches
    int i;
    for(i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            break;
        switch (argv[i][1])
        {
        case 'n': b_no_actionscript = 1; break;
        default:
            printf("error: unrecognized option\n");
            return 1;
        }
    }
    char *file_name;
    if (argc <= i)
    {
#if defined(EMSCRIPTEN)
        file_name = "main.lvg";
#else
        printf("error: not enough parameters\n");
        return 1;
#endif
    } else
        file_name = argv[i];
    char *e = strrchr(file_name, '.');
    is_swf = e && !strcmp(e, ".swf");
    if (!is_swf && open_lvg(file_name))
    {
        printf("error: could not open lvg file\n");
        is_swf = 1;
        //return -1;
    }
#ifdef _TEST
    g_render = &null_render;
    if (open_swf(file_name))
    {
        printf("error: could not open swf file\n");
        return -1;
    }
    for (int i = 0; i < 10; i++)
        lvgDrawClip(g_clip);
    lvgCloseClip(g_clip);
    lvgZipClose(&g_zip);
    return 0;
#endif
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
#ifdef __MINGW32__
    putenv("SDL_AUDIODRIVER=winmm");
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
#ifdef __MINGW32__
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("error: glad init failed\n");
        return -1;
    }
#endif

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
