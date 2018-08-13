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
#include <stdlib.h>
#include <math.h>
#ifdef __MINGW32__
#include <windows/mman.h>
#else
#include <sys/mman.h>
#endif
#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "lunzip.h"
#define STBI_NO_STDIO
#include "stb_image.h"
#include <render/gl.h>
#include <video/video.h>
#include <audio/audio.h>
#include <render/render.h>
#include <platform/platform.h>
#include "lvg.h"
#include "swf/avm1.h"
#include <scripting/scripting.h>

LVGMovieClip *g_clip;
static zip_t g_zip;
LVGColorf g_bgColor;
platform_params g_params;
static int b_no_actionscript, b_fullscreen, b_interpolate;
#ifdef EMSCRIPTEN
static int is_gles3;
#endif

#if ENABLE_VIDEO && VIDEO_FFMPEG
extern const video_dec ff_decoder;
#endif
#if RENDER_NANOVG
extern const render nvg_render;
#endif
#if RENDER_NVPR
extern const render nvpr_render;
#endif
extern const render null_render;
const render *g_render;
void *g_render_obj;

#if AUDIO_SDL
extern const audio_render sdl_audio_render;
#endif
extern const audio_render null_audio_render;
const audio_render *g_audio_render;
void *g_audio_render_obj;

#if PLATFORM_GLFW
extern const platform glfw_platform;
#endif
#if PLATFORM_SDL || (ENABLE_AUDIO && AUDIO_SDL)
extern const platform sdl_platform;
#endif
const platform *g_platform;
void *g_platform_obj;

