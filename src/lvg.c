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

#define STBI_NO_STDIO
#include <stb_image.h>
#include <lvg.h>
#include <swf/avm1.h>
#include <scripting/scripting.h>

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

#if AUDIO_SDL
extern const audio_render sdl_audio_render;
#endif
extern const audio_render null_audio_render;

#if PLATFORM_GLFW
extern const platform glfw_platform;
#endif
#if PLATFORM_SDL || (ENABLE_AUDIO && AUDIO_SDL)
extern const platform sdl_platform;
#endif

#if ENABLE_SCRIPT
#if SCRIPT_PICOC
extern const script_engine script_engine_picoc;
#define SCRIPT_ENGINE script_engine_picoc
#endif
#endif

stbi_uc *stbi__resample_row_hv_2(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_row(stbi_uc *out, const stbi_uc *y, const stbi_uc *pcb, const stbi_uc *pcr, int count, int step);
#if defined(__x86_64__) || defined(_M_X64)
stbi_uc *stbi__resample_row_hv_2_simd(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far, int w, int hs);
void stbi__YCbCr_to_RGB_simd(stbi_uc *out, stbi_uc const *y, stbi_uc const *pcb, stbi_uc const *pcr, int count, int step);
#endif

char *lvgGetFileContents(LVGEngine *e, const char *fname, uint32_t *size)
{
    uint32_t idx;
    if ((idx = lvgZipNameLocate(&e->zip, fname)) != (int32_t)-1)
        return lvgZipDecompress(&e->zip, idx, size);
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

void lvgViewport(LVGEngine *e, int width, int heigth)
{
    e->render->begin_frame(e->render_obj, width, heigth, e->params.winWidth, e->params.winHeight, e->params.width, e->params.height);
}

int lvgImageLoad(LVGEngine *e, const char *file)
{
    char *buf;
    uint32_t size;
    if (!(buf = lvgGetFileContents(e, file, &size)))
    {
        printf("error: could not open file: %s\n", file);
        return 0;
    }
    int image = lvgImageLoadBuf(e, (unsigned char *)buf, size);
    free(buf);
    return image;
}

int lvgImageLoadBuf(LVGEngine *e, const unsigned char *buf, uint32_t buf_size)
{
    int w, h, n;
    unsigned char *img = stbi_load_from_memory(buf, buf_size, &w, &h, &n, 4);
    int image = e->render->cache_image(e->render_obj, w, h, 0, (const unsigned char *)img);
    free(img);
    return image;
}

void lvgImageFree(LVGEngine *e, int image)
{
    e->render->free_image(e->render_obj, image);
}

LVGShapeCollection *lvgShapeLoad(LVGEngine *e, const char *file)
{
    char *buf;
    double time = e->platform->get_time(e->platform_obj);
    if (!(buf = lvgGetFileContents(e, file, 0)))
    {
        printf("error: could not open file: %s\n", file);
        return 0;
    }
    double time2 = e->platform->get_time(e->platform_obj);
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
        e->render->cache_shape(e->render_obj, col->shapes + i);
        NSVGshape *to_free = shape;
        shape = shape->next;
        free(to_free);
    }

    time = e->platform->get_time(e->platform_obj);
    printf("svg load time: %fs\n", time - time2);
    return col;
}

void lvgShapeGetBounds(LVGShapeCollection *col, double *bounds)
{   // picoc do not support floats - convert to doubles
    bounds[0] = col->bounds[0];
    bounds[1] = col->bounds[1];
    bounds[2] = col->bounds[2];
    bounds[3] = col->bounds[3];
}

static void lvgShapeDrawCol(LVGEngine *e, LVGShapeCollection *shapecol, LVGColorTransform *cxform, float ratio, int blend_mode)
{
    e->render->render_shape(e->render_obj, shapecol, cxform, ratio, blend_mode);
}

void lvgShapeDraw(LVGEngine *e, LVGShapeCollection *svg)
{
    lvgShapeDrawCol(e, svg, 0, 0.0f, BLEND_REPLACE);
}

int lvgVideoDecodeToFrame(LVGEngine *e, LVGVideo *video, int frame)
{
#if ENABLE_VIDEO && VIDEO_FFMPEG
    if ((!frame || frame != video->cur_frame) && frame < video->num_frames)
    {
        if (!video->vdec)
            ff_decoder.init(&video->vdec, video->codec);
        video_frame out;
        out.planes[0] = NULL;
        if (!frame)
            video->cur_frame = -1;
        for (;video->cur_frame < frame;)
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
            e->render->update_image(e->render_obj, video->image, img);
            free(img);
        }
    }
#endif
    return video->image;
}

