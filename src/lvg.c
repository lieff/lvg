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
#include <audio/audio.h>
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

extern const audio_render sdl_audio_render;
extern const audio_render null_audio_render;
const audio_render *g_audio_render;
void *g_audio_render_obj;

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

void lvgDrawShape(LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode)
{
    g_render->render_shape(g_render_obj, shapecol, cxform, ratio, blend_mode);
}

void lvgDrawSVG(NSVGimage *image)
{
    LVGShapeCollection col;
    memset(&col, 0, sizeof(col));
    col.shapes = image->shapes;
    col.num_shapes = 1;
    lvgDrawShape(&col, 0, 0.0f, BLEND_REPLACE);
}

static void combine_cxform(LVGColorTransform *newcxform, LVGColorTransform *cxform, double alpha)
{
    newcxform->add[0] += cxform->add[0]; newcxform->add[1] += cxform->add[1]; newcxform->add[2] += cxform->add[2]; newcxform->add[3] += cxform->add[3];
    newcxform->mul[0] *= cxform->mul[0]; newcxform->mul[1] *= cxform->mul[1]; newcxform->mul[2] *= cxform->mul[2]; newcxform->mul[3] *= cxform->mul[3]*alpha;
}

static void lvgDrawClipGroup(LVGMovieClip *clip, LVGMovieClipGroupState *groupstate, LVGColorTransform *cxform, int next_frame, int blend_mode)
{
    LVGMovieClipGroup *group = clip->groups + groupstate->group_num;
    LVGMovieClipFrame *frame = group->frames + groupstate->cur_frame;
    if (!group->num_frames)
        return;
    double alpha = 1.0;
    int i, j, cur_frame = groupstate->cur_frame, visible = 1;
    int do_action = (groupstate->cur_frame + 1) != groupstate->last_acton_frame;
    if (do_action)
    {
        groupstate->last_acton_frame = groupstate->cur_frame + 1;
        for (i = 0; i < frame->num_objects; i++)
        {
            LVGObject *o = frame->objects + i;
            if (LVG_OBJ_GROUP == o->type && (o->flags & 1))
            {   // sprite place position - reset sprite
                LVGMovieClipGroupState *gs = clip->groupstates + o->id;
                int group_num = gs->group_num;
                if (!b_no_actionscript)
                {
                    if (gs->movieclip)
                        free_instance(gs->movieclip);
                    if (gs->timers)
                        free(gs->timers);
                }
                memset(gs, 0, sizeof(LVGMovieClipGroupState));
                if (!b_no_actionscript)
                {
                    ASClass *cls = create_instance(&g_movieclip);
                    gs->movieclip = cls;
                    cls->priv = (void*)(size_t)o->id;
                }
                gs->group_num = group_num;
            }
        }
    }
    if (!b_no_actionscript)
    {
        if (!clip->vm)
        {
            clip->vm = malloc(sizeof(LVGActionCtx));
            lvgInitVM(clip->vm, clip);
            for (i = 0; i < clip->num_groupstates; i++)
            {
                LVGMovieClipGroupState *g = clip->groupstates + i;
                if (!g->movieclip)
                {
                    ASClass *cls = create_instance(&g_movieclip);
                    g->movieclip = cls;
                    cls->priv = (void*)(size_t)i;
                }
            }
        }
        g_properties[0].val.cls = groupstate->movieclip;        // this
        g_properties[1].val.cls = clip->groupstates->movieclip; // _root
        g_properties[2].val.cls = clip->groupstates->movieclip; // _level0
        ASVal *val = find_class_member(clip->vm, THIS, "_totalframes"); SET_INT(val, group->num_frames);
        val = find_class_member(clip->vm, THIS, "_framesloaded"); SET_INT(val, group->num_frames);

        if (frame->obj_labels)
        {
            for (i = 0; i < frame->num_labels; i++)
            {
                LVGObjectLabel *l = frame->obj_labels + i;
                ASVal *v = create_local(clip->vm, THIS, l->name);
                if (LVG_OBJ_GROUP == l->type)
                    SET_CLASS(v, (clip->groupstates + l->id)->movieclip)
                else if (LVG_OBJ_BUTTON == l->type)
                {
                    LVGButton *b = clip->buttons + l->id;
                    if (!b->button_obj)
                    {
                        ASClass *cls = create_instance(&g_button);
                        b->button_obj = cls;
                    }
                    SET_CLASS(v, b->button_obj);
                } else
                {
                    assert(LVG_OBJ_EMPTY == l->type);
                    SET_UNDEF(v);
                }
            }
        }
        if (do_action)
        {   // call frame actions only once
            lvgExecuteActions(clip->vm, frame->actions, groupstate, 0);
        }
        ASVal *onEnterFrame = find_class_member(clip->vm, THIS, "onEnterFrame");
        if (onEnterFrame && onEnterFrame->str)
            lvgExecuteActions(clip->vm, (uint8_t*)onEnterFrame->str, groupstate, 1);
        for (i = 0; i < groupstate->num_timers; i++)
        {
            LVGTimer *t = groupstate->timers + i;
            double time = g_time - t->last_time;
            if (time > t->timeout/1000.0)
            {
                t->last_time = g_time;
                lvgExecuteActions(clip->vm, t->func, groupstate, 1);
            }
        }
        val = find_class_member(clip->vm, THIS, "_visible"); visible = to_int(val);
        val = find_class_member(clip->vm, THIS, "_alpha"); alpha = to_double(clip->vm, val);
        /*val = find_class_member(clip->vm, THIS, "blendMode");
        if (val && ASVAL_STRING == val->type)
        {

            static const char *g_blendModes[] =
            {
                "normal", "layer", "multiply", "screen", "lighten", "darken", "difference",
                "add", "subtract", "invert", "alpha", "erase", "overlay", "hardlight"
            };
            for (i = 0; i < sizeof(g_blendModes)/sizeof(g_blendModes[0]); i++)
                if (0 == strcmp_identifier(clip->vm, val->str, g_blendModes[i]))
                {
                    blend_mode = i;
                    break;
                }
        }*/
    }

    float save_transform[6];
    for (i = 0; i < frame->num_objects; i++)
    {
        LVGObject *o = frame->objects + i;
        g_render->get_transform(g_render_obj, save_transform);
        g_render->set_transform(g_render_obj, o->t, 0);
        if (LVG_OBJ_SHAPE == o->type && visible)
        {
            LVGColorTransform newcxform = *cxform;
            combine_cxform(&newcxform, &o->cxform, alpha);
            lvgDrawShape(&clip->shapes[o->id], &newcxform, o->ratio/65535.0f, blend_mode ? blend_mode : o->blend_mode);
        } else
        if (LVG_OBJ_IMAGE == o->type && visible)
        {
            g_render->render_image(g_render_obj, clip->images[o->id]);
        } else
        if (LVG_OBJ_VIDEO == o->type && visible)
        {
            LVGVideo *video = &clip->videos[o->id];
#if ENABLE_VIDEO && VIDEO_FFMPEG
            if ((!o->ratio || o->ratio != video->cur_frame) && o->ratio < video->num_frames)
            {
                if (!video->vdec)
                    ff_decoder.init(&video->vdec, video->codec);
                video_frame out;
                out.planes[0] = NULL;
                if (!o->ratio)
                    video->cur_frame = -1;
                for (;video->cur_frame < o->ratio;)
                {
                    video->cur_frame++;
                    LVGVideoFrame *frame = video->frames + video->cur_frame;
                    ff_decoder.decode(video->vdec, frame->data, frame->len, &out);
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
            combine_cxform(&newcxform, &o->cxform, alpha);
            lvgDrawClipGroup(clip, clip->groupstates + o->id, &newcxform, next_frame, o->blend_mode);
            THIS = groupstate->movieclip; // restore this if changed in other groups
        } else
        if (LVG_OBJ_BUTTON == o->type)
        {
            LVGButton *b = clip->buttons + o->id;
            double btn_alpha = 1.0;
            int btn_visible = 1;
            if (b->button_obj)
            {
                ASVal *val = find_class_member(clip->vm, b->button_obj, "_alpha");
                btn_alpha = to_double(clip->vm, val);
                val = find_class_member(clip->vm, b->button_obj, "_visible");
                btn_visible = to_int(val);
            }
            int mouse_hit = 0;
            float save_t[6];
            g_render->get_transform(g_render_obj, save_t);
            for (j = 0; j < b->num_btn_shapes; j++)
            {
                LVGButtonState *bs = b->btn_shapes + j;
                if (!(bs->flags & HIT_SHAPE))
                    continue;
                assert(LVG_OBJ_SHAPE == bs->obj.type);
                if (LVG_OBJ_SHAPE != bs->obj.type)
                    continue;
                float t[6];
                g_render->set_transform(g_render_obj, bs->obj.t, 0);
                g_render->get_transform(g_render_obj, t);
                Transform3x2 tr;
                tr[0][0] = t[0]; tr[1][0] = t[1];
                tr[0][1] = t[2]; tr[1][1] = t[3];
                tr[0][2] = t[4]; tr[1][2] = t[5];
                inverse(tr, tr);
                float m[2] = { mx, my };
                xform(m, tr, m);
                LVGShapeCollection *col = &clip->shapes[bs->obj.id];
                for (int k = 0; k < col->num_shapes; k++)
                {
                    NSVGshape *s = col->shapes + k;
                    float x  = s->bounds[0], y  = s->bounds[1];
                    float x1 = s->bounds[2], y1 = s->bounds[3];
                    if (m[0] >= x && m[0] <=x1 && m[1] >= y && m[1] <= y1)
                    {
                        if (g_render->inside_shape)
                            mouse_hit = g_render->inside_shape(g_render_obj, s, m[0], m[1]);
                        else
                            mouse_hit = 1;
                        if (mouse_hit)
                            break;
                    }
                }
                g_render->set_transform(g_render_obj, save_t, 1);
            }
            int state_flags = UP_SHAPE;
            if (mouse_hit)
                state_flags = OVER_SHAPE;
            if (mouse_hit && (mkeys & 1))
                state_flags = DOWN_SHAPE;
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
            for (j = 0; j < b->num_btnactions; j++)
            {
                LVGButtonAction *ba = b->btnactions + j;
                if ((ba->flags & flags) && clip->vm)
                {
                    /*printf("button %d events=%x ", o->id, flags);
                    for (int k = 0; k < b->num_btnactions; k++)
                        printf("%x ", b->btnactions[k].flags);
                    printf("\n"); fflush(stdout);*/
                    lvgExecuteActions(clip->vm, ba->actions, groupstate, 0);
                }
            }
            b->prev_mousehit = mouse_hit;
            if (visible && btn_visible)
                for (j = 0; j < b->num_btn_shapes; j++)
                {
                    LVGButtonState *bs = b->btn_shapes + j;
                    if (!(bs->flags & state_flags))
                        continue;
                    assert(LVG_OBJ_SHAPE == bs->obj.type || LVG_OBJ_GROUP == bs->obj.type || LVG_OBJ_BUTTON == bs->obj.type);
                    if (LVG_OBJ_SHAPE != bs->obj.type)
                        continue;
                    g_render->set_transform(g_render_obj, bs->obj.t, 0);
                    LVGColorTransform newcxform = *cxform;
                    combine_cxform(&newcxform, &o->cxform, 1.0);
                    combine_cxform(&newcxform, &bs->obj.cxform, alpha*btn_alpha);
                    lvgDrawShape(&clip->shapes[bs->obj.id], &newcxform, bs->obj.ratio/65535.0f, blend_mode);
                    g_render->set_transform(g_render_obj, save_t, 1);
                }
        } else
        if (LVG_OBJ_TEXT == o->type && visible)
        {
            LVGText *text = clip->texts + o->id;
            for (j = 0; j < text->num_strings; j++)
            {
                LVGString *str = text->strings + j;
                if (str->font_id < 0)
                    continue;
                LVGFont *f = clip->fonts + str->font_id;
                g_render->set_transform(g_render_obj, save_transform, 1);
                g_render->set_transform(g_render_obj, o->t, 0);
                g_render->set_transform(g_render_obj, text->t, 0);
                float scale = str->height/50.0f;
                if (3 == f->version)
                    scale /= 20.0f;
                float t[6] = { scale, 0.0f, 0.0f, scale, str->x, str->y };
                g_render->set_transform(g_render_obj, t, 0);
                for (int k = 0; k < str->num_chars; k++)
                {
                    LVGChar *c = str->chars + k;
                    LVGShapeCollection *shapecol = &clip->shapes[f->glyphs[c->idx]];
                    LVGColorTransform newcxform = *cxform;
                    combine_cxform(&newcxform, &o->cxform, alpha);
                    for (int l = 0; l < shapecol->num_shapes; l++)
                        shapecol->shapes[l].fill.color = str->color;
                    lvgDrawShape(shapecol, &newcxform, 0.0f, blend_mode);
                    float t[6] = { 1.0f, 0.0f, 0.0f, 1.0f, c->x_advance/20.0f/scale, 0.0f };
                    g_render->set_transform(g_render_obj, t, 0);
                }
            }
        }
        g_render->set_transform(g_render_obj, save_transform, 1);
    }
    if (next_frame && LVG_PLAYING == groupstate->play_state && cur_frame == groupstate->cur_frame/*not changed by as*/)
        groupstate->cur_frame = (groupstate->cur_frame + 1) % group->num_frames;
    if (!b_no_actionscript)
    {   // execute sprite events after frame advance
        ASVal *_currentframe = find_class_member(clip->vm, groupstate->movieclip, "_currentframe");
        SET_INT(_currentframe, groupstate->cur_frame + 1);
        if (!groupstate->events_initialized && group->events[0])
        {
            groupstate->events_initialized = 1;
            lvgExecuteActions(clip->vm, group->events[0], groupstate, 0);
        }
        if (group->events[1])
            lvgExecuteActions(clip->vm, group->events[1], groupstate, 0);
    }
}

/*static void printf_frames(LVGMovieClip *clip, LVGMovieClipGroupState *groupstate)
{
    LVGMovieClipGroup *group = clip->groups + groupstate->group_num;
    LVGMovieClipFrame *frame = group->frames + groupstate->cur_frame;
    printf("%d=%3d ", groupstate->group_num, groupstate->cur_frame);
    if (groupstate->cur_frame >= group->num_frames)
        return;
    for (int i = 0; i < frame->num_objects; i++)
    {
        LVGObject *o = &frame->objects[i];
        if (LVG_OBJ_GROUP == o->type)
        {
            printf_frames(clip, clip->groupstates + o->id);
        }
    }
}*/

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
    //printf_frames(clip, clip->groupstates); printf("\n"); fflush(stdout);
    lvgDrawClipGroup(clip, clip->groupstates, &startcxform, next_frame, BLEND_REPLACE);
}

static void deletePaint(NSVGpaint* paint)
{
    if (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT)
    {
        g_render->free_image(g_render_obj, paint->gradient->cache);
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
        if (shape->morph)
        {
            for (j = 0; j < shape->morph->num_shapes; j++)
                lvgFreeShape(shape->morph->shapes + j);
            free(shape->morph->shapes);
        }
    }
    for (i = 0; i < clip->num_images; i++)
    {
        g_render->free_image(g_render_obj, clip->images[i]);
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
            if (frame->obj_labels)
            {
                for (int k = 0; k < frame->num_labels; k++)
                    if (frame->obj_labels[k].name)
                        free((void*)frame->obj_labels[k].name);
                free(frame->obj_labels);
            }
        }
        for (j = 0; j < group->num_labels; j++)
            free((void*)group->labels[j].name);
        free(group->frames);
        free(group->labels);
        for (j = 0; j < sizeof(group->events)/sizeof(group->events[0]); j++)
            if (group->events[j])
                free(group->events[j]);
    }
    for (i = 0; i < clip->num_groupstates; i++)
    {
        LVGMovieClipGroupState *groupstate = clip->groupstates + i;
        if (groupstate->movieclip)
            free_instance(groupstate->movieclip);
        if (groupstate->timers)
            free(groupstate->timers);
    }
    for (i = 0; i < clip->num_fonts; i++)
    {
        LVGFont *font = clip->fonts + i;
        if (font->glyphs)
            free(font->glyphs);
    }
    for (i = 0; i < clip->num_texts; i++)
    {
        LVGText *text = clip->texts + i;
        for (j = 0; j < text->num_strings; j++)
        {
            LVGString *str = text->strings + j;
            if (str->chars)
                free(str->chars);
        }
        free(text->strings);
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
            g_render->free_image(g_render_obj, video->image);
        if (video->vdec)
            ff_decoder.release(video->vdec);
    }
    if (clip->vm)
    {
        lvgFreeVM(clip->vm);
        free(clip->vm);
    }
    free(clip->shapes);
    free(clip->images);
    free(clip->groups);
    free(clip->fonts);
    free(clip->texts);
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
    g_audio_render = &null_audio_render;
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

    g_audio_render = &sdl_audio_render;
    if (!g_audio_render->init(&g_audio_render_obj, 44100, 2, 0, 0, 0))
        g_audio_render = &null_audio_render;
    for (i = 0; i < g_clip->num_sounds; i++)
        g_audio_render->resample(g_audio_render_obj, g_clip->sounds + i);

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
    g_audio_render->release(g_audio_render_obj);
    if (g_clip)
        lvgCloseClip(g_clip);
    g_render->release(g_render_obj);
    lvgZipClose(&g_zip);
    glfwTerminate();
    SDL_Quit();
    return 0;
}
