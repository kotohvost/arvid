#ifndef __COMPILER_H__
#define __COMPILER_H__

typedef struct {int mode,rows,cols,row_cursor,col_cursor;} scr_par;
extern scr_par spar;

#include <dos.h>
#include <conio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define _BORLAND_DOS_REGS 1
#define INTR_CF 1
#define interrupt
#include <go32.h>
#include <sys\farptr.h>
#include <sys\nearptr.h>
#include <sys\segments.h>
#include <unistd.h>
#ifdef __EMX__
#  include "emx/syscalls.h"
#endif
#else
#include <io.h>
#include <_pc.h>
#include <farptr.h>
#include <segments.h>
#include <movedata.h>
#endif
#endif
