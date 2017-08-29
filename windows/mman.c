#include <windows.h>
#include <io.h>
#include "mman.h"

void *mmap(void *addr, size_t length, int prot, int flags, int fd, size_t offset)
{
    HANDLE mapping = CreateFileMapping((HANDLE)_get_osfhandle(fd), NULL, PAGE_READONLY, 0, offset, NULL);
    if (!mapping)
        return 0;
    void *map = MapViewOfFile(mapping, FILE_MAP_READ, 0, offset, length);
    CloseHandle(mapping);
    return map;
}

int munmap(void *addr, size_t length)
{
    if (UnmapViewOfFile(addr))
        return 0;
    return -1;
}
