#include "avm1.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <platform/platform.h>

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
    "Stage", &g_stage_members[0], 0, sizeof(g_stage_members)/sizeof(g_stage_members[0]), 0
};

static void math_abs(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, fabs(va));
}

static void math_acos(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, acos(va));
}

static void math_asin(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, asin(va));
}

static void math_atan(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, atan(va));
}

static void math_atan2(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(ctx, se_a);
    double vb = to_double(ctx, se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, atan2(va, vb));
}

static void math_ceil(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, ceil(va));
}

static void math_cos(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, cos(va));
}

static void math_exp(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, exp(va));
}

static void math_floor(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, floor(va));
}

static void math_log(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, exp(va));
}

static void math_max(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(ctx, se_a);
    double vb = to_double(ctx, se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, fmax(va, vb));
}

static void math_min(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(ctx, se_a);
    double vb = to_double(ctx, se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, fmin(va, vb));
}

static void math_pow(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr++;
    double va = to_double(ctx, se_a);
    double vb = to_double(ctx, se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, pow(va, vb));
}

static void math_random(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, (double)rand()/(double)RAND_MAX);
}

static void math_round(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, floor(va + 0.5));
}

static void math_sin(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, sin(va));
}

static void math_sqrt(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
    SET_DOUBLE(se_a, sin(va));
}

static void math_tan(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(ctx, se_a);
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
    "Math", &g_math_members[0], 0, sizeof(g_math_members)/sizeof(g_math_members[0]), 0
};

static void string_length(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    assert(1 == nargs);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    const char *str = (const char *)cls;
    SET_INT(res, strlen(str));
}

static void string_substr(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    assert(2 == nargs);
    ASVal *se_start = &ctx->stack[ctx->stack_ptr];
    //ASVal *se_len = se_start + 1;
    ctx->stack_ptr += 1;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    uint32_t start = to_int(se_start);
    //uint32_t len = to_int(se_len);
    const char *str = (const char *)cls;
    uint32_t max_len = strlen(str);
    if (start > max_len)
        start = max_len;
    SET_STRING(res, str + start); // TODO: allocate new string and use len
}

