#pragma once

#define PROT_READ       1
#define MAP_PRIVATE     2

#ifdef __cplusplus
extern "C" {
#endif

void *mmap(void *addr, size_t length, int prot, int flags, int fd, size_t offset);
int munmap(void *addr, size_t length);

#ifdef __cplusplus
}
#endif
