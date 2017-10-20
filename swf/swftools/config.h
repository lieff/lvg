/* config.h.  Generated from config.h.in by configure.  */
#ifndef __config_h__
#define __config_h__

/* Define to 1 if this machine has network byte order*/
/* #undef WORDS_BIGENDIAN */

/* Define to 0 on non-windows systems */
#ifndef O_BINARY
#define O_BINARY 0
#endif

//#ifdef HAVE_BUILTIN_EXPECT
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
# define likely(x)      __builtin_expect((x), 1)
# define unlikely(x)    __builtin_expect((x), 0)
#else
# define likely(x)      (x)
# define unlikely(x)    (x)
#endif

#endif
