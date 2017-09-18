#include "avm1.h"
#include "avm2.h"
#include <signal.h>
#include <assert.h>

#ifdef _TEST
#define DBG(n, m, l, k) n, m, l, k,
#define DBG_BREAK printf("unsupported: %s\n", __FUNCTION__); fflush(stdout); ctx->do_exit = 1;
#elif defined(_DEBUG)
#define DBG(n, m, l, k) n, m, l, k,
#define DBG_BREAK raise(SIGTRAP)
#else
#define DBG(n, m, l, k)
#define DBG_BREAK
#endif

static void action_stub(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }

typedef struct
{
    void (*vm_func)(LVGActionCtx *ctx, uint8_t *a);
} ActionEntry2;

static const ActionEntry2 g_avm2_actions[256] =
{
    [ABC_ACTION_END]            = { action_stub },
    [ABC_ACTION_BKPT]           = { action_stub },
    [ABC_ACTION_NOP]            = { action_stub },
    [ABC_ACTION_THROW]          = { action_stub },
    [ABC_ACTION_GETSUPER]       = { action_stub },
    [ABC_ACTION_SETSUPER]       = { action_stub },
    [ABC_ACTION_DXNS]           = { action_stub },
    [ABC_ACTION_DXNSLATE]       = { action_stub },
    [ABC_ACTION_KILL]           = { action_stub },
    [ABC_ACTION_LABEL]          = { action_stub },
    [ABC_ACTION_IFNLT]          = { action_stub },
    [ABC_ACTION_IFNLE]          = { action_stub },
    [ABC_ACTION_IFNGT]          = { action_stub },
    [ABC_ACTION_IFNGE]          = { action_stub },
    [ABC_ACTION_JUMP]           = { action_stub },
    [ABC_ACTION_IFTRUE]         = { action_stub },
    [ABC_ACTION_IFFALSE]        = { action_stub },
    [ABC_ACTION_IFEQ]           = { action_stub },
    [ABC_ACTION_IFNE]           = { action_stub },
    [ABC_ACTION_IFLT]           = { action_stub },
    [ABC_ACTION_IFLE]           = { action_stub },
    [ABC_ACTION_IFGT]           = { action_stub },
    [ABC_ACTION_IFGE]           = { action_stub },
    [ABC_ACTION_IFSTRICTEQ]     = { action_stub },
    [ABC_ACTION_IFSTRICTNE]     = { action_stub },
    [ABC_ACTION_LOOKUPSWITCH]   = { action_stub },
    [ABC_ACTION_PUSHWITH]       = { action_stub },
    [ABC_ACTION_POPSCOPE]       = { action_stub },
    [ABC_ACTION_NEXTNAME]       = { action_stub },
    [ABC_ACTION_HASNEXT]        = { action_stub },
    [ABC_ACTION_PUSHNULL]       = { action_stub },
    [ABC_ACTION_PUSHUNDEFINED]  = { action_stub },
    [ABC_ACTION_NEXTVALUE]      = { action_stub },
    [ABC_ACTION_PUSHBYTE]       = { action_stub },
    [ABC_ACTION_PUSHSHORT]      = { action_stub },
    [ABC_ACTION_PUSHTRUE]       = { action_stub },
    [ABC_ACTION_PUSHFALSE]      = { action_stub },
    [ABC_ACTION_PUSHNAN]        = { action_stub },
    [ABC_ACTION_POP]            = { action_stub },
    [ABC_ACTION_DUP]            = { action_stub },
    [ABC_ACTION_SWAP]           = { action_stub },
    [ABC_ACTION_PUSHSTRING]     = { action_stub },
    [ABC_ACTION_PUSHINT]        = { action_stub },
    [ABC_ACTION_PUSHUINT]       = { action_stub },
    [ABC_ACTION_PUSHDOUBLE]     = { action_stub },
    [ABC_ACTION_PUSHSCOPE]      = { action_stub },
    [ABC_ACTION_PUSHNAMESPACE]  = { action_stub },
    [ABC_ACTION_HASNEXT2]       = { action_stub },
    [ABC_ACTION_NEWFUNCTION]    = { action_stub },
    [ABC_ACTION_CALL]           = { action_stub },
    [ABC_ACTION_CONSTRUCT]      = { action_stub },
    [ABC_ACTION_CALLMETHOD]     = { action_stub },
    [ABC_ACTION_CALLSTATIC]     = { action_stub },
    [ABC_ACTION_CALLSUPER]      = { action_stub },
    [ABC_ACTION_CALLPROPERTY]   = { action_stub },
    [ABC_ACTION_RETURNVOID]     = { action_stub },
    [ABC_ACTION_RETURNVALUE]    = { action_stub },
    [ABC_ACTION_CONSTRUCTSUPER] = { action_stub },
    [ABC_ACTION_CONSTRUCTPROP]  = { action_stub },
    [ABC_ACTION_CALLSUPERID]    = { action_stub },
    [ABC_ACTION_CALLPROPLEX]    = { action_stub },
    [ABC_ACTION_CALLINTERFACE]  = { action_stub },
    [ABC_ACTION_CALLSUPERVOID]  = { action_stub },
    [ABC_ACTION_CALLPROPVOID]   = { action_stub },
    [ABC_ACTION_NEWOBJECT]      = { action_stub },
    [ABC_ACTION_NEWARRAY]       = { action_stub },
    [ABC_ACTION_NEWACTIVATION]  = { action_stub },
    [ABC_ACTION_NEWCLASS]       = { action_stub },
    [ABC_ACTION_GETDESCENDANTS] = { action_stub },
    [ABC_ACTION_NEWCATCH]       = { action_stub },
    [ABC_ACTION_FINDPROPSTRICT] = { action_stub },
    [ABC_ACTION_FINDPROPERTY]   = { action_stub },
    [ABC_ACTION_FINDDEF]        = { action_stub },
    [ABC_ACTION_GETLEX]         = { action_stub },
    [ABC_ACTION_SETPROPERTY]    = { action_stub },
    [ABC_ACTION_GETLOCAL]       = { action_stub },
    [ABC_ACTION_SETLOCAL]       = { action_stub },
    [ABC_ACTION_GETGLOBALSCOPE] = { action_stub },
    [ABC_ACTION_GETSCOPEOBJECT] = { action_stub },
    [ABC_ACTION_GETPROPERTY]    = { action_stub },
    [ABC_ACTION_INITPROPERTY]   = { action_stub },
    [ABC_ACTION_DELETEPROPERTY] = { action_stub },
    [ABC_ACTION_GETSLOT]        = { action_stub },
    [ABC_ACTION_SETSLOT]        = { action_stub },
    [ABC_ACTION_GETGLOBALSLOT]  = { action_stub },
    [ABC_ACTION_SETGLOBALSLOT]  = { action_stub },
    [ABC_ACTION_CONVERT_S]      = { action_stub },
    [ABC_ACTION_ESC_XELEM]      = { action_stub },
    [ABC_ACTION_ESC_XATTR]      = { action_stub },
    [ABC_ACTION_CONVERT_I]      = { action_stub },
    [ABC_ACTION_CONVERT_U]      = { action_stub },
    [ABC_ACTION_CONVERT_D]      = { action_stub },
    [ABC_ACTION_CONVERT_B]      = { action_stub },
    [ABC_ACTION_CONVERT_O]      = { action_stub },
    [ABC_ACTION_CHECKFILTER]    = { action_stub },
    [ABC_ACTION_COERCE]         = { action_stub },
    [ABC_ACTION_COERCE_B]       = { action_stub },
    [ABC_ACTION_COERCE_A]       = { action_stub },
    [ABC_ACTION_COERCE_I]       = { action_stub },
    [ABC_ACTION_COERCE_D]       = { action_stub },
    [ABC_ACTION_COERCE_S]       = { action_stub },
    [ABC_ACTION_ASTYPE]         = { action_stub },
    [ABC_ACTION_ASTYPELATE]     = { action_stub },
    [ABC_ACTION_COERCE_U]       = { action_stub },
    [ABC_ACTION_COERCE_O]       = { action_stub },
    [ABC_ACTION_NEGATE]         = { action_stub },
    [ABC_ACTION_INCREMENT]      = { action_stub },
    [ABC_ACTION_INCLOCAL]       = { action_stub },
    [ABC_ACTION_DECREMENT]      = { action_stub },
    [ABC_ACTION_DECLOCAL]       = { action_stub },
    [ABC_ACTION_ABC_TYPEOF]     = { action_stub },
    [ABC_ACTION_NOT]            = { action_stub },
    [ABC_ACTION_BITNOT]         = { action_stub },
    [ABC_ACTION_CONCAT]         = { action_stub },
    [ABC_ACTION_ADD_D]          = { action_stub },
    [ABC_ACTION_ADD]            = { action_stub },
    [ABC_ACTION_SUBTRACT]       = { action_stub },
    [ABC_ACTION_MULTIPLY]       = { action_stub },
    [ABC_ACTION_DIVIDE]         = { action_stub },
    [ABC_ACTION_MODULO]         = { action_stub },
    [ABC_ACTION_LSHIFT]         = { action_stub },
    [ABC_ACTION_RSHIFT]         = { action_stub },
    [ABC_ACTION_URSHIFT]        = { action_stub },
    [ABC_ACTION_BITAND]         = { action_stub },
    [ABC_ACTION_BITOR]          = { action_stub },
    [ABC_ACTION_BITXOR]         = { action_stub },
    [ABC_ACTION_EQUALS]         = { action_stub },
    [ABC_ACTION_STRICTEQUALS]   = { action_stub },
    [ABC_ACTION_LESSTHAN]       = { action_stub },
    [ABC_ACTION_LESSEQUALS]     = { action_stub },
    [ABC_ACTION_GREATERTHAN]    = { action_stub },
    [ABC_ACTION_GREATEREQUALS]  = { action_stub },
    [ABC_ACTION_INSTANCEOF]     = { action_stub },
    [ABC_ACTION_ISTYPE]         = { action_stub },
    [ABC_ACTION_ISTYPELATE]     = { action_stub },
    [ABC_ACTION_IN]             = { action_stub },
    [ABC_ACTION_INCREMENT_I]    = { action_stub },
    [ABC_ACTION_DECREMENT_I]    = { action_stub },
    [ABC_ACTION_INCLOCAL_I]     = { action_stub },
    [ABC_ACTION_DECLOCAL_I]     = { action_stub },
    [ABC_ACTION_NEGATE_I]       = { action_stub },
    [ABC_ACTION_ADD_I]          = { action_stub },
    [ABC_ACTION_SUBTRACT_I]     = { action_stub },
    [ABC_ACTION_MULTIPLY_I]     = { action_stub },
    [ABC_ACTION_GETLOCAL0]      = { action_stub },
    [ABC_ACTION_GETLOCAL1]      = { action_stub },
    [ABC_ACTION_GETLOCAL2]      = { action_stub },
    [ABC_ACTION_GETLOCAL3]      = { action_stub },
    [ABC_ACTION_SETLOCAL0]      = { action_stub },
    [ABC_ACTION_SETLOCAL1]      = { action_stub },
    [ABC_ACTION_SETLOCAL2]      = { action_stub },
    [ABC_ACTION_SETLOCAL3]      = { action_stub },
    [ABC_ACTION_ABS_JUMP]       = { action_stub },
    [ABC_ACTION_DEBUG]          = { action_stub },
    [ABC_ACTION_DEBUGLINE]      = { action_stub },
    [ABC_ACTION_DEBUGFILE]      = { action_stub },
    [ABC_ACTION_BKPTLINE]       = { action_stub },
    [ABC_ACTION_TIMESTAMP]      = { action_stub },
    [ABC_ACTION_VERIFYPASS]     = { action_stub },
    [ABC_ACTION_ALLOC]          = { action_stub },
    [ABC_ACTION_MARK]           = { action_stub },
    [ABC_ACTION_WB]             = { action_stub },
    [ABC_ACTION_PROLOGUE]       = { action_stub },
    [ABC_ACTION_SENDENTER]      = { action_stub },
    [ABC_ACTION_DOUBLETOATOM]   = { action_stub },
    [ABC_ACTION_SWEEP]          = { action_stub },
    [ABC_ACTION_CODEGENOP]      = { action_stub },
    [ABC_ACTION_VERIFYOP]       = { action_stub }
};

