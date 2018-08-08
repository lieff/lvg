#include "picoc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PICOC_STACK_SIZE (128*1024)              /* space for the the stack */

int loadScript()
{
    int ParamCount = 1;
    //int DontRunMain = FALSE;
    int StackSize = PICOC_STACK_SIZE;
    char *buf;
    Picoc pc;

    PicocInitialise(&pc, StackSize);
    PicocIncludeAllSystemHeaders(&pc);
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
