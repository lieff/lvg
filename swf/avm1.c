#include "avm1.h"
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>

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

static float read_float(const uint8_t *p)
{
    union {
        float f;
        uint32_t i;
        struct {
            uint16_t s0;
            uint16_t s1;
        } s;
        struct {
            uint8_t c0;
            uint8_t c1;
            uint8_t c2;
            uint8_t c3;
        } c;
    } u;

    u.f = 1.0;
    switch (u.s.s0)
    {
    case 0x0000: // little-endian host
        memcpy(&u.i, p, 4);
        break;
    case 0x3f80: // big-endian host
        u.c.c0 = p[3];
        u.c.c1 = p[2];
        u.c.c2 = p[1];
        u.c.c3 = p[0];
        break;
    default:
        assert(0);
    }

    return u.f;
}

static double read_double(const uint8_t *p)
{
    union {
        double d;
        uint64_t i;
        struct {
            uint32_t l0;
            uint32_t l1;
        } l;
        struct {
            uint16_t s0;
            uint16_t s1;
            uint16_t s2;
            uint16_t s3;
        } s;
        struct {
            uint8_t c0;
            uint8_t c1;
            uint8_t c2;
            uint8_t c3;
            uint8_t c4;
            uint8_t c5;
            uint8_t c6;
            uint8_t c7;
        } c;
    } u;

    _Static_assert(sizeof(u) == sizeof(u.i), "double must be 8 bytes");

    // Detect endianness of doubles by storing a value that is
    // exactly representable and that has different values in the
    // four 16-bit words.
    // 0x11223344 is represented as 0x41b1 2233 4400 0000 (bigendian)
    u.d = 0x11223344;
    switch (u.s.s0)
    {
    case 0x0000: // little-endian host
        memcpy(&u.l.l1, p, 4);
        memcpy(&u.l.l0, p + 4, 4);
        break;
    case 0x41b1: // big-endian host
        u.c.c0 = p[3];
        u.c.c1 = p[2];
        u.c.c2 = p[1];
        u.c.c3 = p[0];
        u.c.c4 = p[7];
        u.c.c5 = p[6];
        u.c.c6 = p[5];
        u.c.c7 = p[4];
        break;
    case 0x2233: // word-swapped little-endian host (PDP / ARM FPA)
        memcpy(&u.i, p, 8);
        break;
    case 0x4400: // word-swapped big-endian host: does this exist?
        u.c.c0 = p[7];
        u.c.c1 = p[6];
        u.c.c2 = p[5];
        u.c.c3 = p[4];
        u.c.c4 = p[3];
        u.c.c5 = p[2];
        u.c.c6 = p[1];
        u.c.c7 = p[0];
        break;
    default:
        assert(0);
    }

    return u.d;
}

double to_double(ASVal *v)
{
    if (ASVAL_DOUBLE == v->type)
        return v->d_int;
    else if (ASVAL_FLOAT == v->type)
        return v->f_int;
    else if (ASVAL_INT == v->type)
        return v->i32;
    else if (ASVAL_BOOL == v->type)
        return v->boolean;
    else if (ASVAL_UNDEFINED == v->type)
        return NAN;
    else if (ASVAL_STRING == v->type)
    {
        char *end = 0;
        double dval = strtod(v->str, &end);
        if (end && 0 == *end)
            return dval;
        return NAN;
    } else if (ASVAL_CLASS == v->type)
        return 0.0; // TODO
    return 0.0;
}

int32_t to_int(ASVal *v)
{
    if (ASVAL_DOUBLE == v->type)
        return v->d_int;
    else if (ASVAL_FLOAT == v->type)
        return v->f_int;
    else if (ASVAL_INT == v->type)
        return v->i32;
    else if (ASVAL_BOOL == v->type)
        return v->boolean;
    return 0;
}

int is_number(ASVal *v)
{
    return ASVAL_DOUBLE == v->type || ASVAL_FLOAT == v->type || ASVAL_INT == v->type || ASVAL_BOOL == v->type;
}

static ASVal *search_var(LVGActionCtx *ctx, const char *name)
{
    int i;
    for (int i = 0; i < ctx->num_locals; i++)
        if (0 == strcmp(ctx->locals[i].name, name))
            return &ctx->locals[i].val;
    for (i = 0; i < g_num_properties; i++)
        if (0 == strcmp(g_properties[i].name, name))
            return &g_properties[i].val;
    ASVal *as_this = &g_properties[0].val;
    assert(ASVAL_CLASS == as_this->type);
    ASClass *pthis = (ASClass *)as_this->str;
    for (i = 0; i < pthis->num_members; i++)
        if (0 == strcmp(pthis->members[i].name, name))
            return &pthis->members[i].val;
    for (i = 0; i < g_num_classes; i++)
        if (0 == strcmp(g_classes[i].cls->name, name))
            return &g_classes[i];
#ifdef _DEBUG
    printf("warning: var %s not found\n", name); fflush(stdout);
#endif
    return 0;
}

ASVal *create_local(LVGActionCtx *ctx, const char *name)
{
    for (int i = 0; i < ctx->num_locals; i++)
        if (0 == strcmp(ctx->locals[i].name, name))
            return &ctx->locals[i].val;
    ctx->locals = realloc(ctx->locals, (ctx->num_locals + 1)*sizeof(ctx->locals[0]));
    ASMember *res = ctx->locals + ctx->num_locals++;
    res->name = name;
    return &res->val;
}

