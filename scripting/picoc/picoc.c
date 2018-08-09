#include "picoc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../src/lvg_header.h"

#define PICOC_STACK_SIZE (128*1024)              /* space for the the stack */

static void lib_lvgGetFileContents(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    if (2 != NumArgs)
        return;
    ReturnValue->Val->Pointer = lvgGetFileContents(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

static void lib_lvgLoadMP3(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    if (4 != NumArgs)
        return;
    ReturnValue->Val->Pointer = lvgLoadMP3(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Pointer, Param[3]->Val->Pointer);
}

static void lib_lvgPlaySound(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    if (5 != NumArgs)
        return;
    lvgPlaySound(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->Integer, Param[3]->Val->Integer, Param[4]->Val->Integer);
}

const char g_lvgDefs[] = "\
typedef struct LVGSound\
{\
    short *samples;\
    int num_samples; int orig_rate; int rate; int channels;\
} LVGSound;\
";

static const struct LibraryFunction g_lvgLib[] =
{
    { lib_lvgGetFileContents, "char *lvgGetFileContents(char *fname, int *size);" },
    { lib_lvgLoadMP3, "short *lvgLoadMP3(char *file, int *rate, int *channels, int *num_samples);" },
    { lib_lvgPlaySound, "void lvgPlaySound(LVGSound *sound, int flags, int start_sample, int end_sample, int loops);" },
    { NULL, NULL }
};

int loadScript()
{
    int StackSize = PICOC_STACK_SIZE;
    Picoc pc;

    PicocInitialise(&pc, StackSize);
    PicocIncludeAllSystemHeaders(&pc);
    PicocParse(&pc, "lvg.h", g_lvgDefs, strlen(g_lvgDefs), TRUE, TRUE, FALSE, FALSE);
    LibraryAdd(&pc, &pc.GlobalTable, "lvg library", &g_lvgLib[0]);
    //PicocParseInteractive(&pc);

    if (PicocPlatformSetExitPoint(&pc))
    {
        PicocCleanup(&pc);
        return pc.PicocExitValue;
    }

    PicocPlatformScanFile(&pc, "main.c");

    //if (!DontRunMain)
    //    PicocCallMain(&pc, argc - ParamCount, &argv[ParamCount]);

    PicocCleanup(&pc);
    return pc.PicocExitValue;
}
