#pragma once
#include <stdint.h>
#ifdef EMSCRIPTEN
#define stat64 stat
#define fstat64 fstat
#endif

typedef struct __attribute__((__packed__)) zipLocalFileHeader_t
{
    uint32_t signature;
    uint16_t versionNeededToExtract;
    uint16_t generalPurposeBitFlag;
    uint16_t compressionMethod;
    uint16_t lastModFileTime;
    uint16_t lastModFileDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t extraFieldLength;
} zipLocalFileHeader_t;

typedef struct __attribute__((__packed__)) zipGlobalFileHeader_t
{
    uint32_t signature;
    uint16_t versionMadeBy;
    uint16_t versionNeededToExtract;
    uint16_t generalPurposeBitFlag;
    uint16_t compressionMethod;
    uint16_t lastModFileTime;
    uint16_t lastModFileDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t extraFieldLength;
    uint16_t fileCommentLength;
    uint16_t diskNumberStart;
    uint16_t internalFileAttributes;
    uint32_t externalFileAttributes;
    uint32_t relativeOffsetOflocalHeader;
} zipGlobalFileHeader_t;

typedef struct __attribute__((__packed__)) zipEndRecord_t
{
    uint32_t signature;
    uint16_t diskNumber;
    uint16_t centralDirectoryDiskNumber;
    uint16_t numEntriesThisDisk;
    uint16_t numEntries;
    uint32_t centralDirectorySize;
    uint32_t centralDirectoryOffset;
    uint16_t zipCommentLength;
} zipEndRecord_t;

typedef struct zip_t
{
    const char *buf;
    zipEndRecord_t *endRecord;
    size_t size;
} zip_t;

char *lvgOpenMap(const char *fname, size_t *size);
int lvgZipOpen(const char *m, size_t size, zip_t *zip);
void lvgZipClose(zip_t *zip);
uint32_t lvgZipNameLocate(zip_t *zip, const char *fname);
char *lvgZipDecompress(zip_t *zip, uint32_t file_ofs, uint32_t *size);