ASClass *create_instance(ASClass *base)
{
    ASClass *cls = malloc(sizeof(ASClass));
    memcpy(cls, base, sizeof(ASClass));
    cls->members = malloc(cls->num_members*sizeof(ASMember));
    memcpy(cls->members, base->members, cls->num_members*sizeof(ASMember));
    return cls;
}

void free_instance(ASClass *cls)
{
    free(cls->members);
    free(cls);
}

static void do_call(LVGActionCtx *ctx, ASClass *c, ASVal *var, uint8_t *a, uint32_t nargs)
{
    if (!var->str)
        return;
    if (ASVAL_FUNCTION == var->type)
    {
        uint8_t *func = (uint8_t *)var->str;
        ctx->calls[ctx->call_depth].save_pc   = ctx->pc;
        ctx->calls[ctx->call_depth].save_size = ctx->size;
        ctx->call_depth++;
        ctx->pc = func - ctx->actions + 2;
        ctx->size = ctx->pc + *(uint16_t*)func;
    } else
    if (ASVAL_NATIVE_FN == var->type)
    {
        var->fn(ctx, c, a, nargs);
    } else
    {
        assert(0);
    }
}

static void action_end(LVGActionCtx *ctx, uint8_t *a)
{
}

static void action_next_frame(LVGActionCtx *ctx, uint8_t *a)
{
    ctx->group->cur_frame = (ctx->group->cur_frame + 1) % ctx->group->num_frames;
}

static void action_previous_frame(LVGActionCtx *ctx, uint8_t *a)
{
    if (ctx->group->cur_frame)
        ctx->group->cur_frame--;
}

static void action_play(LVGActionCtx *ctx, uint8_t *a)
{
    ctx->group->play_state = LVG_PLAYING;
}

static void action_stop(LVGActionCtx *ctx, uint8_t *a)
{
    ctx->group->play_state = LVG_STOPPED;
}

static void action_quality(LVGActionCtx *ctx, uint8_t *a)
{   // ignore
}

static void action_stop_sounds(LVGActionCtx *ctx, uint8_t *a)
{
    lvgStopAudio();
}

static void action_add(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, vb + va);
}

static void action_sub(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, vb - va);
}

static void action_mul(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_DOUBLE(res, vb*va);
}

static void action_div(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (0.0 == va)
    {
        if (ctx->version < 5)
            SET_STRING(res, "#ERROR#")
        else if (0.0 == vb || isnan(vb) || isnan(va))
            SET_DOUBLE(res, NAN)
        else
        {
            double r = (vb < 0) ? -INFINITY : INFINITY;
            SET_DOUBLE(res, r);
        }
        return;
    }
    SET_DOUBLE(res, vb/va);
}

static void action_old_eq(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (ctx->version < 5)
        SET_DOUBLE(res, (vb == va) ? 1.0 : 0.0)
    else
        SET_BOOL(res, (vb == va) ? 1 : 0)
}

static void action_old_less(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (ctx->version < 5)
        SET_DOUBLE(res, (vb < va) ? 1.0 : 0.0)
    else
        SET_BOOL(res, (vb < va) ? 1 : 0)
}

static void action_and(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (ctx->version < 5)
        SET_DOUBLE(res, (vb != 0.0 && va != 0.0) ? 1.0 : 0.0)
    else
        SET_BOOL(res, (vb != 0.0 && va != 0.0) ? 1 : 0)
}

static void action_or(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (ctx->version < 5)
        SET_DOUBLE(res, (vb != 0.0 || va != 0.0) ? 1.0 : 0.0)
    else
        SET_BOOL(res, (vb != 0.0 || va != 0.0) ? 1 : 0)
}

static void action_not(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    double va = to_double(se_a);
    if (ctx->version < 5)
        SET_DOUBLE(se_a, (va != 0.0) ? 0.0 : 1.0)
    else
        SET_BOOL(se_a, (va != 0.0) ? 0 : 1)
}

static void action_string_compare_eq(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    assert(ASVAL_STRING == se_a->type && ASVAL_STRING == se_b->type);
    int cmp = strcmp(se_b->str, se_a->str);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (ctx->version < 5)
        SET_DOUBLE(res, cmp ? 0.0 : 1.0)
    else
        SET_BOOL(res, cmp ? 0 : 1)
}

static void action_string_length(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    assert(ASVAL_STRING == se_a->type);
    int len = strlen(se_a->str);
    SET_DOUBLE(se_a, len);
}

static void action_string_extract(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_count = &ctx->stack[ctx->stack_ptr];
    ASVal *se_index = se_count + 1;
    ASVal *se_str   = se_count + 2;
    ctx->stack_ptr += 2;
    assert(ASVAL_STRING == se_str->type);
    assert(ASVAL_INT == se_count->type || ASVAL_DOUBLE == se_count->type || ASVAL_FLOAT == se_count->type);
    assert(ASVAL_INT == se_index->type || ASVAL_DOUBLE == se_index->type || ASVAL_FLOAT == se_index->type);
    size_t len = strlen(se_str->str);
    uint32_t idx = to_int(se_index);
    assert(idx <= len);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_STRING(res, se_str->str + idx); // TODO: use se_count and allocate new string with gc
}

static void action_pop(LVGActionCtx *ctx, uint8_t *a)
{
    ctx->stack_ptr++;
    assert(ctx->stack_ptr < sizeof(ctx->stack)/sizeof(ctx->stack[0]));
}

