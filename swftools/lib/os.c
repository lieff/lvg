/* os.c

operating system dependent functions

Part of the swftools package. 

Copyright (c) 2005 Matthias Kramm <kramm@quiss.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include "os.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#undef HAVE_STAT
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#else
#undef HAVE_MMAP
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#undef HAVE_STAT
#endif

#if defined(CYGWIN)
char path_seperator = '/';
#elif defined(WIN32)
char path_seperator = '\\';
#else
char path_seperator = '/';
#endif

memfile_t* memfile_open(const char*path)
{
    memfile_t*file = malloc(sizeof(memfile_t));
#if defined(HAVE_MMAP) && defined(HAVE_STAT)
    int fi = open(path, O_RDONLY);
    if(fi<0) {
        perror(path);
        free(file);
        return 0;
    }
    struct stat sb;
    if(fstat(fi, &sb)<0) {
        perror(path);
        free(file);
        return 0;
    }
    file->len = sb.st_size;
    file->data = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fi, 0);
    close(fi);
#else
    FILE*fi = fopen(path, "rb");
    if(!fi) {
        perror(path);
        free(file);
        return 0;
    }
    fseek(fi, 0, SEEK_END);
    file->len = ftell(fi);
    fseek(fi, 0, SEEK_SET);
    file->data = malloc(file->len);
    if(!file->data) {
        fprintf(stderr, "Out of memory while allocating memory for file %s\n", path);
        free(file);
        return 0;
    }
    fread(file->data, file->len, 1, fi);
    fclose(fi);
#endif
    return file;
}

void memfile_close(memfile_t*file)
{
#if defined(HAVE_MMAP) && defined(HAVE_STAT)
    munmap(file->data, file->len);
#else
    free(file->data);
#endif
    file->data = 0;
    file->len = 0;
    free(file);
}

void move_file(const char*from, const char*to)
{
    int result = rename(from, to);

    if(result==0) return; //done!

    /* if we can't rename, for some reason, copy the file
       manually */
    FILE*fi = fopen(from, "rb");
    if(!fi) {
	perror(from);
	return;
    }
    FILE*fo = fopen(to, "wb");
    if(!fo) {
	perror(to);
	return;
    }
    char buffer[16384];
    while(1) {
	int bytes = fread(buffer, 1, 16384, fi);
	if(bytes<=0)
	    break;
	fwrite(buffer, bytes, 1, fo);
    }

    fclose(fo);
    fclose(fi);
    unlink(from);
}

char file_exists(const char*filename)
{
#ifdef HAVE_STAT
    struct stat sb;
    return stat(filename, &sb) >= 0;
#else
    int fi = open(filename, O_RDONLY);
    if(fi>=0) {
        close(fi);
        return 1;
    }
    return 0;
#endif
}

int file_size(const char*filename)
{
#ifdef HAVE_STAT
    struct stat sb;
    if(stat(filename, &sb) >= 0) {
        return sb.st_size;
    }
#endif
    FILE*fi = fopen(filename, "rb");
    if(fi>=0) {
        fseek(fi, 0, SEEK_END);
        int size = ftell(fi);
        fclose(fi);
        return size;
    }
    return 0;
}

int open_file_or_stdin(const char*filename, int attr)
{
    int f;
    if(strcmp(filename, "-")) {
        f = open(filename, attr);
    } else {
        f = 0; //stdin
    }
    return f;
}

