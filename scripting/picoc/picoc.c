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

static const struct LibraryFunction g_lvgLib[] =
{
    { lib_lvgGetFileContents, "char *lvgGetFileContents(char *fname, int *size);" },
    { NULL, NULL }
};

int loadScript()
{
    int StackSize = PICOC_STACK_SIZE;
    Picoc pc;

    PicocInitialise(&pc, StackSize);
    PicocIncludeAllSystemHeaders(&pc);
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
