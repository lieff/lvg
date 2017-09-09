#include "avm1.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <strings.h>

ASMember g_stage_members[] =
{
    // properties
    { "align",        { { "" }, ASVAL_STRING } },
    { "displayState", { { "" }, ASVAL_STRING } },
    { "fullScreenSourceRect", { { .str = 0 }, ASVAL_CLASS } },
    { "height",       { { .ui32 = 150 }, ASVAL_INT } },
    { "scaleMode",    { { "showAll" }, ASVAL_STRING } },
    { "showMenu",     { { .boolean = 0 }, ASVAL_BOOL } },
    { "width",        { { .ui32 = 200 }, ASVAL_INT } },
    // events
    { "onFullScreen",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onResize",       { { .str = 0 }, ASVAL_FUNCTION } },
    // static methods
    { "addListener",    { { .str = 0 }, ASVAL_NATIVE_FN } },
    { "removeListener", { { .str = 0 }, ASVAL_NATIVE_FN } },
};

ASClass g_stage =
{
    "Stage", &g_stage_members[0], sizeof(g_stage_members)/sizeof(g_stage_members[0])
};

static void math_abs(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, fabs(va));
}

static void math_acos(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, acos(va));
}

static void math_asin(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, asin(va));
}

static void math_atan(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, atan(va));
}

static void math_atan2(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, atan2(vb, va));
}

static void math_ceil(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, ceil(va));
}

static void math_cos(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, cos(va));
}

static void math_exp(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, exp(va));
}

static void math_floor(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, floor(va));
}

static void math_log(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, exp(va));
}

static void math_max(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, fmax(vb, va));
}

static void math_min(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, fmin(vb, va));
}

static void math_pow(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, pow(vb, va));
}

static void math_random(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, (double)rand()/(double)RAND_MAX);
}

static void math_round(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, floor(va + 0.5));
}

static void math_sin(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, sin(va));
}

static void math_sqrt(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, sin(va));
}

static void math_tan(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    SET_DOUBLE(se_a, sin(va));
}

