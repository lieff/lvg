#pragma once
#include <stdint.h>
#include <../render/render.h>

#define STACK_SIZE 4096
#define SET_STRING(se, string) { (se)->type = ASVAL_STRING; (se)->str = string; }
#define SET_DOUBLE(se, number) { (se)->type = ASVAL_DOUBLE; (se)->d_int = number; }
#define SET_INT(se, number)    { (se)->type = ASVAL_INT;    (se)->i32 = number; }
#define SET_BOOL(se, value)    { (se)->type = ASVAL_BOOL;   (se)->boolean = value; }
#define SET_UNDEF(se)          { (se)->type = ASVAL_UNDEFINED; (se)->str = 0; }
#define SET_CLASS(se, cls_val) { (se)->type = ASVAL_CLASS;  (se)->cls = cls_val; }

typedef enum {
    ACTION_END = 0x00,
    ACTION_NEXT_FRAME = 0x04,
    ACTION_PREVIOUS_FRAME = 0x05,
    ACTION_PLAY = 0x06,
    ACTION_STOP = 0x07,
    ACTION_TOGGLE_QUALITY = 0x08,
    ACTION_STOP_SOUNDS = 0x09,
    ACTION_ADD = 0x0A,
    ACTION_SUBTRACT = 0x0B,
    ACTION_MULTIPLY = 0x0C,
    ACTION_DIVIDE = 0x0D,
    ACTION_EQUALS = 0x0E,
    ACTION_LESS = 0x0F,
    ACTION_AND = 0x10,
    ACTION_OR = 0x11,
    ACTION_NOT = 0x12,
    ACTION_STRING_EQUALS = 0x13,
    ACTION_STRING_LENGTH = 0x14,
    ACTION_STRING_EXTRACT = 0x15,
    ACTION_POP = 0x17,
    ACTION_TO_INTEGER = 0x18,
    ACTION_GET_VARIABLE = 0x1C,
    ACTION_SET_VARIABLE = 0x1D,
    ACTION_SET_TARGET2 = 0x20,
    ACTION_STRING_ADD = 0x21,
    ACTION_GET_PROPERTY = 0x22,
    ACTION_SET_PROPERTY = 0x23,
    ACTION_CLONE_SPRITE = 0x24,
    ACTION_REMOVE_SPRITE = 0x25,
    ACTION_TRACE = 0x26,
    ACTION_START_DRAG = 0x27,
    ACTION_END_DRAG = 0x28,
    ACTION_STRING_LESS = 0x29,
    ACTION_THROW = 0x2A,
    ACTION_CAST = 0x2B,
    ACTION_IMPLEMENTS = 0x2C,
    ACTION_RANDOM = 0x30,
    ACTION_MB_STRING_LENGTH = 0x31,
    ACTION_CHAR_TO_ASCII = 0x32,
    ACTION_ASCII_TO_CHAR = 0x33,
    ACTION_GET_TIME = 0x34,
    ACTION_MB_STRING_EXTRACT = 0x35,
    ACTION_MB_CHAR_TO_ASCII = 0x36,
    ACTION_MB_ASCII_TO_CHAR = 0x37,
    ACTION_DELETE = 0x3A,
    ACTION_DELETE2 = 0x3B,
    ACTION_DEFINE_LOCAL = 0x3C,
    ACTION_CALL_FUNCTION = 0x3D,
    ACTION_RETURN = 0x3E,
    ACTION_MODULO = 0x3F,
    ACTION_NEW_OBJECT = 0x40,
    ACTION_DEFINE_LOCAL2 = 0x41,
    ACTION_INIT_ARRAY = 0x42,
    ACTION_INIT_OBJECT = 0x43,
    ACTION_TYPE_OF = 0x44,
    ACTION_TARGET_PATH = 0x45,
    ACTION_ENUMERATE = 0x46,
    ACTION_ADD2 = 0x47,
    ACTION_LESS2 = 0x48,
    ACTION_EQUALS2 = 0x49,
    ACTION_TO_NUMBER = 0x4A,
    ACTION_TO_STRING = 0x4B,
    ACTION_PUSH_DUPLICATE = 0x4C,
    ACTION_SWAP = 0x4D,
    ACTION_GET_MEMBER = 0x4E,
    ACTION_SET_MEMBER = 0x4F,
    ACTION_INCREMENT = 0x50,
    ACTION_DECREMENT = 0x51,
    ACTION_CALL_METHOD = 0x52,
    ACTION_NEW_METHOD = 0x53,
    ACTION_INSTANCE_OF = 0x54,
    ACTION_ENUMERATE2 = 0x55,
    ACTION_BREAKPOINT = 0x5F,
    ACTION_BIT_AND = 0x60,
    ACTION_BIT_OR = 0x61,
    ACTION_BIT_XOR = 0x62,
    ACTION_BIT_LSHIFT = 0x63,
    ACTION_BIT_RSHIFT = 0x64,
    ACTION_BIT_URSHIFT = 0x65,
    ACTION_STRICT_EQUALS = 0x66,
    ACTION_GREATER = 0x67,
    ACTION_STRING_GREATER = 0x68,
    ACTION_EXTENDS = 0x69,
    ACTION_GOTO_FRAME = 0x81,
    ACTION_GET_URL = 0x83,
    ACTION_STORE_REGISTER = 0x87,
    ACTION_CONSTANT_POOL = 0x88,
    ACTION_STRICT_MODE = 0x89,
    ACTION_WAIT_FOR_FRAME = 0x8A,
    ACTION_SET_TARGET = 0x8B,
    ACTION_GOTO_LABEL = 0x8C,
    ACTION_WAIT_FOR_FRAME2 = 0x8D,
    ACTION_DEFINE_FUNCTION2 = 0x8E,
    ACTION_TRY = 0x8F,
    ACTION_WITH = 0x94,
    ACTION_PUSH = 0x96,
    ACTION_JUMP = 0x99,
    ACTION_GET_URL2 = 0x9A,
    ACTION_DEFINE_FUNCTION = 0x9B,
    ACTION_IF = 0x9D,
    ACTION_CALL = 0x9E,
    ACTION_GOTO_FRAME2 = 0x9F,
    ACTION_PLAY_LVG_SOUND = 0xA0
} Actions;