static void lvgVideoFree_internal(LVGEngine *e, LVGVideo *video)
{
    int i;
    for (i = 0; i < video->num_frames; i++)
    {
        if (video->frames[i].data)
            free(video->frames[i].data);
    }
    if (video->frames)
        free(video->frames);
    if (video->image)
        e->render->free_image(e->render_obj, video->image);
    if (video->vdec)
        ff_decoder.release(video->vdec);
}

void lvgVideoFree(LVGEngine *e, LVGVideo *video)
{
    lvgVideoFree_internal(e, video);
    free(video);
}

static void combine_cxform(LVGColorTransform *newcxform, LVGColorTransform *cxform, double alpha)
{
    newcxform->add[0] += cxform->add[0]; newcxform->add[1] += cxform->add[1]; newcxform->add[2] += cxform->add[2]; newcxform->add[3] += cxform->add[3];
    newcxform->mul[0] *= cxform->mul[0]; newcxform->mul[1] *= cxform->mul[1]; newcxform->mul[2] *= cxform->mul[2]; newcxform->mul[3] *= cxform->mul[3]*alpha;
}

static void lvgClipDrawGroup(LVGEngine *e, LVGMovieClip *clip, LVGMovieClipGroupState *groupstate, LVGColorTransform *cxform, double r, int next_frame, int blend_mode)
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
                if (!e->b_no_actionscript)
                {
                    if (gs->movieclip)
                        free_instance(gs->movieclip);
                    if (gs->timers)
                        free(gs->timers);
                }
                memset(gs, 0, sizeof(LVGMovieClipGroupState));
                if (!e->b_no_actionscript)
                {
                    ASClass *cls = create_instance(0, &g_movieclip);
                    gs->movieclip = cls;
                    cls->priv = (void*)(size_t)o->id;
                }
                gs->group_num = group_num;
            }
        }
    }
    if (!e->b_no_actionscript)
    {
        if (!clip->vm)
        {
            clip->vm = malloc(sizeof(LVGActionCtx));
            lvgInitVM(clip->vm, e, clip);
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
            double time = e->params.time - t->last_time;
            if (time > t->timeout/1000.0)
            {
                t->last_time = e->params.time;
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
        e->render->get_transform(e->render_obj, save_transform);
        int ratio = o->ratio;
#ifdef LVG_INTERPOLATE
        float o_t[6];
        if (e->b_interpolate && o->interpolate_obj)
        {
            LVGObject *inter = o->interpolate_obj;
            double omr = 1.0 - r;
            ratio = round((double)ratio*omr + (double)inter->ratio*r);
            for (int idx = 0; idx < 6; idx++)
                o_t[idx] = o->t[idx]*omr + inter->t[idx]*r;
            e->render->set_transform(e->render_obj, o_t, 0);
        } else
#endif
        e->render->set_transform(e->render_obj, o->t, 0);
        if (LVG_OBJ_SHAPE == o->type && visible)
        {
            LVGColorTransform newcxform = *cxform;
            combine_cxform(&newcxform, &o->cxform, alpha);
            lvgShapeDrawCol(e, &clip->shapes[o->id], &newcxform, ratio/65535.0f, blend_mode ? blend_mode : o->blend_mode);
        } else
        if (LVG_OBJ_IMAGE == o->type && visible)
        {
            e->render->render_image(e->render_obj, clip->images[o->id]);
        } else
        if (LVG_OBJ_VIDEO == o->type && visible)
        {
            LVGVideo *video = &clip->videos[o->id];
            lvgVideoDecodeToFrame(e, video, o->ratio);
            e->render->render_image(e->render_obj, video->image);
        } else
        if (LVG_OBJ_GROUP == o->type)
        {
            LVGColorTransform newcxform = *cxform;
            combine_cxform(&newcxform, &o->cxform, alpha);
            lvgClipDrawGroup(e, clip, clip->groupstates + o->id, &newcxform, r, next_frame, o->blend_mode);
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
            e->render->get_transform(e->render_obj, save_t);
            for (j = 0; j < b->num_btn_shapes; j++)
            {
                LVGButtonState *bs = b->btn_shapes + j;
                if (!(bs->flags & HIT_SHAPE))
                    continue;
                assert(LVG_OBJ_SHAPE == bs->obj.type);
                if (LVG_OBJ_SHAPE != bs->obj.type)
                    continue;
                float t[6];
                e->render->set_transform(e->render_obj, bs->obj.t, 0);
                e->render->get_transform(e->render_obj, t);
                Transform3x2 tr;
                tr[0][0] = t[0]; tr[1][0] = t[1];
                tr[0][1] = t[2]; tr[1][1] = t[3];
                tr[0][2] = t[4]; tr[1][2] = t[5];
                inverse(tr, tr);
                float m[2] = { e->params.mx, e->params.my };
                xform(m, tr, m);
                LVGShapeCollection *col = &clip->shapes[bs->obj.id];
                for (int k = 0; k < col->num_shapes; k++)
                {
                    NSVGshape *s = col->shapes + k;
                    float x  = s->bounds[0], y  = s->bounds[1];
                    float x1 = s->bounds[2], y1 = s->bounds[3];
                    if (m[0] >= x && m[0] <=x1 && m[1] >= y && m[1] <= y1)
                    {
                        if (e->render->inside_shape)
                            mouse_hit = e->render->inside_shape(e->render_obj, s, m[0], m[1]);
                        else
                            mouse_hit = 1;
                        if (mouse_hit)
                            break;
                    }
                }
                e->render->set_transform(e->render_obj, save_t, 1);
            }
            int state_flags = UP_SHAPE;
            if (mouse_hit)
                state_flags = OVER_SHAPE;
            if (mouse_hit && (e->params.mkeys & 1))
                state_flags = DOWN_SHAPE;
            int flags = 0, keypressed = (e->params.mkeys & MOUSE_BUTTON_LEFT), waspressed = (e->params.last_mkeys & MOUSE_BUTTON_LEFT);
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
                    e->render->set_transform(e->render_obj, bs->obj.t, 0);
                    LVGColorTransform newcxform = *cxform;
                    combine_cxform(&newcxform, &o->cxform, 1.0);
                    combine_cxform(&newcxform, &bs->obj.cxform, alpha*btn_alpha);
                    lvgShapeDrawCol(e, &clip->shapes[bs->obj.id], &newcxform, bs->obj.ratio/65535.0f, blend_mode);
                    e->render->set_transform(e->render_obj, save_t, 1);
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
                e->render->set_transform(e->render_obj, save_transform, 1);
#ifdef LVG_INTERPOLATE
                if (e->b_interpolate && o->interpolate_obj)
                    e->render->set_transform(e->render_obj, o_t, 0);
                else
#endif
                e->render->set_transform(e->render_obj, o->t, 0);
                e->render->set_transform(e->render_obj, text->t, 0);
                float scale = str->height/50.0f;
                if (3 == f->version)
                    scale /= 20.0f;
                float t[6] = { scale, 0.0f, 0.0f, scale, str->x, str->y };
                e->render->set_transform(e->render_obj, t, 0);
                for (int k = 0; k < str->num_chars; k++)
                {
                    LVGChar *c = str->chars + k;
                    LVGShapeCollection *shapecol = &clip->shapes[f->glyphs[c->idx]];
                    LVGColorTransform newcxform = *cxform;
                    combine_cxform(&newcxform, &o->cxform, alpha);
                    for (int l = 0; l < shapecol->num_shapes; l++)
                        shapecol->shapes[l].fill.color = str->color;
                    lvgShapeDrawCol(e, shapecol, &newcxform, 0.0f, blend_mode);
                    float t[6] = { 1.0f, 0.0f, 0.0f, 1.0f, c->x_advance/20.0f/scale, 0.0f };
                    e->render->set_transform(e->render_obj, t, 0);
                }
            }
        }
        e->render->set_transform(e->render_obj, save_transform, 1);
    }
    if (next_frame && LVG_PLAYING == groupstate->play_state && cur_frame == groupstate->cur_frame/*not changed by as*/)
        groupstate->cur_frame = (groupstate->cur_frame + 1) % group->num_frames;
    if (!e->b_no_actionscript)
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

void lvgClipDraw(LVGEngine *e, LVGMovieClip *clip)
{
#ifndef _TEST
    int next_frame = 0;
    double r, diff = e->params.time - clip->last_time;
    if (diff > 1.0/clip->fps)
    {
        next_frame = 1;
        clip->last_time += 1.0/clip->fps;
        if ((e->params.time - clip->last_time) > 1.0/clip->fps)
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
    lvgClipDrawGroup(e, clip, clip->groupstates, &startcxform, r, next_frame, BLEND_REPLACE);
}

static void deletePaint(LVGEngine *e, NSVGpaint *paint)
{
    if (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT)
    {
        e->render->free_image(e->render_obj, paint->gradient->cache);
        free(paint->gradient);
    }
}

static void lvgFreeNSVGShape(LVGEngine *e, NSVGshape *shape)
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
    deletePaint(e, &shape->fill);
    deletePaint(e, &shape->stroke);
}

static void lvgShapeFree_internal(LVGEngine *e, LVGShapeCollection *shape)
{
    int i;
    for (i = 0; i < shape->num_shapes; i++)
        lvgFreeNSVGShape(e, shape->shapes + i);
    free(shape->shapes);
    if (shape->morph)
    {
        for (i = 0; i < shape->morph->num_shapes; i++)
            lvgFreeNSVGShape(e, shape->morph->shapes + i);
        if (shape->morph->shapes)
            free(shape->morph->shapes);
        free(shape->morph);
    }
}

void lvgShapeFree(LVGEngine *e, LVGShapeCollection *shape)
{
    lvgShapeFree_internal(e, shape);
    free(shape);
}

void lvgClipFree(LVGEngine *e, LVGMovieClip *clip)
{
    int i, j;
    if (!clip)
        return;
    for (i = 0; i < clip->num_shapes; i++)
    {
        lvgShapeFree_internal(e, clip->shapes + i);
    }
    for (i = 0; i < clip->num_images; i++)
    {
        e->render->free_image(e->render_obj, clip->images[i]);
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
        lvgVideoFree_internal(e, clip->videos + i);
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
    if (clip->groupstates)
        free(clip->groupstates);
    free(clip);
}

platform_params *lvgGetParams(LVGEngine *e)
{
    return &e->params;
}

void lvgTranslate(LVGEngine *e, float x, float y)
{
    float t[6];
    Transform3x2 tr;
    translate(tr, x, y);
    from_transform3x2(t, tr);
    e->render->set_transform(e->render_obj, t, 0);
}

void lvgScale(LVGEngine *e, float x, float y)
{
    float t[6];
    Transform3x2 tr;
    scale(tr, x, y);
    from_transform3x2(t, tr);
    e->render->set_transform(e->render_obj, t, 0);
}

void drawframe(LVGEngine *e)
{
    e->platform->pull_events(e->platform_obj);
    glViewport(0, 0, e->params.width, e->params.height);
    glClearColor(e->bgColor.r, e->bgColor.g, e->bgColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    int pressed = (e->params.mkeys & MOUSE_BUTTON_LEFT) && !(e->params.last_mkeys & MOUSE_BUTTON_LEFT);
    if (pressed)
    {
        if (e->params.time - e->last_click < 0.2)
            goto change_fullscreen;
        e->last_click = e->params.time;
    }
    if (e->platform->get_key(e->platform_obj, KEY_LEFTALT) || e->platform->get_key(e->platform_obj, KEY_RIGHTALT))
    {
        int enter_state = e->platform->get_key(e->platform_obj, KEY_ENTER);
        if (enter_state && !e->last_enter)
        {
change_fullscreen:
            e->b_fullscreen = !e->b_fullscreen;
            e->platform->fullscreen(e->platform_obj, e->b_fullscreen);
        }
        e->last_enter = enter_state;
    }

    if (e->clip)
    {
        e->render->begin_frame(e->render_obj, e->clip->bounds[2] - e->clip->bounds[0], e->clip->bounds[3] - e->clip->bounds[1], e->params.winWidth, e->params.winHeight, e->params.width, e->params.height);
        lvgClipDraw(e, e->clip);
    } else
    {
        e->render->begin_frame(e->render_obj, 800, 600, e->params.winWidth, e->params.winHeight, e->params.width, e->params.height);
        if (e->script)
            if (SCRIPT_ENGINE.run_function(e->script, "onFrame"))
                e->platform->set_exit(e->platform_obj);
    }

    e->render->end_frame(e->render_obj);
    e->platform->swap_buffers(e->platform_obj);
    e->params.last_mkeys = e->params.mkeys;
}

int lvg_init(LVGEngine *e)
{
#if defined(EMSCRIPTEN)
    if (e->b_gles3)
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
    e->platform = &glfw_platform;
#endif
#if PLATFORM_SDL
    e->platform = &sdl_platform;
#endif
#if ENABLE_AUDIO && AUDIO_SDL && !PLATFORM_SDL
    void *audio_platform_obj;
    sdl_platform.init(&audio_platform_obj, 0, 0, 0, 1);
#endif
    if (!e->platform->init(&e->platform_obj, &e->params, (void (*)(void *))drawframe, e, 0))
    {
        printf("error: could not open platform\n");
        return -1;
    }
    if (e->b_fullscreen)
        e->platform->fullscreen(e->platform_obj, e->b_fullscreen);

#ifndef EMSCRIPTEN
    e->render = &nvpr_render;
    if (!e->render->init(&e->render_obj, e->platform))
#endif
    {
        e->render = &nvg_render;
        if (!e->render->init(&e->render_obj, e->platform))
        {
            printf("error: could not open render\n");
            return -1;
        }
    }

#if ENABLE_AUDIO && AUDIO_SDL
    e->audio_render = &sdl_audio_render;
    if (!e->audio_render->init(&e->audio_render_obj, 44100, 2, 0, 0, 0))
        e->audio_render = &null_audio_render;
#else
    e->audio_render = &null_audio_render;
#endif
    return 0;
}

void lvg_close(LVGEngine *e)
{
    e->audio_render->release(e->audio_render_obj);
    if (e->clip)
        lvgClipFree(e, e->clip);
    e->render->release(e->render_obj);
    lvgZipClose(&e->zip);
    e->platform->release(e->platform_obj);
#if ENABLE_AUDIO && AUDIO_SDL && !PLATFORM_SDL
    sdl_platform.release(audio_platform_obj);
#endif
#if ENABLE_SCRIPT
    if (e->script)
        SCRIPT_ENGINE.release(e->script);
#endif
}

int lvg_open(LVGEngine *e, const char *file_name)
{
    size_t size;
    char *map = lvgOpenMap(file_name, &size);
    if (!map)
        return -1;
    if (0 == lvgZipOpen(map, size, &e->zip))
    {
#ifdef EMSCRIPTEN
        char *buf;
        if ((buf = lvgGetFileContents(e, "features", 0)))
        {
            e->b_gles3 = NULL != strstr(buf, "gles3");
            free(buf);
        }
#endif
#if ENABLE_SCRIPT
        if (!SCRIPT_ENGINE.init(e, &e->script, "main.c"))
        {
            if (e->script)
                if (!SCRIPT_ENGINE.run_function(e->script, "onInit"))
                    return 0;
        }
#endif
    } else if ((e->clip = lvgClipLoadBuf(e, map, size, 0)))
    {
        munmap(map, size);
        if (!e->clip)
            return -1;
        e->bgColor = e->clip->bgColor;
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
    LVGEngine engine;
    LVGEngine *e = &engine;
    memset(e, 0, sizeof(*e));
    // check switches
    int i;
    for(i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            break;
        switch (argv[i][1])
        {
        case 'n': e->b_no_actionscript = 1; break;
        case 'f': e->b_fullscreen = 1; break;
        case 'i': e->b_interpolate = 1; break;
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
    e->render = &null_render;
    e->audio_render = &null_audio_render;
    if (lvg_open(e, file_name))
    {
        printf("error: could not open swf file\n");
        return -1;
    }
    for (int i = 0; i < 10; i++)
        lvgClipDraw(e, e->clip);
    lvgClipFree(e, e->clip);
    lvgZipClose(&e->zip);
    return 0;
#else
    if (lvg_init(e))
        return -1;

    if (lvg_open(e, file_name))
    {
        printf("error: could not open lvg or swf file\n");
        return -1;
    }

    e->platform->main_loop(e->platform_obj);

    lvg_close(e);
#endif
    return 0;
}
