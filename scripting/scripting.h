#pragma once

typedef struct script_engine
{
    int (*init)(void **script, const char *file_name);
    void (*release)(void *script);
    void (*run_function)(void *script, const char *func_name);
} script_engine;
