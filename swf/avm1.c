#include "avm1.h"
#include <../render/render.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifdef _DEBUG
#define DBG(n, m) n, m,
#define DBG_BREAK raise(SIGTRAP)
#else
#define DBG(n, m)
#define DBG_BREAK
#endif

#define STACK_SIZE 4096

typedef enum {
    STACK_STRING, STACK_FLOAT, STACK_DOUBLE, STACK_BOOL, STACK_INT, STACK_NULL, STACK_UNDEFINED, STACK_CLASS, STACK_FUNCTION
} StackValType;

typedef struct StackEntry
{
    union {
        const char *str;
        float f_int;
        double d_int;
        uint32_t i32;
        int boolean;
    };
    StackValType type;
} StackEntry;

typedef struct LVGActionCtx
{
    LVGMovieClip *clip;
    LVGMovieClipGroup *group;
    LVGMovieClipFrame *frame;
    StackEntry stack[STACK_SIZE];
    StackEntry regs[256];
    const char **cpool;
    int stack_ptr, cpool_size, pc;
} LVGActionCtx;

static void action_end(LVGActionCtx *ctx, LVGAction *a)
{
}

static void action_next_frame(LVGActionCtx *ctx, LVGAction *a)
{
    ctx->group->cur_frame = (ctx->group->cur_frame + 1) % ctx->group->num_frames;
}

static void action_previous_frame(LVGActionCtx *ctx, LVGAction *a)
{
    if (ctx->group->cur_frame)
        ctx->group->cur_frame--;
}

static void action_play(LVGActionCtx *ctx, LVGAction *a)
{
    ctx->group->play_state = LVG_PLAYING;
}

static void action_stop(LVGActionCtx *ctx, LVGAction *a)
{
    ctx->group->play_state = LVG_STOPPED;
}

static void action_quality(LVGActionCtx *ctx, LVGAction *a)
{   // ignore
}

