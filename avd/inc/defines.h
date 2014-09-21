
#ifndef __DEFINES_H__
#define __DEFINES_H__

/* I386 assembler */
typedef unsigned char  byte;
typedef unsigned short word;
typedef volatile int   vint;
typedef unsigned int  dword;

#ifdef __GNUC__
typedef unsigned long long ddword;
#endif

#ifndef __cplusplus
typedef unsigned char bool;
#endif

#ifdef unix
#define stricmp strcasecmp
#endif

#ifndef true
# define true 1
#endif

#ifndef false
# define false 0
#endif

#ifndef TRUE
# define TRUE  true
#endif

#ifndef FALSE
# define FALSE false
#endif

#ifndef MAX_PATH
# define MAX_PATH 256
#endif

#ifdef __GNUC__
# ifndef O_BINARY
#  define O_BINARY 0
# endif
#endif

#define FILE_ACCESS 0666
#define ERROR	    -1
#define ONE_SECOND  1000000 /* microseconds in second */

#endif
