#ifndef __rfxtypes_h__
#define __rfxtypes_h__

#include "../config.h"
#include <stdint.h>
#include <stddef.h>

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

/* little/big endian stuff */

#define PUT8(ptr,x) {((uint8_t*)(ptr))[0]=x;}
#define PUT16(ptr,x) {((uint8_t*)(ptr))[0]=(uint8_t)(x);((uint8_t*)(ptr))[1]=(uint8_t)((x)>>8);}
#define PUT32(ptr,x) {((uint8_t*)(ptr))[0]=(uint8_t)(x);((uint8_t*)(ptr))[1]=(uint8_t)((x)>>8);((uint8_t*)(ptr))[2]=(uint8_t)((x)>>16);((uint8_t*)(ptr))[3]=(uint8_t)((x)>>24);}
#define GET16(ptr) (((uint16_t)(((uint8_t*)(ptr))[0]))+(((uint16_t)(((uint8_t*)(ptr))[1]))<<8))
#define GET32(ptr) (((uint16_t)(((uint8_t*)(ptr))[0]))+(((uint16_t)(((uint8_t*)(ptr))[1]))<<8)+(((uint16_t)(((uint8_t*)(ptr))[2]))<<16)+(((uint16_t)(((uint8_t*)(ptr))[3]))<<24))

#define SWAP16(s) ((((s)>>8)&0x00ff)|(((s)<<8)&0xff00))
#define SWAP32(s) (SWAP16(((s)>>16)&0x0000ffff)|((SWAP16(s)<<16)&0xffff0000))

#ifdef WORDS_BIGENDIAN
#define LE_16_TO_NATIVE(s) SWAP16(s)
#define LE_32_TO_NATIVE(s) SWAP32(s)
#define BE_16_TO_NATIVE(x) (x)
#define BE_32_TO_NATIVE(x) (x)
#else
#define LE_16_TO_NATIVE(x) (x)
#define LE_32_TO_NATIVE(x) (x)
#define BE_16_TO_NATIVE(s) SWAP16(s)
#define BE_32_TO_NATIVE(s) SWAP32(s)
#endif

typedef ptrdiff_t ptroff_t;

#endif