static void action_to_integer(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_var = &ctx->stack[ctx->stack_ptr];
    double var = to_double(se_var);
    se_var->type = ASVAL_INT;
    se_var->i32 = (int32_t)floor(var);
}

static void action_get_variable(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se = &ctx->stack[ctx->stack_ptr];
    assert(ASVAL_STRING == se->type);
    ASVal *var = search_var(ctx, se->str);
    if (var)
    {
        se->type = var->type;
        se->str  = var->str;
        return;
    } else
        SET_UNDEF(se);
}

static void action_set_variable(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_val = &ctx->stack[ctx->stack_ptr];
    ASVal *se_var = se_val + 1;
    ctx->stack_ptr += 2;
    assert(ASVAL_STRING == se_var->type);
    ASVal *res = create_local(ctx, se_var->str);
    *res = *se_val;
}

static void action_set_target2(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_string_add(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    assert(ASVAL_STRING == se_a->type && ASVAL_STRING == se_b->type);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_STRING(res, se_b->str); // TODO: use se_a and allocate new string with gc
}

static const char *props[] =
{
    "_X", "_Y", "_xscale", "_yscale", "_currentframe", "_totalframes", "_alpha", "_visible",
    "_width", "_height", "_rotation", "_target", "_framesloaded", "_name", "_droptarget",
    "_url", "_highquality", "_focusrect", "_soundbuftime", "_quality", "_xmouse", "_ymouse"
};

static void action_get_property(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_idx = &ctx->stack[ctx->stack_ptr];
    ASVal *se_target = se_idx + 1;
    ctx->stack_ptr += 1;
    assert(ASVAL_INT == se_idx->type);
    assert(ASVAL_STRING == se_target->type);
    assert(se_idx->ui32 <= 21);
    uint32_t idx = to_int(se_idx);
    if (idx > 21)
        return;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    ASClass *c = ctx->group->movieclip;
    const char *prop = props[idx];
    for (int i = 0; i < c->num_members; i++)
        if (0 == strcmp(c->members[i].name, prop))
        {
            *res = c->members[i].val;
            return;
        }
    assert(0);
}

static void action_set_property(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_val = &ctx->stack[ctx->stack_ptr];
    ASVal *se_idx = se_val + 1;
    ASVal *se_target = se_val + 2;
    ctx->stack_ptr += 3;
    assert(ASVAL_INT == se_idx->type);
    assert(ASVAL_STRING == se_target->type);
    assert(se_idx->ui32 <= 21);
    uint32_t idx = to_int(se_idx);
    if (idx > 21)
        return;
    ASClass *c = ctx->group->movieclip;
    const char *prop = props[idx];
    for (int i = 0; i < c->num_members; i++)
        if (0 == strcmp(c->members[i].name, prop))
        {
            c->members[i].val = *se_val;
            return;
        }
    assert(0);
}

static void action_clone_sprite(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_remove_sprite(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_trace(LVGActionCtx *ctx, uint8_t *a)
{
#ifdef _DEBUG
    ASVal *se = &ctx->stack[ctx->stack_ptr];
    ctx->stack_ptr++;
    if (ASVAL_STRING == se->type)
        printf("%s\n", se->str);
    else if (ASVAL_INT == se->type)
        printf("%d\n", se->i32);
    else if (ASVAL_FLOAT == se->type)
        printf("%.6f\n", se->f_int);
    else if (ASVAL_DOUBLE == se->type)
        printf("%.10f\n", se->d_int);
    else if (ASVAL_CLASS == se->type)
        printf("_level0\n");
    fflush(stdout);
#endif
}

static void action_start_drag(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_end_drag(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }

static void action_string_compare_le(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    assert(ASVAL_STRING == se_a->type && ASVAL_STRING == se_b->type);
    int cmp = strcmp(se_b->str, se_a->str);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    if (ctx->version < 5)
        SET_DOUBLE(res, (cmp < 0) ? 1.0 : 0.0)
    else
        SET_BOOL(res, (cmp < 0) ? 1 : 0)
}

static void action_throw(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_cast(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_implements(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_random_number(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_mb_string_length(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_char_to_ascii(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_ascii_to_char(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_get_time(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_mb_string_extract(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_mb_char_to_ascii(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_mb_ascii_to_char(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_delete(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_delete2(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_define_local(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_val = &ctx->stack[ctx->stack_ptr];
    ASVal *se_name = se_val + 1;
    ctx->stack_ptr += 2;
    ASVal *res = create_local(ctx, se_name->str);
    *res = *se_val;
}

static void action_call_function(LVGActionCtx *ctx, uint8_t *a)
{
    if (ctx->call_depth >= sizeof(ctx->calls)/sizeof(ctx->calls[0]))
    {
        ctx->do_exit = 1;
        return;
    }
    ASVal *se_name = &ctx->stack[ctx->stack_ptr];
    ASVal *se_nargs = se_name + 1;
    ctx->stack_ptr += 2;
    assert(ASVAL_STRING == se_name->type);
    assert(ASVAL_INT == se_nargs->type || ASVAL_DOUBLE == se_nargs->type || ASVAL_FLOAT == se_nargs->type);
    uint32_t nargs = to_int(se_nargs);
    ASVal *var = search_var(ctx, se_name->str);
    if (var)
    {
        do_call(ctx, 0, var, a, nargs);
        return;
    }
    assert(0);
}

static void action_return(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_modulo(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_new_object(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_define_local2(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_init_array(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_init_object(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_type_of(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_target_path(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_enumerate(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_add2(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    *res = *se_b; // TODO: allocate with gc and concatenate
}

static void action_less2(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_BOOL(res, (vb < va) ? 1 : 0)
}

static void action_equals2(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_BOOL(res, (vb == va) ? 1 : 0)
}

static void action_to_number(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se = &ctx->stack[ctx->stack_ptr];
    double d = to_double(se);
    SET_DOUBLE(se, d);
    // TODO: support valueOF()
}

static void action_to_string(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se = &ctx->stack[ctx->stack_ptr];
    if (ASVAL_STRING == se->type)
        return;
    if (ASVAL_INT == se->type || ASVAL_DOUBLE == se->type || ASVAL_FLOAT == se->type || ASVAL_BOOL == se->type)
    {
        // TODO: allocate string with gc
    }
    if (ASVAL_CLASS == se->type)
    {
        // TODO: support toString()
    }
    assert(0);
}

static void action_push_duplicate(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_top = &ctx->stack[ctx->stack_ptr];
    ctx->stack_ptr--;
    ctx->stack[ctx->stack_ptr] = *se_top;
}

static void action_swap(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ASVal tmp = *se_a;
    *se_a = *se_b;
    *se_b = tmp;
}

static void action_get_member(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_member = &ctx->stack[ctx->stack_ptr];
    ASVal *se_var = se_member + 1;
    ctx->stack_ptr += 1;
    assert(ASVAL_CLASS == se_var->type && se_var->str && ASVAL_STRING == se_member->type);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    ASClass *c = (ASClass *)se_var->str;
    for (int i = 0; i < c->num_members; i++)
        if (0 == strcmp(se_member->str, c->members[i].name))
        {
            *res = c->members[i].val;
            return;
        }
    assert(0);
}

static void action_set_member(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_val = &ctx->stack[ctx->stack_ptr];
    ASVal *se_member = se_val + 1;
    ASVal *se_var = se_val + 2;
    ctx->stack_ptr += 3;
    if (ASVAL_UNDEFINED == se_var->type)
        return;
    ASClass *c = (ASClass *)se_var->str;
    for (int i = 0; i < c->num_members; i++)
        if (0 == strcmp(se_member->str, c->members[i].name))
        {
            int mnum = is_number(&c->members[i].val), vnum = is_number(se_val);
            if ((mnum && vnum) || c->members[i].val.type == se_val->type)
                c->members[i].val = *se_val;
            else if (mnum && ASVAL_STRING == se_val->type)
            {
                char *end = 0;
                long int ival = strtol(se_val->str, &end, 10);
                if (end && 0 == *end)
                {
                    SET_INT(&c->members[i].val, ival);
                    return;
                }
                double dval = strtod(se_val->str, &end);
                if (end && 0 == *end)
                    SET_DOUBLE(&c->members[i].val, dval);
            }
            return;
        }
#ifdef _TEST
    assert(0);
#endif
}

static void action_increment(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_decrement(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_call_method(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_method = &ctx->stack[ctx->stack_ptr];
    ASVal *se_obj = se_method + 1;
    ASVal *se_nargs = se_method + 2;
    ctx->stack_ptr += 3;
    assert(ASVAL_STRING == se_method->type);
    assert(ASVAL_CLASS == se_obj->type);
    assert(ASVAL_INT == se_nargs->type || ASVAL_DOUBLE == se_nargs->type || ASVAL_FLOAT == se_nargs->type);
    ASClass *c = (ASClass *)se_obj->str;
    for (int i = 0; i < c->num_members; i++)
        if (0 == strcmp(se_method->str, c->members[i].name))
        {
            uint32_t nargs = to_int(se_nargs);
            do_call(ctx, c, &c->members[i].val, a, nargs);
            return;
        }
    assert(0);
}

static void action_new_method(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_instance_of(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_enumerate2(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_bitwise_and(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_bitwise_or(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_bitwise_xor(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_lshift(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_rshift(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_urshift(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_strict_equals(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_gt(LVGActionCtx *ctx, uint8_t *a)
{
    assert(ctx->version >= 6);
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    double va = to_double(se_a);
    double vb = to_double(se_b);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_BOOL(res, (vb > va) ? 1 : 0);
}

static void action_string_compare_gt(LVGActionCtx *ctx, uint8_t *a)
{
    assert(ctx->version >= 6);
    ASVal *se_a = &ctx->stack[ctx->stack_ptr];
    ASVal *se_b = se_a + 1;
    ctx->stack_ptr += 1;
    assert(ASVAL_STRING == se_a->type && ASVAL_STRING == se_b->type);
    int cmp = strcmp(se_b->str, se_a->str);
    ASVal *res = &ctx->stack[ctx->stack_ptr];
    SET_BOOL(res, (cmp > 0) ? 1 : 0);
}

static void action_extends(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }

/*------------------------- vv --- with len --- vv -------------------------*/
static void action_goto_frame(LVGActionCtx *ctx, uint8_t *a)
{
    int frame = *(uint16_t*)(a + 2);
    ctx->group->cur_frame = frame % ctx->group->num_frames;
}

static void action_get_url(LVGActionCtx *ctx, uint8_t *a)
{
    const char *url = (const char*)(a + 2);
    const uint8_t *data = (const uint8_t *)a + 2;
    int i = 0;
    while (data[i++]);
    const char *target = (const char *)&data[i];
    printf("URL=%s target=%s", url, target);
}

static void action_store_register(LVGActionCtx *ctx, uint8_t *a)
{
    int reg = *(uint8_t*)(a + 2);
    ASVal *se = &ctx->stack[ctx->stack_ptr];
    ctx->regs[reg] = *se;
}

static void action_constant_pool(LVGActionCtx *ctx, uint8_t *a)
{
    //int size = *(uint16_t*)a;
    ctx->cpool_size = *(uint16_t*)(a + 2);
    ctx->cpool = realloc(ctx->cpool, ctx->cpool_size*sizeof(char *));
    const char *s = (const char *)a + 4;
    for (int i = 0; i < ctx->cpool_size; i++)
    {
        ctx->cpool[i] = s;
        while(*s++);
    }
}

static void action_wait_for_frame(LVGActionCtx *ctx, uint8_t *a)
{   // all frames always loaded - never skip actions
}

static void action_set_target(LVGActionCtx *ctx, uint8_t *a)
{
    const char *target = (const char*)(a + 2);
    if (strcmp(target, ""))
    {
        assert(0);
    }
}

static void action_goto_label(LVGActionCtx *ctx, uint8_t *a)
{
    LVGFrameLabel *l = ctx->group->labels;
    const char *name = (const char *)a + 2;
    for (int i = 0; i < ctx->group->num_labels; i++)
        if (0 == strcasecmp(name, l[i].name))
        {
            ctx->group->cur_frame = l[i].frame_num % ctx->group->num_frames;
            ctx->group->play_state = LVG_STOPPED;
            return;
        }
}

static void action_wait_for_frame2(LVGActionCtx *ctx, uint8_t *a)
{   // all frames always loaded - never skip actions
    ASVal *se_frame = &ctx->stack[ctx->stack_ptr];
    ctx->stack_ptr++;
    assert(ASVAL_DOUBLE == se_frame->type || ASVAL_FLOAT == se_frame->type || ASVAL_INT == se_frame->type || ASVAL_BOOL == se_frame->type);
}

static void action_define_function2(LVGActionCtx *ctx, uint8_t *a)
{
    const char *fname = (const char *)a + 2;
    const uint8_t *data = (const uint8_t *)a + 2;
    int i = 0;
    while (data[i++]);
    int nparams = *(uint16_t*)&data[i]; i += 2;
    /*uint8_t nregs = data[i++];
    uint8_t flags1 = data[i++];
    uint8_t flags2 = data[i++];*/
    i += 3;
    for (int p = 0; p < nparams; p++)
    {
        //uint8_t regs = data[i++];
        i++;
        while (data[i++]);
    }
    ASVal *res;
    if (!*fname)
    {
        ctx->stack_ptr--;
        res = &ctx->stack[ctx->stack_ptr];
    } else
        res = create_local(ctx, fname);
    res->type = ASVAL_FUNCTION;
    res->str  = (const char *)&data[i];
    int codesize = *(uint16_t*)&data[i]; i += 2;
    ctx->pc += codesize;
}

static void action_try(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_with(LVGActionCtx *ctx, uint8_t *a) { DBG_BREAK; }
static void action_push(LVGActionCtx *ctx, uint8_t *a)
{
    int len = *(uint16_t*)a;
    const char *data = (const char *)a + 2;
    do {
        ctx->stack_ptr--;
        ASVal *se = &ctx->stack[ctx->stack_ptr];
        assert(ctx->stack_ptr >= 0);
        int size = 0, type = *(uint8_t*)data;
        switch(type)
        {
        case 0: se->type = ASVAL_STRING; se->str = (const char*)data + 1; size = strlen(se->str) + 1; break;
        case 1: se->type = ASVAL_FLOAT; se->f_int = read_float((uint8_t*)data + 1); size = 4; break;
        case 2: se->type = ASVAL_NULL; break;
        case 3: se->type = ASVAL_UNDEFINED; break;
        case 4: { int reg = *((uint8_t*)data + 1); se->type = ctx->regs[reg].type; se->str = ctx->regs[reg].str; size = 1; } break;
        case 5: se->type = ASVAL_BOOL; se->boolean = *((uint8_t*)data + 1) ? 1 : 0; size = 1; break;
        case 6: se->type = ASVAL_DOUBLE; se->d_int = read_double((uint8_t*)data + 1); size = 8; break;
        case 7: se->type = ASVAL_INT; se->ui32 = *(uint32_t*)((char*)data + 1); size = 4; break;
        case 8: se->type = ASVAL_STRING; se->str = ctx->cpool[*(uint8_t*)((char*)data + 1)]; size = 1; break;
        case 9: se->type = ASVAL_STRING; se->str = ctx->cpool[*(uint16_t*)((char*)data + 1)]; size = 2; break;
        default:
            assert(0);
            return;
        }
        len -= size + 1;
        data += size + 1;
    } while (len > 0);
}

static void action_jump(LVGActionCtx *ctx, uint8_t *a)
{
    int8_t offset = *(uint16_t *)(a + 2);
    ctx->pc += offset;
}

static void action_get_url2(LVGActionCtx *ctx, uint8_t *a)
{
    //int flags = *(uint8_t*)a->data;
    ASVal *se_target = &ctx->stack[ctx->stack_ptr];
    ASVal *se_url = se_target + 1;
    ctx->stack_ptr += 2;
#ifdef _DEBUG
    if (0 == strcasecmp(se_url->str, "FSCommand:quit"))
        ctx->do_exit = 1;
#endif
}

static void action_define_function(LVGActionCtx *ctx, uint8_t *a)
{
    int i = 0;
    const uint8_t *data = (const uint8_t *)a + 2;
    const char *fname = (const char *)data;
    while (data[i++]);
    int params = data[i++];
    params += data[i++]*256;
    for(int j = 0; j < params;j++)
    {
        while (data[i++]);
    }
    ASVal *res;
    if (!*fname)
    {
        ctx->stack_ptr--;
        res = &ctx->stack[ctx->stack_ptr];
    } else
        res = create_local(ctx, fname);
    res->type = ASVAL_FUNCTION;
    res->str  = (const char *)&data[i];
    int codesize = data[i++];
    codesize += data[i++]*256;
    ctx->pc += codesize;
}

static void action_if(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se_cond = &ctx->stack[ctx->stack_ptr++];
    double cond = to_double(se_cond);
    int offset = *(int16_t *)(a + 2);
    if (0.0 != cond)
        ctx->pc += offset;
}

static void action_goto_frame2(LVGActionCtx *ctx, uint8_t *a)
{
    ASVal *se = &ctx->stack[ctx->stack_ptr];
    ctx->stack_ptr++;
    assert(ASVAL_INT == se->type || ASVAL_STRING == se->type);
    int add = 0, flags = *(uint8_t*)(a + 2);
    ctx->group->play_state = (flags & 1) ? LVG_PLAYING : LVG_STOPPED;
    if (flags & 2)
        add = *(uint16_t*)(a + 3);
    if (ASVAL_INT == se->type)
    {
        ctx->group->cur_frame = (se->ui32 + add) % ctx->group->num_frames;
    } else if (ASVAL_STRING == se->type)
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

static void action_play_lvg_sound(LVGActionCtx *ctx, uint8_t *a)
{
    int sound = *(uint16_t*)(a + 2);
    lvgPlaySound(ctx->clip->sounds + sound);
}

typedef struct
{
    void      (*vm_func)(LVGActionCtx *ctx, uint8_t *a);
#if defined(_DEBUG) || defined(_TEST)
    const char *name;
    uint8_t   version;
    uint8_t   npop_params;
    uint8_t   npush_params;
#endif
} ActionEntry;

const ActionEntry g_avm1_actions[256] =
{
    /* version 1 */
    [ACTION_END]               = { action_end,               DBG("End", 1,             0, 0) },
    [ACTION_NEXT_FRAME]        = { action_next_frame,        DBG("NextFrame", 1,       0, 0) },
    [ACTION_PREVIOUS_FRAME]    = { action_previous_frame,    DBG("PreviousFrame", 1,   0, 0) },
    [ACTION_PLAY]              = { action_play,              DBG("Play", 1,            0, 0) },
    [ACTION_STOP]              = { action_stop,              DBG("Stop", 1,            0, 0) },
    [ACTION_TOGGLE_QUALITY]    = { action_quality,           DBG("ToggleQuality", 1,  -1, -1) },
    /* version 2 */
    [ACTION_STOP_SOUNDS]       = { action_stop_sounds,       DBG("StopSounds", 2,      0, 0) },
    /* version 4 */
    [ACTION_ADD]               = { action_add,               DBG("Add", 4,             2, 1) },
    [ACTION_SUBTRACT]          = { action_sub,               DBG("Subtract", 4,        2, 1) },
    [ACTION_MULTIPLY]          = { action_mul,               DBG("Multiply", 4,        2, 1) },
    [ACTION_DIVIDE]            = { action_div,               DBG("Divide", 4,          2, 1) },
    [ACTION_EQUALS]            = { action_old_eq,            DBG("Equals", 4,          2, 1) },
    [ACTION_LESS]              = { action_old_less,          DBG("Less", 4,            2, 1) },
    [ACTION_AND]               = { action_and,               DBG("And", 4,             2, 1) },
    [ACTION_OR]                = { action_or,                DBG("Or", 4,              2, 1) },
    [ACTION_NOT]               = { action_not,               DBG("Not", 4,             1, 1) },
    [ACTION_STRING_EQUALS]     = { action_string_compare_eq, DBG("StringEquals", 4,    2, 1) },
    [ACTION_STRING_LENGTH]     = { action_string_length,     DBG("StringLength", 4,    1, 1) },
    [ACTION_STRING_EXTRACT]    = { action_string_extract,    DBG("StringExtract", 4,   3, 1) },
    [ACTION_POP]               = { action_pop,               DBG("Pop", 4,             1, 0) },
    [ACTION_TO_INTEGER]        = { action_to_integer,        DBG("ToInteger", 4,       1, 1) },
    [ACTION_GET_VARIABLE]      = { action_get_variable,      DBG("GetVariable", 4,     1, 1) },
    [ACTION_SET_VARIABLE]      = { action_set_variable,      DBG("SetVariable", 4,     2, 0) },
    /* version 3 */
    [ACTION_SET_TARGET2]       = { action_set_target2,       DBG("SetTarget2", 3,      1, 0) },
    /* version 4 */
    [ACTION_STRING_ADD]        = { action_string_add,        DBG("StringAdd", 4,       2, 1) },
    [ACTION_GET_PROPERTY]      = { action_get_property,      DBG("GetProperty", 4,     2, 1) },
    [ACTION_SET_PROPERTY]      = { action_set_property,      DBG("SetProperty", 4,     3, 0) },
    [ACTION_CLONE_SPRITE]      = { action_clone_sprite,      DBG("CloneSprite", 4,     3, 0) },
    [ACTION_REMOVE_SPRITE]     = { action_remove_sprite,     DBG("RemoveSprite", 4,    1, 0) },
    [ACTION_TRACE]             = { action_trace,             DBG("Trace", 4,           1, 0) },
    [ACTION_START_DRAG]        = { action_start_drag,        DBG("StartDrag", 4,      -1, 0) },
    [ACTION_END_DRAG]          = { action_end_drag,          DBG("EndDrag", 4,         0, 0) },
    [ACTION_STRING_LESS]       = { action_string_compare_le, DBG("StringLess", 4,      2, 1) },
    /* version 7 */
    [ACTION_THROW]             = { action_throw,             DBG("Throw", 7,           1, 0) },
    [ACTION_CAST]              = { action_cast,              DBG("Cast", 7,            2, 1) },
    [ACTION_IMPLEMENTS]        = { action_implements,        DBG("Implements", 7,     -1, 0) },
    /* version 4 */
    [ACTION_RANDOM]            = { action_random_number,     DBG("RandomNumber", 4,    1, 1) },
    [ACTION_MB_STRING_LENGTH]  = { action_mb_string_length,  DBG("MBStringLength", 4,  1, 1) },
    [ACTION_CHAR_TO_ASCII]     = { action_char_to_ascii,     DBG("CharToAscii", 4,     1, 1) },
    [ACTION_ASCII_TO_CHAR]     = { action_ascii_to_char,     DBG("AsciiToChar", 4,     1, 1) },
    [ACTION_GET_TIME]          = { action_get_time,          DBG("GetTime", 4,         0, 1) },
    [ACTION_MB_STRING_EXTRACT] = { action_mb_string_extract, DBG("MBStringExtract", 4, 3, 1) },
    [ACTION_MB_CHAR_TO_ASCII]  = { action_mb_char_to_ascii,  DBG("MBCharToAscii", 4,   1, 1) },
    [ACTION_MB_ASCII_TO_CHAR]  = { action_mb_ascii_to_char,  DBG("MBAsciiToChar", 4,   1, 1) },
    /* version 5 */
    [ACTION_DELETE]            = { action_delete,            DBG("Delete", 5,          2, 1) },
    [ACTION_DELETE2]           = { action_delete2,           DBG("Delete2", 5,         1, 1) },
    [ACTION_DEFINE_LOCAL]      = { action_define_local,      DBG("DefineLocal", 5,     2, 0) },
    [ACTION_CALL_FUNCTION]     = { action_call_function,     DBG("CallFunction", 5,   -1, 1) },
    [ACTION_RETURN]            = { action_return,            DBG("Return", 5,          1, 0) },
    [ACTION_MODULO]            = { action_modulo,            DBG("Modulo", 5,          2, 1) },
    [ACTION_NEW_OBJECT]        = { action_new_object,        DBG("NewObject", 5,      -1, 1) },
    [ACTION_DEFINE_LOCAL2]     = { action_define_local2,     DBG("DefineLocal2", 5,    1, 0) },
    [ACTION_INIT_ARRAY]        = { action_init_array,        DBG("InitArray", 5,      -1, 1) },
    [ACTION_INIT_OBJECT]       = { action_init_object,       DBG("InitObject", 5,     -1, 1) },
    [ACTION_TYPE_OF]           = { action_type_of,           DBG("TypeOf", 5,          1, 1) },
    [ACTION_TARGET_PATH]       = { action_target_path,       DBG("TargetPath", 5,      1, 1) },
    [ACTION_ENUMERATE]         = { action_enumerate,         DBG("Enumerate", 5,      1, -1) },
    [ACTION_ADD2]              = { action_add2,              DBG("Add2", 5,            2, 1) },
    [ACTION_LESS2]             = { action_less2,             DBG("Less2", 5,           2, 1) },
    [ACTION_EQUALS2]           = { action_equals2,           DBG("Equals2", 5,         2, 1) },
    [ACTION_TO_NUMBER]         = { action_to_number,         DBG("ToNumber", 5,        1, 1) },
    [ACTION_TO_STRING]         = { action_to_string,         DBG("ToString", 5,        1, 1) },
    [ACTION_PUSH_DUPLICATE]    = { action_push_duplicate,    DBG("PushDuplicate", 5,   1, 2) },
    [ACTION_SWAP]              = { action_swap,              DBG("Swap", 5,            2, 2) },
    /* version 4 */
    [ACTION_GET_MEMBER]        = { action_get_member,        DBG("GetMember", 4,       2, 1) },
    [ACTION_SET_MEMBER]        = { action_set_member,        DBG("SetMember", 4,       3, 0) },
    /* version 5 */
    [ACTION_INCREMENT]         = { action_increment,         DBG("Increment", 5,       1, 1) },
    [ACTION_DECREMENT]         = { action_decrement,         DBG("Decrement", 5,       1, 1) },
    [ACTION_CALL_METHOD]       = { action_call_method,       DBG("CallMethod", 5,     -1, 1) },
    [ACTION_NEW_METHOD]        = { action_new_method,        DBG("NewMethod", 5,      -1, 1) },
    /* version 6 */
    [ACTION_INSTANCE_OF]       = { action_instance_of,       DBG("InstanceOf", 6,      2, 1) },
    [ACTION_ENUMERATE2]        = { action_enumerate2,        DBG("Enumerate2", 6,     1, -1) },
    [ACTION_BREAKPOINT]        = { 0,                        DBG("Breakpoint", 6,    -1, -1) },
    /* version 5 */
    [ACTION_BIT_AND]           = { action_bitwise_and,       DBG("BitAnd", 5,          2, 1) },
    [ACTION_BIT_OR]            = { action_bitwise_or,        DBG("BitOr", 5,           2, 1) },
    [ACTION_BIT_XOR]           = { action_bitwise_xor,       DBG("BitXor", 5,          2, 1) },
    [ACTION_BIT_LSHIFT]        = { action_lshift,            DBG("BitLShift", 5,       2, 1) },
    [ACTION_BIT_RSHIFT]        = { action_rshift,            DBG("BitRShift", 5,       2, 1) },
    [ACTION_BIT_URSHIFT]       = { action_urshift,           DBG("BitURShift", 5,      2, 1) },
    /* version 6 */
    [ACTION_STRICT_EQUALS]     = { action_strict_equals,     DBG("StrictEquals", 6,    2, 1) },
    [ACTION_GREATER]           = { action_gt,                DBG("Greater", 6,         2, 1) },
    [ACTION_STRING_GREATER]    = { action_string_compare_gt, DBG("StringGreater", 6,   2, 1) },
    /* version 7 */
    [ACTION_EXTENDS]           = { action_extends,           DBG("Extends", 7,         2, 0) },
    /* version 1 ------------------------- vv --- with len --- vv -------------------------*/
    [ACTION_GOTO_FRAME]        = { action_goto_frame,        DBG("GotoFrame", 1,       0, 0) },
    [ACTION_GET_URL]           = { action_get_url,           DBG("GetURL", 1,          0, 0) },
    /* version 5 */
    [ACTION_STORE_REGISTER]    = { action_store_register,    DBG("StoreRegister", 5,   1, 1) },
    [ACTION_CONSTANT_POOL]     = { action_constant_pool,     DBG("ConstantPool", 5,    0, 0) },
    [ACTION_STRICT_MODE]       = { 0,                        DBG("StrictMode", 5,    -1, -1) },
    /* version 1 */
    [ACTION_WAIT_FOR_FRAME]    = { action_wait_for_frame,    DBG("WaitForFrame", 1,    0, 0) },
    [ACTION_SET_TARGET]        = { action_set_target,        DBG("SetTarget", 1,       0, 0) },
    /* version 3 */
    [ACTION_GOTO_LABEL]        = { action_goto_label,        DBG("GotoLabel", 3,       0, 0) },
    /* version 4 */
    [ACTION_WAIT_FOR_FRAME2]   = { action_wait_for_frame2,   DBG("WaitForFrame2", 4,   1, 0) },
    /* version 7 */
    [ACTION_DEFINE_FUNCTION2]  = { action_define_function2,  DBG("DefineFunction2", 7, 0, -1) },
    [ACTION_TRY]               = { action_try,               DBG("Try", 7,             0, 0) },
    /* version 5 */
    [ACTION_WITH]              = { action_with,              DBG("With", 5,            1, 0) },
    /* version 4 */
    [ACTION_PUSH]              = { action_push,              DBG("Push", 4,           0, -1) },
    [ACTION_JUMP]              = { action_jump,              DBG("Jump", 4,            0, 0) },
    [ACTION_GET_URL2]          = { action_get_url2,          DBG("GetURL2", 4,         2, 0) },
    /* version 5 */
    [ACTION_DEFINE_FUNCTION]   = { action_define_function,   DBG("DefineFunction", 5, 0, -1) },
    /* version 4 */
    [ACTION_IF]                = { action_if,                DBG("If", 4,              1, 0) },
    [ACTION_CALL]              = { 0,                        DBG("Call", 4,          -1, -1) },
    [ACTION_GOTO_FRAME2]       = { action_goto_frame2,       DBG("GotoFrame2", 4,      1, 0) },
    [ACTION_PLAY_LVG_SOUND]    = { action_play_lvg_sound,    DBG("PlayLVGSound", 0,    0, 0) },
};

void lvgInitVM(LVGActionCtx *ctx, LVGMovieClip *clip, LVGMovieClipGroup *group)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->clip   = clip;
    ctx->group  = group;
    ctx->frame  = ctx->group->frames + ctx->group->cur_frame;
    ctx->stack_ptr = sizeof(ctx->stack)/sizeof(ctx->stack[0]) - 1;
    ctx->version = clip->as_version;
}

void lvgFreeVM(LVGActionCtx *ctx)
{
    if (ctx->locals)
        free(ctx->locals);
    if (ctx->cpool)
        free(ctx->cpool);
    ctx->locals = NULL;
    ctx->cpool  = NULL;
}

void lvgExecuteActions(LVGActionCtx *ctx, uint8_t *actions, int is_function)
{
    if (!actions)
        return;
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
        Actions a = actions[ctx->pc++];
        int len = 0;
        if (a >= 0x80)
            len = *(uint16_t*)(actions + ctx->pc) + 2;
        const ActionEntry *ae = &g_avm1_actions[a];
        uint8_t *opdata = &actions[ctx->pc];
        ctx->pc += len;
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
        goto restart;
    }
}