void lvgExecuteActions2(LVGActionCtx *ctx, uint8_t *actions, LVGMovieClipGroupState *groupstate, int is_function)
{
    if (!actions)
        return;
    ctx->groupstate = groupstate;
    ctx->group  = ctx->clip->groups + groupstate->group_num;
    ctx->frame  = ctx->group->frames + groupstate->cur_frame;
    if (is_function)
    {
        ctx->size = *(uint16_t*)actions;
        actions += 2;
        // TODO: use DEFINEFUNCTION2 flags
        ctx->regs[1] = *search_var(ctx, "_root");
    } else
    {
        ctx->size = *(uint32_t*)actions;
        actions += 4;
    }
    ctx->actions = actions;
    ctx->pc = 0;
restart:
    for (; ctx->pc < ctx->size;)
    {
        Actions2 a = actions[ctx->pc++];
        const ActionEntry2 *ae = &g_avm2_actions[a];
        uint8_t *opdata = &actions[ctx->pc];
        if (ae->vm_func)
            ae->vm_func(ctx, opdata);
        if (ctx->do_exit)
            break;
    }
    if (ctx->call_depth && !ctx->do_exit)
    {
        ctx->call_depth--;
        ctx->pc   = ctx->calls[ctx->call_depth].save_pc;
        ctx->size = ctx->calls[ctx->call_depth].save_size;
        THIS = ctx->calls[ctx->call_depth].save_this;
        g_properties[0].val.type = THIS ? ASVAL_CLASS : ASVAL_UNDEFINED;
        goto restart;
    }
}
