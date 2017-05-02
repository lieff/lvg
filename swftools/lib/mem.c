#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mem.h"

void* rfx_calloc(int size)
{
  void*ptr;
  if (size == 0) {
    return 0;
  }
#ifdef HAVE_CALLOC
  ptr = calloc(1, size);
#else
  ptr = malloc(size);
#endif
  if(!ptr) {
    fprintf(stderr, "FATAL: Out of memory (while trying to claim %d bytes)\n", size);
    exit(1);
  }
#ifndef HAVE_CALLOC
  memset(ptr, 0, size);
#endif
  return ptr;
}