ASMember g_math_members[] =
{
    // properties
    { "E",       { { .d_int = M_E }, ASVAL_DOUBLE } },
    { "LN10",    { { .d_int = M_LN10 }, ASVAL_DOUBLE } },
    { "LN2",     { { .d_int = M_LN2 }, ASVAL_DOUBLE } },
    { "LOG10E",  { { .d_int = M_LOG10E }, ASVAL_DOUBLE } },
    { "LOG2E",   { { .d_int = M_LOG2E }, ASVAL_DOUBLE } },
    { "PI",      { { .d_int = M_PI }, ASVAL_DOUBLE } },
    { "SQRT1_2", { { .d_int = M_SQRT1_2 }, ASVAL_DOUBLE } },
    { "SQRT2",   { { .d_int = M_SQRT2 }, ASVAL_DOUBLE } },
    // static methods
    { "abs",     { { .fn = math_abs }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "acos",    { { .fn = math_acos }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "asin",    { { .fn = math_asin }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "atan",    { { .fn = math_atan }, ASVAL_NATIVE_FN } }, //(tangent:Number) : Number
    { "atan2",   { { .fn = math_atan2 }, ASVAL_NATIVE_FN } }, //(y:Number, x:Number) : Number
    { "ceil",    { { .fn = math_ceil }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "cos",     { { .fn = math_cos }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "exp",     { { .fn = math_exp }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "floor",   { { .fn = math_floor }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "log",     { { .fn = math_log }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "max",     { { .fn = math_max }, ASVAL_NATIVE_FN } }, //(x:Number, y:Number) : Number
    { "min",     { { .fn = math_min }, ASVAL_NATIVE_FN } }, //(x:Number, y:Number) : Number
    { "pow",     { { .fn = math_pow }, ASVAL_NATIVE_FN } }, //(x:Number, y:Number) : Number
    { "random",  { { .fn = math_random }, ASVAL_NATIVE_FN } }, //() : Number
    { "round",   { { .fn = math_round }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "sin",     { { .fn = math_sin }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "sqrt",    { { .fn = math_sqrt }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
    { "tan",     { { .fn = math_tan }, ASVAL_NATIVE_FN } }, //(x:Number) : Number
};

ASClass g_math =
{
    "Math", &g_math_members[0], sizeof(g_math_members)/sizeof(g_math_members[0])
};

ASVal g_classes[] =
{
    { { .cls = &g_stage }, ASVAL_CLASS },
    { { .cls = &g_math }, ASVAL_CLASS }
};

int g_num_classes = sizeof(g_classes)/sizeof(g_classes[0]);

static void createEmptyMovieClip(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ASVal *se_name = &ctx->stack[ctx->stack_ptr];
    ASVal *se_depth = se_name + 1;
    assert(ASVAL_INT == se_depth->type || ASVAL_DOUBLE == se_depth->type || ASVAL_FLOAT == se_depth->type);
    assert(ASVAL_STRING == se_name->type);
    ASVal *loc = create_local(ctx, se_name->str);
    loc->type = ASVAL_UNDEFINED;
    loc->str  = 0;
    ctx->stack_ptr += nargs - 1;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    *res = *loc;
}

static void getBytesLoaded(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, 100.0);
}

extern LVGMovieClip *g_clip;

static void gotoAndPlay(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    assert(1 == nargs);
    ASVal *se_frame = &ctx->stack[ctx->stack_ptr];
    assert(ASVAL_INT == se_frame->type || ASVAL_DOUBLE == se_frame->type || ASVAL_FLOAT == se_frame->type || ASVAL_STRING == se_frame->type);
    LVGMovieClipGroup *group = g_clip->groups;
    if (ASVAL_STRING == se_frame->type)
    {
        LVGFrameLabel *l = group->labels;
        for (int i = 0; i < group->num_labels; i++)
            if (0 == strcasecmp(l[i].name, se_frame->str))
            {
                group->cur_frame = l[i].frame_num % group->num_frames;
                group->play_state = LVG_PLAYING;
                return;
            }
    }
    uint32_t frame = to_int(se_frame);
    group->cur_frame = frame % group->num_frames;
    group->play_state = LVG_PLAYING;
}

static void gotoAndStop(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    gotoAndPlay(ctx, a, nargs);
    g_clip->groups->play_state = LVG_STOPPED;
}

static void play(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_UNDEF(res);
    g_clip->groups->play_state = LVG_PLAYING;
}

static void stop(LVGActionCtx *ctx, uint8_t *a, int nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_UNDEF(res);
    g_clip->groups->play_state = LVG_STOPPED;
}

ASMember g_movieclip_members[] =
{
    // properties
    { "_alpha",        { { "" }, ASVAL_STRING } }, //Number
    { "blendMode",     { { "" }, ASVAL_STRING } }, //Object
    { "cacheAsBitmap", { { "" }, ASVAL_STRING } }, //Boolean
    { "_currentframe", { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_droptarget",   { { "" }, ASVAL_STRING } }, //String [read-only]
    { "enabled",       { { "" }, ASVAL_STRING } }, //Boolean
    { "filters",       { { "" }, ASVAL_STRING } }, //Array
    { "focusEnabled",  { { "" }, ASVAL_STRING } }, //Boolean
    { "_focusrect",    { { "" }, ASVAL_STRING } }, //Boolean
    { "forceSmoothing", { { "" }, ASVAL_STRING } }, //Boolean
    { "_framesloaded", { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_height",       { { "" }, ASVAL_STRING } }, //Number
    { "_highquality",  { { "" }, ASVAL_STRING } }, //Number
    { "hitArea",       { { "" }, ASVAL_STRING } }, //Object
    { "_lockroot",     { { "" }, ASVAL_STRING } }, //Boolean
    { "menu",          { { "" }, ASVAL_STRING } }, //ContextMenu
    { "_name",         { { "" }, ASVAL_STRING } }, //String
    { "opaqueBackground", { { "" }, ASVAL_STRING } }, //Number
    { "_parent",       { { "" }, ASVAL_STRING } }, //MovieClip
    { "_quality",      { { "" }, ASVAL_STRING } }, //String
    { "_rotation",     { { "" }, ASVAL_STRING } }, //Number
    { "scale9Grid",    { { "" }, ASVAL_STRING } }, //Rectangle
    { "scrollRect",    { { "" }, ASVAL_STRING } }, //Object
    { "_soundbuftime", { { "" }, ASVAL_STRING } }, //Number
    { "tabChildren",   { { "" }, ASVAL_STRING } }, //Boolean
    { "tabEnabled",    { { "" }, ASVAL_STRING } }, //Boolean
    { "tabIndex",      { { "" }, ASVAL_STRING } }, //Number
    { "_target",       { { "" }, ASVAL_STRING } }, //String [read-only]
    { "_totalframes",  { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "trackAsMenu",   { { "" }, ASVAL_STRING } }, //Boolean
    { "transform" ,    { { "" }, ASVAL_STRING } }, //Transform
    { "_url",          { { "" }, ASVAL_STRING } }, //String [read-only]
    { "useHandCursor", { { "" }, ASVAL_STRING } }, //Boolean
    { "_visible",      { { "" }, ASVAL_STRING } }, //Boolean
    { "_width",        { { "" }, ASVAL_STRING } }, //Number
    { "_x",            { { "" }, ASVAL_STRING } }, //Number
    { "_xmouse",       { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_xscale",       { { "" }, ASVAL_STRING } }, //Number
    { "_y",            { { "" }, ASVAL_STRING } }, //Number
    { "_ymouse",       { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_yscale",       { { "" }, ASVAL_STRING } }, //Number
    // events
    { "onData",        { { .str = 0 }, ASVAL_FUNCTION } },
    { "onDragOut",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onDragOver",    { { .str = 0 }, ASVAL_FUNCTION } },
    { "onEnterFrame",  { { .str = 0 }, ASVAL_FUNCTION } },
    { "onKeyDown",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onKeyUp",       { { .str = 0 }, ASVAL_FUNCTION } },
    { "onKillFocus",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onLoad",        { { .str = 0 }, ASVAL_FUNCTION } },
    { "onMouseDown",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onMouseMove",   { { .str = 0 }, ASVAL_FUNCTION } },
    { "onMouseUp",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onPress",       { { .str = 0 }, ASVAL_FUNCTION } },
    { "onRelease",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onReleaseOutside", { { .str = 0 }, ASVAL_FUNCTION } },
    { "onRollOut",     { { .str = 0 }, ASVAL_FUNCTION } },
    { "onRollOver",    { { .str = 0 }, ASVAL_FUNCTION } },
    { "onSetFocus",    { { .str = 0 }, ASVAL_FUNCTION } },
    { "onUnload",      { { .str = 0 }, ASVAL_FUNCTION } },
    // methods
    { "attachAudio",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //(id: Object) : Void
    { "attachBitmap",  { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bmp: BitmapData, depth: Number, [pixelSnapping: String], [smoothing: Boolean]) : Void
    { "attachMovie",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //(id: String, name: String, depth: Number, [initObject: Object]) : MovieClip
    { "beginBitmapFill", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bmp: BitmapData, [matrix: Matrix], [repeat: Boolean], [smoothing: Boolean]) : Void
    { "beginFill",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(rgb: Number, [alpha: Number]) : Void
    { "beginGradientFill", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(fillType: String, colors: Array, alphas: Array, ratios: Array, matrix: Object, [spreadMethod: String], [interpolationMethod: String], [focalPointRatio: Number]) : Void
    { "clear",         { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "createEmptyMovieClip", { { .fn = createEmptyMovieClip }, ASVAL_NATIVE_FN } }, //(name: String, depth: Number) : MovieClip
    { "createTextField", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(instanceName: String, depth: Number, x: Number, y: Number, width: Number, height: Number) : TextField
    { "curveTo",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //(controlX: Number, controlY: Number, anchorX: Number, anchorY: Number) : Void
    { "duplicateMovieClip", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(name: String, depth: Number, [initObject: Object]) : MovieClip
    { "endFill",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "getBounds",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bounds: Object) : Object
    { "getBytesLoaded", { { .fn = getBytesLoaded }, ASVAL_NATIVE_FN } }, //() : Number
    { "getBytesTotal", { { .fn = getBytesLoaded }, ASVAL_NATIVE_FN } }, //() : Number
    { "getDepth",      { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getInstanceAtDepth", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(depth: Number) : MovieClip
    { "getNextHighestDepth", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getRect",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //(bounds: Object) : Object
    { "getSWFVersion", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getTextSnapshot", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : TextSnapshot
    { "getURL",        { { .str = 0 }, ASVAL_NATIVE_FN } }, //(url: String, [window: String], [method: String]) : Void
    { "globalToLocal", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(pt: Object) : Void
    { "gotoAndPlay",   { { .fn = gotoAndPlay }, ASVAL_NATIVE_FN } }, //(frame: Object) : Void
    { "gotoAndStop",   { { .fn = gotoAndStop }, ASVAL_NATIVE_FN } }, //(frame: Object) : Void
    { "hitTest",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Boolean
    { "lineGradientStyle", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(fillType: String, colors: Array, alphas: Array, ratios: Array, matrix: Object, [spreadMethod: String], [interpolationMethod: String], [focalPointRatio: Number]) : Void
    { "lineStyle",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(thickness: Number, rgb: Number, alpha: Number, pixelHinting: Boolean, noScale: String, capsStyle: String, jointStyle: String, miterLimit: Number) : Void
    { "lineTo",        { { .str = 0 }, ASVAL_NATIVE_FN } }, //(x: Number, y: Number) : Void
    { "loadMovie",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //(url: String, [method: String]) : Void
    { "loadVariables", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(url: String, [method: String]) : Void
    { "localToGlobal", { { .str = 0 }, ASVAL_NATIVE_FN } }, //(pt: Object) : Void
    { "moveTo",        { { .str = 0 }, ASVAL_NATIVE_FN } }, //(x: Number, y: Number) : Void
    { "nextFrame",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "play",          { { .fn = play }, ASVAL_NATIVE_FN } }, //() : Void
    { "prevFrame",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "removeMovieClip", { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "setMask",       { { .str = 0 }, ASVAL_NATIVE_FN } }, //(mc: Object) : Void
    { "startDrag",     { { .str = 0 }, ASVAL_NATIVE_FN } }, //([lockCenter: Boolean], [left: Number], [top: Number], [right: Number], [bottom: Number]) : Void
    { "stop",          { { .fn = stop }, ASVAL_NATIVE_FN } }, //() : Void
    { "stopDrag",      { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
    { "swapDepths",    { { .str = 0 }, ASVAL_NATIVE_FN } }, //(target: Object) : Void
    { "unloadMovie",   { { .str = 0 }, ASVAL_NATIVE_FN } }, //() : Void
};

ASClass g_movieclip =
{
    "MovieClip", &g_movieclip_members[0], sizeof(g_movieclip_members)/sizeof(g_movieclip_members[0])
};

ASMember g_properties[] =
{
    { "this", { { .str = (void*)&g_movieclip }, ASVAL_CLASS } },
    { "_root", { { .str = (void*)&g_movieclip }, ASVAL_CLASS } },
    { "_accProps", { { .str = 0 }, ASVAL_CLASS } },
    { "_focusrect", { { .str = 0 }, ASVAL_CLASS } },
    { "_global", { { .str = 0 }, ASVAL_CLASS } },
    { "_highquality", { { .str = 0 }, ASVAL_CLASS } },
    { "_level", { { .str = 0 }, ASVAL_CLASS } },
    { "maxscroll", { { .str = 0 }, ASVAL_CLASS } },
    { "_parent", { { .str = 0 }, ASVAL_CLASS } },
    { "_quality", { { .str = 0 }, ASVAL_CLASS } },
    { "scroll", { { .str = 0 }, ASVAL_CLASS } },
    { "_soundbuftime", { { .str = 0 }, ASVAL_CLASS } }
};

int g_num_properties = sizeof(g_properties)/sizeof(g_properties[0]);
