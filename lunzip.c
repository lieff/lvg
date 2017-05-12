#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <lunzip.h>
#include "stb_image.h"

int lvgZipOpen(const char *fname, zip_t *zip)
{
    struct stat64 st;
    int fd = open(fname, O_RDONLY);
    if (fd < 0 || fstat64(fd, &st) < 0)
        return -1;
    size_t size = st.st_size;
    char *m = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (*(int32_t*)m != 0x04034B50)
        goto error;
    int i;
    zipEndRecord_t *er;
    for (i = size - sizeof(zipEndRecord_t); i >= 0; i--)
    {
        er = (zipEndRecord_t *)(m + i);
        if (er->signature == 0x06054B50)
            break;
    }
    if (i == 0 || er->diskNumber || er->centralDirectoryDiskNumber || er->numEntries != er->numEntriesThisDisk)
        goto error;
    zip->file = fd;
    zip->buf = m;
    zip->endRecord = er;
    zip->size = size;
    return 0;
error:
    close(fd);
    return -1;
}

void lvgZipClose(zip_t *zip)
{
    if (zip->buf)
    {
        munmap(zip->buf, zip->size);
        zip->buf = 0;
    }
    if (zip->file > 0)
    {
        close(zip->file);
        zip->file = 0;
    }
}

uint32_t lvgZipNameLocate(zip_t *zip, const char *fname)
{
    if (!zip || zip->endRecord->centralDirectoryOffset > zip->size)
        return -1;
    size_t flen = strlen(fname);
    zipGlobalFileHeader_t *fh = (zipGlobalFileHeader_t *)(zip->buf + zip->endRecord->centralDirectoryOffset);
    for (int i = 0; i < zip->endRecord->numEntries; i++)
    {
        if (fh->signature != 0x02014B50)
            return -1;
        if (fh->fileNameLength == flen && !strncmp((char *)(fh + 1), fname, fh->fileNameLength))
            return fh->relativeOffsetOflocalHeader;
        fh = (zipGlobalFileHeader_t *)((char *)(fh + 1) + fh->fileNameLength + fh->extraFieldLength + fh->fileCommentLength);
    }
    return -1;
}

char *lvgZipDecompress(zip_t *zip, uint32_t file_ofs, uint32_t *size)
{
    zipLocalFileHeader_t *fh = (zipLocalFileHeader_t *)(zip->buf + file_ofs);
    if (fh->signature != 0x04034B50)
        return 0;
    char *c_data = (char *)(fh + 1) + fh->fileNameLength + fh->extraFieldLength;
    char *u_data = (char *)malloc(fh->uncompressedSize + 1);
    u_data[fh->uncompressedSize] = 0;
    if (0 == fh->compressionMethod)
    {
        if ((fh->compressedSize != fh->uncompressedSize))
            return 0;
        memcpy(u_data, c_data, fh->uncompressedSize);
    } else
        stbi_zlib_decode_noheader_buffer(u_data, fh->uncompressedSize, c_data, fh->compressedSize);
    u_data[fh->uncompressedSize] = 0; // automatic zero-terminate
    if (size)
        *size = fh->uncompressedSize;
    return u_data;
}