ASMember g_string_members[] =
{
    // properties
    { "length",       { { .fn = string_length }, ASVAL_NATIVE_FN } },
    // methods
    { "charAt",       { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(index: Number) : String
    { "charCodeAt",   { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(index: Number) : Number
    { "concat",       { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(value: Object) : String
    { "fromCharCode", { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : String
    { "indexOf",      { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(value: String, [startIndex: Number]) : Number
    { "lastIndexOf",  { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(value: String, [startIndex: Number]) : Number
    { "slice",        { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(start: Number, end: Number) : String
    { "split",        { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(delimiter: String, [limit: Number]) : Array
    { "substr",       { { .fn = string_substr }, ASVAL_NATIVE_FN } }, //(start: Number, length: Number) : String
    { "substring",    { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(start: Number, end: Number) : String
    { "toLowerCase",  { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : String
    { "toString",     { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : String
    { "toUpperCase",  { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : String
    { "valueOf",      { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : String
};

ASClass g_string =
{
    "String", &g_string_members[0], 0, sizeof(g_string_members)/sizeof(g_string_members[0]), 0
};

ASMember g_number_members[] =
{
    // properties
    { "MAX_VALUE",         { { .d_int = 0 }, ASVAL_DOUBLE } }, //: Number
    { "MIN_VALUE",         { { .d_int = 0 }, ASVAL_DOUBLE } }, //: Number
    { "NaN",               { { .d_int = NAN }, ASVAL_DOUBLE } }, //: Number
    { "NEGATIVE_INFINITY", { { .d_int = -INFINITY }, ASVAL_DOUBLE } }, //: Number
    { "POSITIVE_INFINITY", { { .d_int =  INFINITY }, ASVAL_DOUBLE } }, //: Number
    // methods
    { "toString",          { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(radix: Number) : String
    { "valueOf",           { { .fn = 0 }, ASVAL_NATIVE_FN } }  //() : Number
};

ASClass g_number =
{
    "Number", &g_number_members[0], 0, sizeof(g_number_members)/sizeof(g_number_members[0]), 0
};

ASMember g_key_members[] =
{
    // properties
    { "BACKSPACE",  { { .ui32 = 8 }, ASVAL_INT } }, //: Number
    { "CAPSLOCK",   { { .ui32 = 20 }, ASVAL_INT } }, //: Number
    { "CONTROL",    { { .ui32 = 17 }, ASVAL_INT } }, //: Number
    { "DELETEKEY",  { { .ui32 = 46 }, ASVAL_INT } }, //: Number
    { "DOWN",       { { .ui32 = 40 }, ASVAL_INT } }, //: Number
    { "END",        { { .ui32 = 35 }, ASVAL_INT } }, //: Number
    { "ENTER",      { { .ui32 = 13 }, ASVAL_INT } }, //: Number
    { "ESCAPE",     { { .ui32 = 27 }, ASVAL_INT } }, //: Number
    { "HOME",       { { .ui32 = 36 }, ASVAL_INT } }, //: Number
    { "INSERT",     { { .ui32 = 45 }, ASVAL_INT } }, //: Number
    { "LEFT",       { { .ui32 = 37 }, ASVAL_INT } }, //: Number
    { "_listeners", { { .ui32 = 0 }, ASVAL_INT } }, //: Array [read-only]
    { "PGDN",       { { .ui32 = 34 }, ASVAL_INT } }, //: Number
    { "PGUP",       { { .ui32 = 33 }, ASVAL_INT } }, //: Number
    { "RIGHT",      { { .ui32 = 39 }, ASVAL_INT } }, //: Number
    { "SHIFT",      { { .ui32 = 16 }, ASVAL_INT } }, //: Number
    { "SPACE",      { { .ui32 = 32 }, ASVAL_INT } }, //: Number
    { "TAB",        { { .ui32 = 9 }, ASVAL_INT } }, //: Number
    { "UP",         { { .ui32 = 38 }, ASVAL_INT } }, //: Number
    // methods
    { "addListener",    { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(listener: Object) : Void
    { "getAscii",       { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "getCode",        { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : Number
    { "isAccessible",   { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : Boolean
    { "isDown",         { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(code: Number) : Boolean
    { "isToggled",      { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(code: Number) : Boolean
    { "removeListener", { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(listener: Object) : Boolean
    // events
    { "onKeyDown", { { .fn = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onKeyUp",   { { .fn = 0 }, ASVAL_FUNCTION } }  // = function() {}
};

ASClass g_key =
{
    "Key", &g_key_members[0], 0, sizeof(g_key_members)/sizeof(g_key_members[0]), 0
};

ASMember g_object_members[] =
{
    // properties
    { "constructor",  { { .fn = 0 }, ASVAL_NATIVE_FN } }, //: Object
    { "__proto__",    { { .fn = 0 }, ASVAL_NATIVE_FN } }, //: Object
    { "prototype",    { { .fn = 0 }, ASVAL_NATIVE_FN } }, //: Object
    { "__resolve",    { { .fn = 0 }, ASVAL_NATIVE_FN } }, //: Object
    // methods
    { "addProperty",    { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(name: String, getter: Function, setter: Function) : Boolean
    { "hasOwnProperty", { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(name: String) : Boolean
    { "isPropertyEnumerable", { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(name: String) : Boolean
    { "isPrototypeOf",  { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(theClass: Object) : Boolean
    { "registerClass",  { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(name: String, theClass: Function) : Boolean
    { "toString",       { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : String
    { "unwatch",        { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(name: String) : Boolean
    { "valueOf",        { { .fn = 0 }, ASVAL_NATIVE_FN } }, //() : Object
    { "watch",          { { .fn = 0 }, ASVAL_NATIVE_FN } }, //(name: String, callback: Function, [userData: Object]) : Boolean
};

ASClass g_object =
{
    "Object", &g_object_members[0], 0, sizeof(g_object_members)/sizeof(g_object_members[0]), 0
};

ASVal g_classes[] =
{
    { { .cls = &g_object }, ASVAL_CLASS },
    { { .cls = &g_stage }, ASVAL_CLASS },
    { { .cls = &g_math }, ASVAL_CLASS },
    { { .cls = &g_number }, ASVAL_CLASS },
    { { .cls = &g_key }, ASVAL_CLASS }
};

int g_num_classes = sizeof(g_classes)/sizeof(g_classes[0]);

static void createEmptyMovieClip(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ASVal *se_name = &ctx->stack[ctx->stack_ptr];
#ifdef _DEBUG
    ASVal *se_depth = se_name + 1;
    assert(ASVAL_INT == se_depth->type || ASVAL_DOUBLE == se_depth->type || ASVAL_FLOAT == se_depth->type);
#endif
    assert(ASVAL_STRING == se_name->type);
    ASVal *loc = create_local(ctx, THIS, se_name->str);
    loc->type = ASVAL_UNDEFINED;
    loc->str  = 0;
    ctx->stack_ptr += nargs - 1;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    *res = *loc;
}

static void getBytesLoaded(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, 100.0);
}

static void do_gotoAndPlay(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    assert(1 == nargs);
    ASVal *se_frame = &ctx->stack[ctx->stack_ptr];
    assert(ASVAL_INT == se_frame->type || ASVAL_DOUBLE == se_frame->type || ASVAL_FLOAT == se_frame->type || ASVAL_STRING == se_frame->type);
    LVGMovieClipGroupState *groupstate = ctx->clip->groupstates + (size_t)cls->priv;
    LVGMovieClipGroup *group = ctx->clip->groups + groupstate->group_num;
    uint32_t frame = -1;
    if (ASVAL_STRING == se_frame->type)
    {
        LVGFrameLabel *l = group->labels;
        for (int i = 0; i < group->num_labels; i++)
            if (0 == strcasecmp(l[i].name, se_frame->str))
            {
                frame = l[i].frame_num;
                break;
            }
        if (frame == (uint32_t)-1)
            return;
    } else
        frame = to_int(se_frame);
    groupstate->cur_frame = frame % group->num_frames;
}

static void gotoAndPlay(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    do_gotoAndPlay(ctx, cls, a, nargs);
    LVGMovieClipGroupState *groupstate = ctx->clip->groupstates + (size_t)cls->priv;
    groupstate->play_state = LVG_PLAYING;
    handle_frame_change(ctx, groupstate);
}

static void gotoAndStop(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    do_gotoAndPlay(ctx, cls, a, nargs);
    LVGMovieClipGroupState *groupstate = ctx->clip->groupstates + (size_t)cls->priv;
    groupstate->play_state = LVG_STOPPED;
    handle_frame_change(ctx, groupstate);
}

static void play(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_UNDEF(res);
    LVGMovieClipGroupState *groupstate = ctx->clip->groupstates + (size_t)cls->priv;
    groupstate->play_state = LVG_PLAYING;
    handle_frame_change(ctx, groupstate);
}

static void stop(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    ctx->stack_ptr--;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_UNDEF(res);
    LVGMovieClipGroupState *groupstate = ctx->clip->groupstates + (size_t)cls->priv;
    groupstate->play_state = LVG_STOPPED;
    handle_frame_change(ctx, groupstate);
}

ASMember g_movieclip_members[] =
{
    // properties
    { "_alpha",        { { .d_int = 1.0 }, ASVAL_DOUBLE } }, //Number
    { "blendMode",     { { "normal" }, ASVAL_STRING } }, //Object
    { "cacheAsBitmap", { { "" }, ASVAL_STRING } }, //Boolean
    { "_currentframe", { { .ui32 = 0 }, ASVAL_INT } }, //Number [read-only]
    { "_droptarget",   { { "" }, ASVAL_STRING } }, //String [read-only]
    { "enabled",       { { "" }, ASVAL_STRING } }, //Boolean
    { "filters",       { { "" }, ASVAL_STRING } }, //Array
    { "focusEnabled",  { { "" }, ASVAL_STRING } }, //Boolean
    { "_focusrect",    { { "" }, ASVAL_STRING } }, //Boolean
    { "forceSmoothing", { { "" }, ASVAL_STRING } }, //Boolean
    { "_framesloaded", { { .ui32 = 0 }, ASVAL_INT } }, //Number [read-only]
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
    { "_totalframes",  { { .ui32 = 0 }, ASVAL_INT } }, //Number [read-only]
    { "trackAsMenu",   { { "" }, ASVAL_STRING } }, //Boolean
    { "transform" ,    { { "" }, ASVAL_STRING } }, //Transform
    { "_url",          { { "" }, ASVAL_STRING } }, //String [read-only]
    { "useHandCursor", { { "" }, ASVAL_STRING } }, //Boolean
    { "_visible",      { { .boolean = 1 }, ASVAL_BOOL } }, //Boolean
    { "_width",        { { "" }, ASVAL_STRING } }, //Number
    { "_x",            { { "" }, ASVAL_STRING } }, //Number
    { "_xmouse",       { { "" }, ASVAL_STRING } }, //Number [read-only]
    { "_xscale",       { { .i32 = 100 }, ASVAL_INT } }, //Number
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
    "MovieClip", &g_movieclip_members[0], 0, sizeof(g_movieclip_members)/sizeof(g_movieclip_members[0]), 0
};

ASMember g_button_members[] =
{
    // properties
    { "_alpha",        { { .d_int = 1.0 }, ASVAL_DOUBLE } }, //: Number
    { "blendMode",     { { "normal" }, ASVAL_STRING } }, //: Object
    { "cacheAsBitmap", { { "" }, ASVAL_STRING } }, //: Boolean
    { "enabled",       { { "" }, ASVAL_STRING } }, //: Boolean
    { "filters",       { { "" }, ASVAL_STRING } }, //: Array
    { "_focusrect",    { { "" }, ASVAL_STRING } }, //: Boolean
    { "_height",       { { "" }, ASVAL_STRING } }, //: Number
    { "_highquality",  { { "" }, ASVAL_STRING } }, //: Number
    { "menu",          { { "" }, ASVAL_STRING } }, //: ContextMenu
    { "_name",         { { "" }, ASVAL_STRING } }, //: String
    { "_parent",       { { "" }, ASVAL_STRING } }, //: MovieClip
    { "_quality",      { { "" }, ASVAL_STRING } }, //: String
    { "_rotation",     { { "" }, ASVAL_STRING } }, //: Number
    { "scale9Grid",    { { "" }, ASVAL_STRING } }, //: Rectangle
    { "_soundbuftime", { { "" }, ASVAL_STRING } }, //: Number
    { "tabEnabled",    { { "" }, ASVAL_STRING } }, //: Boolean
    { "tabIndex",      { { "" }, ASVAL_STRING } }, //: Number
    { "_target",       { { "" }, ASVAL_STRING } }, //: String [read-only]
    { "trackAsMenu",   { { "" }, ASVAL_STRING } }, //: Boolean
    { "_url",          { { "" }, ASVAL_STRING } }, //: String [read-only]
    { "useHandCursor", { { "" }, ASVAL_STRING } }, //: Boolean
    { "_visible",      { { .boolean = 1 }, ASVAL_BOOL } }, //: Boolean
    { "_width",        { { "" }, ASVAL_STRING } }, //: Number
    { "_x",            { { "" }, ASVAL_STRING } }, //: Number
    { "_xmouse",       { { "" }, ASVAL_STRING } }, //: Number [read-only]
    { "_xscale",       { { "" }, ASVAL_STRING } }, //: Number
    { "_y",            { { "" }, ASVAL_STRING } }, //: Number
    { "_ymouse",       { { "" }, ASVAL_STRING } }, //: Number [read-only]
    { "_yscale",       { { "" }, ASVAL_STRING } }, //: Number
    // events
    { "onDragOut",        { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onDragOver",       { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onKeyDown",        { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onKeyUp",          { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onKillFocus",      { { .str = 0 }, ASVAL_FUNCTION } }, // = function(newFocus: Object) {}
    { "onPress",          { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onRelease",        { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onReleaseOutside", { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onRollOut",        { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onRollOver",       { { .str = 0 }, ASVAL_FUNCTION } }, // = function() {}
    { "onSetFocus",       { { .str = 0 }, ASVAL_FUNCTION } }, // = function(oldFocus: Object) {}
    // methods
    { "getDepth",         { { .ui32 = 0 }, ASVAL_INT } } //() : Number
};

ASClass g_button =
{
    "Button", &g_button_members[0], 0, sizeof(g_button_members)/sizeof(g_button_members[0]), 0
};

static void setInterval(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    assert(2 == nargs);
    ASVal *se_func = &ctx->stack[ctx->stack_ptr];
    ASVal *se_timeout = se_func + 1;
    ctx->stack_ptr += (int32_t)nargs - 1;
    assert(ASVAL_FUNCTION == se_func->type);
    assert(ASVAL_INT == se_timeout->type || ASVAL_DOUBLE == se_timeout->type || ASVAL_FLOAT == se_timeout->type);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (ctx->groupstate->num_timers > 10)
    {
        SET_INT(res, 0);
        return;
    }
    ctx->groupstate->timers = realloc(ctx->groupstate->timers, (ctx->groupstate->num_timers + 1)*sizeof(ctx->groupstate->timers[0]));
    LVGTimer *t = ctx->groupstate->timers + ctx->groupstate->num_timers++;
    t->func = (uint8_t *)se_func->str;
    t->last_time = g_params.frame_time;
    t->timeout = to_double(ctx, se_timeout);
    static int counter = 0;
    t->id = ++counter;
    SET_INT(res, t->id);
}

static void clearInterval(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs)
{
    assert(1 == nargs);
    ASVal *se_id = &ctx->stack[ctx->stack_ptr];
    assert(ASVAL_INT == se_id->type || ASVAL_DOUBLE == se_id->type || ASVAL_FLOAT == se_id->type);
    int id = to_int(se_id);
    for (int i = 0; i < ctx->groupstate->num_timers; i++)
        if (ctx->groupstate->timers[i].id == id)
        {
            memmove(&ctx->groupstate->timers[i], &ctx->groupstate->timers[i + 1], (ctx->groupstate->num_timers - i - 1)*sizeof(ctx->groupstate->timers[0]));
            ctx->groupstate->num_timers--;
            return;
        }
    assert(0);
}

ASMember g_properties[] =
{
    { "this", { { .str = 0 }, ASVAL_CLASS } },
    { "_root", { { .str = 0 }, ASVAL_CLASS } },
    { "_level0", { { .str = 0 }, ASVAL_CLASS } },
    { "_accProps", { { .str = 0 }, ASVAL_CLASS } },
    { "_focusrect", { { .str = 0 }, ASVAL_CLASS } },
    { "_global", { { .str = 0 }, ASVAL_CLASS } },
    { "_highquality", { { .str = 0 }, ASVAL_CLASS } },
    { "_level", { { .str = 0 }, ASVAL_CLASS } },
    { "maxscroll", { { .str = 0 }, ASVAL_CLASS } },
    { "_parent", { { .str = 0 }, ASVAL_CLASS } },
    { "_quality", { { .str = 0 }, ASVAL_CLASS } },
    { "scroll", { { .str = 0 }, ASVAL_CLASS } },
    { "_soundbuftime", { { .str = 0 }, ASVAL_CLASS } },
    // math
    { "Infinity", { { .d_int = INFINITY }, ASVAL_DOUBLE } },
    { "NaN", { { .d_int = NAN }, ASVAL_DOUBLE } },
    // global functions
    { "setInterval", { { .fn = setInterval }, ASVAL_NATIVE_FN } },
    { "clearInterval", { { .fn = clearInterval }, ASVAL_NATIVE_FN } }
};

int g_num_properties = sizeof(g_properties)/sizeof(g_properties[0]);
