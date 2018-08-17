#pragma once
#include <lvg_header.h>

typedef struct script_engine
{
    int (*init)(LVGEngine *e, void **script, const char *file_name);
    void (*release)(void *script);
    int (*run_function)(void *script, const char *func_name);
} script_engine;