stbi_uc *stbi__resample_row_hv_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_row(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step);
#if defined(__x86_64__) || defined(_M_X64)
stbi_uc *stbi__resample_row_hv_2_simd(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_simd(stbi_uc *out, stbi_uc const *y, stbi_uc const *pcb, stbi_uc const *pcr, int count, int step);
#endif

#if ENABLE_SCRIPT
#if SCRIPT_PICOC
extern const script_engine script_engine_picoc;
#define SCRIPT_ENGINE script_engine_picoc
#endif
void *g_script;
#endif

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

LVGShapeCollection *lvgLoadShape(const char *file)
{
    char *buf;
    double time = g_platform->get_time(g_platform_obj);
    if (!(buf = lvgGetFileContents(file, 0)))
    {
        printf("error: could not open SVG image.\n");
        return 0;
    }
    double time2 = g_platform->get_time(g_platform_obj);
    printf("zip time: %fs\n", time2 - time);
    NSVGimage *image = nsvgParse(buf, "px", 96.0f);
    free(buf);
    // convert to LVGShapeCollection
    int i;
    LVGShapeCollection *col = calloc(1, sizeof(LVGShapeCollection));
    NSVGshape *shape = image->shapes;
    col->num_shapes = 1;
    for (; shape->next; col->num_shapes++, shape = shape->next);
    col->shapes = malloc(sizeof(NSVGshape)*col->num_shapes);
    shape = image->shapes;
    memcpy(col->bounds, shape->bounds, sizeof(col->bounds));
    for (i = 0; i < col->num_shapes; i++)
    {
        memcpy(col->shapes + i, shape, sizeof(NSVGshape));
        if (shape->bounds[0] < col->bounds[0])
            col->bounds[0] = shape->bounds[0];
        if (shape->bounds[2] < col->bounds[2])
            col->bounds[2] = shape->bounds[2];
        if (shape->bounds[1] > col->bounds[1])
            col->bounds[1] = shape->bounds[1];
        if (shape->bounds[3] > col->bounds[3])
            col->bounds[3] = shape->bounds[3];
        g_render->cache_shape(g_render_obj, col->shapes + i);
        NSVGshape *to_free = shape;
        shape = shape->next;
        free(to_free);
    }

    time = g_platform->get_time(g_platform_obj);
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
    return col;
}

void lvgGetShapeBounds(LVGShapeCollection *col, double *bounds)
{   // picoc do not support floats - convert to doubles
    bounds[0] = col->bounds[0];
    bounds[1] = col->bounds[1];
    bounds[2] = col->bounds[2];
    bounds[3] = col->bounds[3];
}

static void lvgDrawShapeCol(LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode)
{
    g_render->render_shape(g_render_obj, shapecol, cxform, ratio, blend_mode);
}

void lvgDrawShape(LVGShapeCollection *svg)
{
    lvgDrawShapeCol(svg, 0, 0.0f, BLEND_REPLACE);
}

static void combine_cxform(LVGColorTransform *newcxform, LVGColorTransform *cxform, double alpha)
{
    newcxform->add[0] += cxform->add[0]; newcxform->add[1] += cxform->add[1]; newcxform->add[2] += cxform->add[2]; newcxform->add[3] += cxform->add[3];
    newcxform->mul[0] *= cxform->mul[0]; newcxform->mul[1] *= cxform->mul[1]; newcxform->mul[2] *= cxform->mul[2]; newcxform->mul[3] *= cxform->mul[3]*alpha;
}

static void lvgDrawClipGroup(LVGMovieClip *clip, LVGMovieClipGroupState *groupstate, LVGColorTransform *cxform, double r, int next_frame, int blend_mode)
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
                    ASClass *cls = create_instance(0, &g_movieclip);
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
                    ASClass *cls = create_instance(0, &g_movieclip);
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
                        ASClass *cls = create_instance(0, &g_button);
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
            double time = g_params.time - t->last_time;
            if (time > t->timeout/1000.0)
            {
                t->last_time = g_params.time;
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
        int ratio = o->ratio;
#ifdef LVG_INTERPOLATE
        float o_t[6];
        if (b_interpolate && o->interpolate_obj)
        {
            LVGObject *inter = o->interpolate_obj;
            double omr = 1.0 - r;
            ratio = round((double)ratio*omr + (double)inter->ratio*r);
            for (int idx = 0; idx < 6; idx++)
                o_t[idx] = o->t[idx]*omr + inter->t[idx]*r;
            g_render->set_transform(g_render_obj, o_t, 0);
        } else
#endif
        g_render->set_transform(g_render_obj, o->t, 0);
        if (LVG_OBJ_SHAPE == o->type && visible)
        {
            LVGColorTransform newcxform = *cxform;
            combine_cxform(&newcxform, &o->cxform, alpha);
            lvgDrawShapeCol(&clip->shapes[o->id], &newcxform, ratio/65535.0f, blend_mode ? blend_mode : o->blend_mode);
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
            lvgDrawClipGroup(clip, clip->groupstates + o->id, &newcxform, r, next_frame, o->blend_mode);
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
                float m[2] = { g_params.mx, g_params.my };
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
            if (mouse_hit && (g_params.mkeys & 1))
                state_flags = DOWN_SHAPE;
            int flags = 0, keypressed = (g_params.mkeys & MOUSE_BUTTON_LEFT), waspressed = (g_params.last_mkeys & MOUSE_BUTTON_LEFT);
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
                    assert(LVG_OBJ_SHAPE == bs->obj.type || LVG_OBJ_GROUP == bs->obj.type || LVG_OBJ_BUTTON == bs->obj.type || LVG_OBJ_TEXT == bs->obj.type);
                    if (LVG_OBJ_SHAPE != bs->obj.type)
                        continue;
                    g_render->set_transform(g_render_obj, bs->obj.t, 0);
                    LVGColorTransform newcxform = *cxform;
                    combine_cxform(&newcxform, &o->cxform, 1.0);
                    combine_cxform(&newcxform, &bs->obj.cxform, alpha*btn_alpha);
                    lvgDrawShapeCol(&clip->shapes[bs->obj.id], &newcxform, bs->obj.ratio/65535.0f, blend_mode);
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
#ifdef LVG_INTERPOLATE
                if (b_interpolate && o->interpolate_obj)
                    g_render->set_transform(g_render_obj, o_t, 0);
                else
#endif
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
                    lvgDrawShapeCol(shapecol, &newcxform, 0.0f, blend_mode);
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
    double r, diff = g_params.time - clip->last_time;
    if (diff > 1.0/clip->fps)
    {
        next_frame = 1;
        clip->last_time += 1.0/clip->fps;
        if ((g_params.time - clip->last_time) > 1.0/clip->fps)
            r = 0.0;
        else
            r = 1.0;
    } else
        r = diff/(1.0/clip->fps);
#else
    double r = 1;
    int next_frame = 1;
#endif
    LVGColorTransform startcxform;
    memset(&startcxform, 0, sizeof(startcxform));
    startcxform.mul[0] = startcxform.mul[1] = startcxform.mul[2] = startcxform.mul[3] = 1.0f;
    //printf_frames(clip, clip->groupstates); printf("\n"); fflush(stdout);
    lvgDrawClipGroup(clip, clip->groupstates, &startcxform, r, next_frame, BLEND_REPLACE);
}

static void deletePaint(NSVGpaint* paint)
{
    if (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT)
    {
        g_render->free_image(g_render_obj, paint->gradient->cache);
        free(paint->gradient);
    }
}

static void lvgFreeNSVGShape(NSVGshape *shape)
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

static void lvgFreeShape_internal(LVGShapeCollection *shape)
{
    int i;
    for (i = 0; i < shape->num_shapes; i++)
        lvgFreeNSVGShape(shape->shapes + i);
    free(shape->shapes);
    if (shape->morph)
    {
        for (i = 0; i < shape->morph->num_shapes; i++)
            lvgFreeNSVGShape(shape->morph->shapes + i);
        if (shape->morph->shapes)
            free(shape->morph->shapes);
        free(shape->morph);
    }
}

void lvgFreeShape(LVGShapeCollection *shape)
{
    lvgFreeShape_internal(shape);
    free(shape);
}

void lvgFreeClip(LVGMovieClip *clip)
{
    int i, j;
    if (!clip)
        return;
    for (i = 0; i < clip->num_shapes; i++)
    {
        lvgFreeShape_internal(clip->shapes + i);
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
        if (group->frames)
            free(group->frames);
        if (group->labels)
            free(group->labels);
        if (group->ssounds)
            free(group->ssounds);
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
        if (text->strings)
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
        if (video->frames)
            free(video->frames);
        if (video->image)
            g_render->free_image(g_render_obj, video->image);
        if (video->vdec)
            ff_decoder.release(video->vdec);
    }
    for (i = 0; i < clip->num_buttons; i++)
    {
        LVGButton *btn = clip->buttons + i;
        for (j = 0; j < btn->num_btnactions; j++)
        {
            if (btn->btnactions[j].actions)
                free(btn->btnactions[j].actions);
        }
        if (btn->btnactions)
            free(btn->btnactions);
        if (btn->btn_shapes)
            free(btn->btn_shapes);
        if (btn->button_obj)
            free_instance(btn->button_obj);
    }
    if (clip->vm)
    {
        lvgFreeVM(clip->vm);
        free(clip->vm);
    }
    if (clip->shapes)
        free(clip->shapes);
    if (clip->images)
        free(clip->images);
    if (clip->groups)
        free(clip->groups);
    if (clip->fonts)
        free(clip->fonts);
    if (clip->texts)
        free(clip->texts);
    if (clip->sounds)
        free(clip->sounds);
    if (clip->videos)
        free(clip->videos);
    if (clip->buttons)
        free(clip->buttons);
    free(clip);
}

platform_params *lvgGetParams()
{
    return &g_params;
}

void lvgTranslate(float x, float y)
{
    float t[6];
    Transform3x2 tr;
    translate(tr, x, y);
    from_transform3x2(t, tr);
    g_render->set_transform(g_render_obj, t, 0);
}

void lvgScale(float x, float y)
{
    float t[6];
    Transform3x2 tr;
    scale(tr, x, y);
    from_transform3x2(t, tr);
    g_render->set_transform(g_render_obj, t, 0);
}

void drawframe()
{
    g_platform->pull_events(g_platform_obj);
    glViewport(0, 0, g_params.width, g_params.height);
    glClearColor(g_bgColor.r, g_bgColor.g, g_bgColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    int pressed = (g_params.mkeys & MOUSE_BUTTON_LEFT) && !(g_params.last_mkeys & MOUSE_BUTTON_LEFT);
    if (pressed)
    {
        static double last_click;
        if (g_params.time - last_click < 0.2)
            goto change_fullscreen;
        last_click = g_params.time;
    }
    if (g_platform->get_key(g_platform_obj, KEY_LEFTALT) || g_platform->get_key(g_platform_obj, KEY_RIGHTALT))
    {
        static int last_enter;
        int enter_state = g_platform->get_key(g_platform_obj, KEY_ENTER);
        if (enter_state && !last_enter)
        {
change_fullscreen:
            b_fullscreen = !b_fullscreen;
            g_platform->fullscreen(g_platform_obj, b_fullscreen);
        }
        last_enter = enter_state;
    }

    if (g_clip)
    {
        g_render->begin_frame(g_render_obj, g_clip->bounds[2] - g_clip->bounds[0], g_clip->bounds[3] - g_clip->bounds[1], g_params.winWidth, g_params.winHeight, g_params.width, g_params.height);
        lvgDrawClip(g_clip);
    } else
    {
        g_render->begin_frame(g_render_obj, 800, 600, g_params.winWidth, g_params.winHeight, g_params.width, g_params.height);
        if (g_script)
            SCRIPT_ENGINE.run_function(g_script, "onFrame");
    }

    g_render->end_frame(g_render_obj);
    g_platform->swap_buffers(g_platform_obj);
    g_params.last_mkeys = g_params.mkeys;
}

int open_lvg(const char *file_name)
{
    size_t size;
    char *map = lvgOpenMap(file_name, &size);
    if (!map)
        return -1;
    if (0 == lvgZipOpen(map, size, &g_zip))
    {
#ifdef EMSCRIPTEN
        char *buf;
        if ((buf = lvgGetFileContents("features", 0)))
        {
            is_gles3 = NULL != strstr(buf, "gles3");
            free(buf);
        }
#endif
#if ENABLE_SCRIPT
        int ret = SCRIPT_ENGINE.init(&g_script, "main.c");
        if (g_script)
            SCRIPT_ENGINE.run_function(g_script, "onInit");
        return ret;
#endif
    } else if ((g_clip = lvgLoadSWFBuf(map, size, 0)))
    {
        munmap(map, size);
        if (!g_clip)
            return -1;
        g_bgColor = g_clip->bgColor;
        return 0;
    }
    munmap(map, size);
    return -1;
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
        case 'f': b_fullscreen = 1; break;
        case 'i': b_interpolate = 1; break;
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
        printf("usage: lvg .swf or .lvg file to play\n");
        return 0;
#endif
    } else
        file_name = argv[i];
#ifdef _TEST
    g_render = &null_render;
    g_audio_render = &null_audio_render;
    if (open_lvg(file_name))
    {
        printf("error: could not open swf file\n");
        return -1;
    }
    for (int i = 0; i < 10; i++)
        lvgDrawClip(g_clip);
    lvgFreeClip(g_clip);
    lvgZipClose(&g_zip);
    return 0;
#else
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

#if PLATFORM_GLFW
    g_platform = &glfw_platform;
#endif
#if PLATFORM_SDL
    g_platform = &sdl_platform;
#endif
#if ENABLE_AUDIO && AUDIO_SDL && !PLATFORM_SDL
    void *audio_platform_obj;
    sdl_platform.init(&audio_platform_obj, 0, 1);
#endif
    if (!g_platform->init(&g_platform_obj, &g_params, 0))
    {
        printf("error: could not open platform\n");
        return 1;
    }
    if (b_fullscreen)
        g_platform->fullscreen(g_platform_obj, b_fullscreen);

#ifndef EMSCRIPTEN
    g_render = &nvpr_render;
    if (!g_render->init(&g_render_obj, g_platform))
#endif
    {
        g_render = &nvg_render;
        if (!g_render->init(&g_render_obj, g_platform))
        {
            printf("error: could not open render\n");
            return -1;
        }
    }

#if ENABLE_AUDIO && AUDIO_SDL
    g_audio_render = &sdl_audio_render;
    if (!g_audio_render->init(&g_audio_render_obj, 44100, 2, 0, 0, 0))
        g_audio_render = &null_audio_render;
#else
    g_audio_render = &null_audio_render;
#endif

    if (open_lvg(file_name))
    {
        printf("error: could not open lvg or swf file\n");
        return -1;
    }

    g_platform->main_loop(g_platform_obj);

    g_audio_render->release(g_audio_render_obj);
    if (g_clip)
        lvgFreeClip(g_clip);
    g_render->release(g_render_obj);
    lvgZipClose(&g_zip);
    g_platform->release(g_platform_obj);
#if ENABLE_AUDIO && AUDIO_SDL && !PLATFORM_SDL
    sdl_platform.release(audio_platform_obj);
#endif
#if ENABLE_SCRIPT
    if (g_script)
        SCRIPT_ENGINE.release(g_script);
#endif
#endif
    return 0;
}