static void action_stop_sounds(LVGActionCtx *ctx, LVGAction *a)
{
    if (a->sdata)
    {
        lvgPlaySound(ctx->clip->sounds + a->sdata - 1);
    } else
        lvgStopAudio();
}
static void action_add(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_sub(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_mul(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_div(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_old_eq(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_old_less(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_and(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_or(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_not(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_string_compare_eq(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_string_length(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_string_extract(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_pop(LVGActionCtx *ctx, LVGAction *a)
{
    ctx->stack_ptr++;
    assert(ctx->stack_ptr < sizeof(ctx->stack)/sizeof(ctx->stack[0]));
}
static void action_to_integer(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_get_variable(LVGActionCtx *ctx, LVGAction *a)
{
    StackEntry *se = &ctx->stack[ctx->stack_ptr];
    assert(STACK_STRING == se->type);
    if (0 == strcmp(se->str, "Stage") || 0 == strcmp(se->str, "this"))
    {
        se->type = STACK_CLASS;
        se->str = 0;
    } else
    {
        assert(0);
    }
}
static void action_set_variable(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_set_target2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_string_add(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_get_property(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_set_property(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_clone_sprite(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_remove_sprite(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_trace(LVGActionCtx *ctx, LVGAction *a)
{
#ifdef _DEBUG
    StackEntry *se = &ctx->stack[ctx->stack_ptr];
    ctx->stack_ptr++;
    if (STACK_STRING == se->type)
        printf("%s\n", se->str);
    fflush(stdout);
#endif
}
static void action_start_drag(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_end_drag(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_string_compare_le(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_throw(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_cast(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_implements(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_random_number(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_mb_string_length(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_char_to_ascii(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_ascii_to_char(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_get_time(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_mb_string_extract(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_mb_char_to_ascii(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_mb_ascii_to_char(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_delete(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_delete2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_define_local(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_call_function(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_return(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_modulo(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_new_object(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_define_local2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_init_array(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_init_object(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_type_of(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_target_path(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_enumerate(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_add2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_less2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_equals2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_to_number(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_to_string(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_push_duplicate(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_swap(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_get_member(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_set_member(LVGActionCtx *ctx, LVGAction *a)
{
    StackEntry *se_val = &ctx->stack[ctx->stack_ptr];
    StackEntry *se_member = se_val + 1;
    StackEntry *se_var = se_val + 2;
    ctx->stack_ptr += 3;
    assert(STACK_CLASS == se_var->type && STACK_STRING == se_member->type);
    // ignore sets for now
}
static void action_increment(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_decrement(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_call_method(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_new_method(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_instance_of(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_enumerate2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_bitwise_and(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_bitwise_or(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_bitwise_xor(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_lshift(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_rshift(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_urshift(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_strict_equals(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_gt(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_string_compare_gt(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_extends(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_goto_frame(LVGActionCtx *ctx, LVGAction *a)
{
    ctx->group->cur_frame = a->sdata % ctx->group->num_frames;
}
static void action_get_url(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_store_register(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_constant_pool(LVGActionCtx *ctx, LVGAction *a)
{
    ctx->cpool_size = *(uint16_t*)a->data;
    ctx->cpool = realloc(ctx->cpool, ctx->cpool_size*sizeof(char *));
    const char *s = (const char *)a->data + 2;
    for (int i = 0; i < ctx->cpool_size; i++)
    {
        ctx->cpool[i] = s;
        while(*s++);
    }
}

static void action_wait_for_frame(LVGActionCtx *ctx, LVGAction *a)
{   // all frames always loaded - never skip actions
}

static void action_set_target(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_goto_label(LVGActionCtx *ctx, LVGAction *a)
{
    LVGFrameLabel *l = ctx->group->labels;
    const char *name = (const char *)a->data;
    for (int i = 0; i < ctx->group->num_labels; i++)
        if (0 == strcmp(name, l[i].name))
        {
            ctx->group->cur_frame = l[i].frame_num % ctx->group->num_frames;
            return;
        }
}
static void action_wait_for_frame2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_define_function2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_try(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_with(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_push(LVGActionCtx *ctx, LVGAction *a)
{
    int len = a->len;
    const char *data = (const char *)a->data;
    do {
        ctx->stack_ptr--;
        StackEntry *se = &ctx->stack[ctx->stack_ptr];
        assert(ctx->stack_ptr >= 0);
        int size = 0, type = *(uint8_t*)data;
        switch(type)
        {
        case 0: se->type = STACK_STRING; se->str = (const char*)data + 1; size = strlen(se->str) + 1; break;
        case 1: se->type = STACK_FLOAT; se->f_int = *(float*)((char*)data + 1); size = 4; break;
        case 2: se->type = STACK_NULL;
        case 3: se->type = STACK_UNDEFINED;
        case 4: { int reg = *((uint8_t*)data + 1); se->type = ctx->regs[reg].type; se->str = ctx->regs[reg].str; size = 1; } break;
        case 5: se->type = STACK_BOOL; se->boolean = *((uint8_t*)data + 1) ? 1 : 0; size = 1; break;
        case 6: se->type = STACK_DOUBLE; se->d_int = *(double*)((char*)data + 1); size = 8; break;
        case 7: se->type = STACK_INT; se->i32 = *(uint32_t*)((char*)data + 1); size = 4; break;
        case 8: se->type = STACK_STRING; se->str = ctx->cpool[*(uint8_t*)((char*)data + 1)]; size = 1; break;
        case 9: se->type = STACK_STRING; se->str = ctx->cpool[*(uint16_t*)((char*)data + 1)]; size = 2; break;
        default:
            assert(0);
            return;
        }
        len -= size + 1;
        data += size + 1;
    } while (len > 0);
}
static void action_jump(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_get_url2(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_define_function(LVGActionCtx *ctx, LVGAction *a)
{
    ctx->stack_ptr--;
    StackEntry *se = &ctx->stack[ctx->stack_ptr];
    se->type = STACK_FUNCTION;
    se->str = 0;

    int i = 0;
    const uint8_t *data = (const uint8_t *)a->data;
    //const char *fname = (const char *)data;
    while (data[i++]);
    int params = data[i++];
    params += data[i++]*256;
    for(int j = 0; j < params;j++)
    {
        while (data[i++]);
    }
    int codesize = data[i++];
    codesize += data[i++]*256;
    while (codesize)
    {
        a++; ctx->pc++;
#define ATAG_FULLLENGTH(a) (a->len + 1 + ((a->opcode & 0x80) ? 2 : 0))
        codesize -= ATAG_FULLLENGTH(a);
    }
}
static void action_if(LVGActionCtx *ctx, LVGAction *a) { DBG_BREAK; }
static void action_goto_frame2(LVGActionCtx *ctx, LVGAction *a)
{
    StackEntry *se = &ctx->stack[ctx->stack_ptr];
    ctx->stack_ptr++;
    assert(STACK_INT == se->type || STACK_STRING == se->type);
    int add = 0, flags = *(uint8_t*)a->data;
    ctx->group->play_state = (flags & 1) ? LVG_PLAYING : LVG_STOPPED;
    if (flags & 2)
        add = *(uint16_t*)((char*)a->data + 1);
    if (STACK_INT == se->type)
    {
        ctx->group->cur_frame = (se->i32 + add) % ctx->group->num_frames;
    } else if (STACK_STRING == se->type)
    {
        LVGFrameLabel *l = ctx->group->labels;
        for (int i = 0; i < ctx->group->num_labels; i++)
            if (0 == strcmp(se->str, l[i].name))
            {
                ctx->group->cur_frame = (l[i].frame_num + add) % ctx->group->num_frames;
                return;
            }
    }
}

typedef struct
{
    void      (*vm_func)(LVGActionCtx *ctx, LVGAction *a);
#ifdef _DEBUG
    const char *name;
    uint8_t   version;
#endif
    uint8_t   npop_params;
    uint8_t   npush_params;
} ActionEntry;

const ActionEntry g_avm1_actions[256] =
{
    /* version 1 */
    [ACTION_END]               = { action_end,               DBG("End", 1)             0, 0 },
    [ACTION_NEXT_FRAME]        = { action_next_frame,        DBG("NextFrame", 1)       0, 0 },
    [ACTION_PREVIOUS_FRAME]    = { action_previous_frame,    DBG("PreviousFrame", 1)   0, 0 },
    [ACTION_PLAY]              = { action_play,              DBG("Play", 1)            0, 0 },
    [ACTION_STOP]              = { action_stop,              DBG("Stop", 1)            0, 0 },
    [ACTION_TOGGLE_QUALITY]    = { action_quality,           DBG("ToggleQuality", 1)  -1, -1 },
    /* version 2 */
    [ACTION_STOP_SOUNDS]       = { action_stop_sounds,       DBG("StopSounds", 2)      0, 0 },
    /* version 4 */
    [ACTION_ADD]               = { action_add,               DBG("Add", 4)             2, 1 },
    [ACTION_SUBTRACT]          = { action_sub,               DBG("Subtract", 4)        2, 1 },
    [ACTION_MULTIPLY]          = { action_mul,               DBG("Multiply", 4)        2, 1 },
    [ACTION_DIVIDE]            = { action_div,               DBG("Divide", 4)          2, 1 },
    [ACTION_EQUALS]            = { action_old_eq,            DBG("Equals", 4)          2, 1 },
    [ACTION_LESS]              = { action_old_less,          DBG("Less", 4)            2, 1 },
    [ACTION_AND]               = { action_and,               DBG("And", 4)             2, 1 },
    [ACTION_OR]                = { action_or,                DBG("Or", 4)              2, 1 },
    [ACTION_NOT]               = { action_not,               DBG("Not", 4)             1, 1 },
    [ACTION_STRING_EQUALS]     = { action_string_compare_eq, DBG("StringEquals", 4)    2, 1 },
    [ACTION_STRING_LENGTH]     = { action_string_length,     DBG("StringLength", 4)    1, 1 },
    [ACTION_STRING_EXTRACT]    = { action_string_extract,    DBG("StringExtract", 4)   3, 1 },
    [ACTION_POP]               = { action_pop,               DBG("Pop", 4)             1, 0 },
    [ACTION_TO_INTEGER]        = { action_to_integer,        DBG("ToInteger", 4)       1, 1 },
    [ACTION_GET_VARIABLE]      = { action_get_variable,      DBG("GetVariable", 4)     1, 1 },
    [ACTION_SET_VARIABLE]      = { action_set_variable,      DBG("SetVariable", 4)     2, 0 },
    /* version 3 */
    [ACTION_SET_TARGET2]       = { action_set_target2,       DBG("SetTarget2", 3)      1, 0 },
    /* version 4 */
    [ACTION_STRING_ADD]        = { action_string_add,        DBG("StringAdd", 4)       2, 1 },
    [ACTION_GET_PROPERTY]      = { action_get_property,      DBG("GetProperty", 4)     2, 1 },
    [ACTION_SET_PROPERTY]      = { action_set_property,      DBG("SetProperty", 4)     3, 0 },
    [ACTION_CLONE_SPRITE]      = { action_clone_sprite,      DBG("CloneSprite", 4)     3, 0 },
    [ACTION_REMOVE_SPRITE]     = { action_remove_sprite,     DBG("RemoveSprite", 4)    1, 0 },
    [ACTION_TRACE]             = { action_trace,             DBG("Trace", 4)           1, 0 },
    [ACTION_START_DRAG]        = { action_start_drag,        DBG("StartDrag", 4)      -1, 0 },
    [ACTION_END_DRAG]          = { action_end_drag,          DBG("EndDrag", 4)         0, 0 },
    [ACTION_STRING_LESS]       = { action_string_compare_le, DBG("StringLess", 4)      2, 1 },
    /* version 7 */
    [ACTION_THROW]             = { action_throw,             DBG("Throw", 7)           1, 0 },
    [ACTION_CAST]              = { action_cast,              DBG("Cast", 7)            2, 1 },
    [ACTION_IMPLEMENTS]        = { action_implements,        DBG("Implements", 7)     -1, 0 },
    /* version 4 */
    [ACTION_RANDOM]            = { action_random_number,     DBG("RandomNumber", 4)    1, 1 },
    [ACTION_MB_STRING_LENGTH]  = { action_mb_string_length,  DBG("MBStringLength", 4)  1, 1 },
    [ACTION_CHAR_TO_ASCII]     = { action_char_to_ascii,     DBG("CharToAscii", 4)     1, 1 },
    [ACTION_ASCII_TO_CHAR]     = { action_ascii_to_char,     DBG("AsciiToChar", 4)     1, 1 },
    [ACTION_GET_TIME]          = { action_get_time,          DBG("GetTime", 4)         0, 1 },
    [ACTION_MB_STRING_EXTRACT] = { action_mb_string_extract, DBG("MBStringExtract", 4) 3, 1 },
    [ACTION_MB_CHAR_TO_ASCII]  = { action_mb_char_to_ascii,  DBG("MBCharToAscii", 4)   1, 1 },
    [ACTION_MB_ASCII_TO_CHAR]  = { action_mb_ascii_to_char,  DBG("MBAsciiToChar", 4)   1, 1 },
    /* version 5 */
    [ACTION_DELETE]            = { action_delete,            DBG("Delete", 5)          2, 1 },
    [ACTION_DELETE2]           = { action_delete2,           DBG("Delete2", 5)         1, 1 },
    [ACTION_DEFINE_LOCAL]      = { action_define_local,      DBG("DefineLocal", 5)     2, 0 },
    [ACTION_CALL_FUNCTION]     = { action_call_function,     DBG("CallFunction", 5)   -1, 1 },
    [ACTION_RETURN]            = { action_return,            DBG("Return", 5)          1, 0 },
    [ACTION_MODULO]            = { action_modulo,            DBG("Modulo", 5)          2, 1 },
    [ACTION_NEW_OBJECT]        = { action_new_object,        DBG("NewObject", 5)      -1, 1 },
    [ACTION_DEFINE_LOCAL2]     = { action_define_local2,     DBG("DefineLocal2", 5)    1, 0 },
    [ACTION_INIT_ARRAY]        = { action_init_array,        DBG("InitArray", 5)      -1, 1 },
    [ACTION_INIT_OBJECT]       = { action_init_object,       DBG("InitObject", 5)     -1, 1 },
    [ACTION_TYPE_OF]           = { action_type_of,           DBG("TypeOf", 5)          1, 1 },
    [ACTION_TARGET_PATH]       = { action_target_path,       DBG("TargetPath", 5)      1, 1 },
    [ACTION_ENUMERATE]         = { action_enumerate,         DBG("Enumerate", 5)       1, -1 },
    [ACTION_ADD2]              = { action_add2,              DBG("Add2", 5)            2, 1 },
    [ACTION_LESS2]             = { action_less2,             DBG("Less2", 5)           2, 1 },
    [ACTION_EQUALS2]           = { action_equals2,           DBG("Equals2", 5)         2, 1 },
    [ACTION_TO_NUMBER]         = { action_to_number,         DBG("ToNumber", 5)        1, 1 },
    [ACTION_TO_STRING]         = { action_to_string,         DBG("ToString", 5)        1, 1 },
    [ACTION_PUSH_DUPLICATE]    = { action_push_duplicate,    DBG("PushDuplicate", 5)   1, 2 },
    [ACTION_SWAP]              = { action_swap,              DBG("Swap", 5)            2, 2 },
    /* version 4 */
    [ACTION_GET_MEMBER]        = { action_get_member,        DBG("GetMember", 4)       2, 1 },
    [ACTION_SET_MEMBER]        = { action_set_member,        DBG("SetMember", 4)       3, 0 },
    /* version 5 */
    [ACTION_INCREMENT]         = { action_increment,         DBG("Increment", 5)       1, 1 },
    [ACTION_DECREMENT]         = { action_decrement,         DBG("Decrement", 5)       1, 1 },
    [ACTION_CALL_METHOD]       = { action_call_method,       DBG("CallMethod", 5)     -1, 1 },
    [ACTION_NEW_METHOD]        = { action_new_method,        DBG("NewMethod", 5)      -1, 1 },
    /* version 6 */
    [ACTION_INSTANCE_OF]       = { action_instance_of,       DBG("InstanceOf", 6)      2, 1 },
    [ACTION_ENUMERATE2]        = { action_enumerate2,        DBG("Enumerate2", 6)      1, -1 },
    [ACTION_BREAKPOINT]        = { 0,                        DBG("Breakpoint", 6)     -1, -1 },
    /* version 5 */
    [ACTION_BIT_AND]           = { action_bitwise_and,       DBG("BitAnd", 5)          2, 1 },
    [ACTION_BIT_OR]            = { action_bitwise_or,        DBG("BitOr", 5)           2, 1 },
    [ACTION_BIT_XOR]           = { action_bitwise_xor,       DBG("BitXor", 5)          2, 1 },
    [ACTION_BIT_LSHIFT]        = { action_lshift,            DBG("BitLShift", 5)       2, 1 },
    [ACTION_BIT_RSHIFT]        = { action_rshift,            DBG("BitRShift", 5)       2, 1 },
    [ACTION_BIT_URSHIFT]       = { action_urshift,           DBG("BitURShift", 5)      2, 1 },
    /* version 6 */
    [ACTION_STRICT_EQUALS]     = { action_strict_equals,     DBG("StrictEquals", 6)    2, 1 },
    [ACTION_GREATER]           = { action_gt,                DBG("Greater", 6)         2, 1 },
    [ACTION_STRING_GREATER]    = { action_string_compare_gt, DBG("StringGreater", 6) 2, 1 },
    /* version 7 */
    [ACTION_EXTENDS]           = { action_extends,           DBG("Extends", 7)         2, 0 },
    /* version 1 */
    [ACTION_GOTO_FRAME]        = { action_goto_frame,        DBG("GotoFrame", 1)       0, 0 },
    [ACTION_GET_URL]           = { action_get_url,           DBG("GetURL", 1)          0, 0 },
    /* version 5 */
    [ACTION_STORE_REGISTER]    = { action_store_register,    DBG("StoreRegister", 5)   1, 1 },
    [ACTION_CONSTANT_POOL]     = { action_constant_pool,     DBG("ConstantPool", 5)    0, 0 },
    [ACTION_STRICT_MODE]       = { 0,                        DBG("StrictMode", 5)     -1, -1 },
    /* version 1 */
    [ACTION_WAIT_FOR_FRAME]    = { action_wait_for_frame,    DBG("WaitForFrame", 1)    0, 0 },
    [ACTION_SET_TARGET]        = { action_set_target,        DBG("SetTarget", 1)       0, 0 },
    /* version 3 */
    [ACTION_GOTO_LABEL]        = { action_goto_label,        DBG("GotoLabel", 3)       0, 0 },
    /* version 4 */
    [ACTION_WAIT_FOR_FRAME2]   = { action_wait_for_frame2,   DBG("WaitForFrame2", 4)   1, 0 },
    /* version 7 */
    [ACTION_DEFINE_FUNCTION2]  = { action_define_function2,  DBG("DefineFunction2", 7) 0, -1 },
    [ACTION_TRY]               = { action_try,               DBG("Try", 7)             0, 0 },
    /* version 5 */
    [ACTION_WITH]              = { action_with,              DBG("With", 5)            1, 0 },
    /* version 4 */
    [ACTION_PUSH]              = { action_push,              DBG("Push", 4)            0, -1 },
    [ACTION_JUMP]              = { action_jump,              DBG("Jump", 4)            0, 0 },
    [ACTION_GET_URL2]          = { action_get_url2,          DBG("GetURL2", 4)         2, 0 },
    /* version 5 */
    [ACTION_DEFINE_FUNCTION]   = { action_define_function,   DBG("DefineFunction", 5)  0, -1 },
    /* version 4 */
    [ACTION_IF]                = { action_if,                DBG("If", 4)              1, 0 },
    [ACTION_CALL]              = { 0,                        DBG("Call", 4)           -1, -1 },
    [ACTION_GOTO_FRAME2]       = { action_goto_frame2,       DBG("GotoFrame2", 4)      1, 0 }
};

void lvgExecuteActions(LVGMovieClip *clip, LVGAction *actions, int num_actions)
{
    if (!actions)
        return;
    LVGActionCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.clip   = clip;
    ctx.group  = clip->groups;
    ctx.frame  = ctx.group->frames + ctx.group->cur_frame;
    ctx.stack_ptr = sizeof(ctx.stack)/sizeof(ctx.stack[0]) - 1;
    for (; ctx.pc < num_actions;)
    {
        LVGAction *a = &actions[ctx.pc++];
        const ActionEntry *ae = &g_avm1_actions[a->opcode];
        if (ae->vm_func)
            ae->vm_func(&ctx, a);
    }
}
