#ifndef __MYTYPES_H__
#define __MYTYPES_H__

typedef unsigned char  byte;
typedef unsigned short word;
typedef volatile int   vint;
typedef unsigned int  dword;

#define TRUE  true
#define FALSE false
#define MAX_PATH 128
#define FILE_ACCESS 0666

#ifdef __GNUC__
static inline void brk () { asm ("int $3"); }
#else
static inline void brk (void) { _asm {int 3}}
#endif // __GNUC__

#endif