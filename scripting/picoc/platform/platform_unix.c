#include "../picoc.h"
#include "../interpreter.h"

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* mark where to end the program for platforms which require this */
jmp_buf PicocExitBuf;

#ifndef NO_DEBUGGER
#include <signal.h>

Picoc *break_pc = NULL;

static void BreakHandler(int Signal)
{
    break_pc->DebugManualBreak = TRUE;
}

void PlatformInit(Picoc *pc)
{
    /* capture the break signal and pass it to the debugger */
    break_pc = pc;
    signal(SIGINT, BreakHandler);
}
#else
void PlatformInit(Picoc *pc)
{
}
#endif

void PlatformCleanup(Picoc *pc)
{
}

/* get a line of interactive input */
char *PlatformGetLine(char *Buf, int MaxLen, const char *Prompt)
{
#ifdef USE_READLINE
    if (Prompt != NULL)
    {
        /* use GNU readline to read the line */
        char *InLine = readline(Prompt);
        if (InLine == NULL)
            return NULL;

        Buf[MaxLen-1] = '\0';
        strncpy(Buf, InLine, MaxLen-2);
        strncat(Buf, "\n", MaxLen-2);

        if (InLine[0] != '\0')
            add_history(InLine);

        free(InLine);
        return Buf;
    }
#endif

    if (Prompt != NULL)
        printf("%s", Prompt);

    fflush(stdout);
    return fgets(Buf, MaxLen, stdin);
}

/* get a character of interactive input */
int PlatformGetCharacter()
{
    fflush(stdout);
    return getchar();
}

/* write a character to the console */
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *Stream)
{
    putchar(OutCh);
}

/* exit the program */
void PlatformExit(Picoc *pc, int RetVal)
{
    pc->PicocExitValue = RetVal;
    longjmp(pc->PicocExitBuf, 1);
}