typedef enum {
    ASVAL_STRING, ASVAL_FLOAT, ASVAL_DOUBLE, ASVAL_BOOL, ASVAL_INT, ASVAL_NULL, ASVAL_UNDEFINED, ASVAL_CLASS, ASVAL_FUNCTION, ASVAL_NATIVE_FN
} ASValType;

typedef struct ASClass ASClass;
typedef void (*as_native_fn)(LVGActionCtx *ctx, ASClass *cls, uint8_t *a, uint32_t nargs);

typedef struct ASVal
{
    union {
        const char *str;
        as_native_fn fn;
        ASClass *cls;
        float f_int;
        double d_int;
        uint32_t ui32;
        int32_t i32;
        int boolean;
    };
    ASValType type;
} ASVal;

typedef struct ASMember
{
    const char *name;
    ASVal val;
} ASMember;

typedef struct ASClass
{
    const char *name;
    ASMember *members;
    void *priv;
    int num_members;
} ASClass;

typedef struct LVGActionCall
{
    int save_pc;
    int save_size;
} LVGActionCall;

typedef struct LVGActionCtx
{
    LVGMovieClip *clip;
    LVGMovieClipGroup *group;
    LVGMovieClipFrame *frame;
    ASVal stack[STACK_SIZE];
    ASVal regs[256];
    LVGActionCall calls[256];
    ASMember *locals;
    const char **cpool;
    uint8_t *actions;
    int size, version, stack_ptr, cpool_size, pc, call_depth, do_exit, num_locals;
} LVGActionCtx;

extern ASVal g_classes[];
extern int g_num_classes;
extern ASMember g_properties[];
extern int g_num_properties;
extern ASClass g_movieclip;

double to_double(ASVal *v);
int32_t to_int(ASVal *v);
ASVal *find_class_member(ASClass *c, const char *name);
ASVal *create_local(LVGActionCtx *ctx, const char *name);
ASClass *create_instance(ASClass *base);
void free_instance(ASClass *cls);
