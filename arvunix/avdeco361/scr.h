/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Serge Vakulenko not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Serge Vakulenko makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * SERGE VAKULENKO DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SERGE VAKULENKO BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# ifndef cntrl
#    define cntrl(c)    (c & 037)
# endif
# ifndef meta
#    define meta(c)     (c | 0400)
# endif

typedef struct {
	short cury, curx;
	short flgs;
	char clear;
	short **y;
	short *firstch;
	short *lastch;
	short *lnum;
} WINDOW;

typedef struct {
	short y, x;
	short ny, nx;
	short *mem;
} BOX;

typedef long CURSOR;

extern LINES, COLS;
extern WINDOW VScreen;

/*------------------------------------------------*/
class V {
void	initcolor(void);
public:
int	VInit(void);
void	VOpen(void);
void	VClose(void);
void	VSetPalette(int n, int nb, int nr, int nrb, \
			int b, int bb, int br, int brb, \
			int d, int db, int dr, int drb);
};
extern V v;

void VReopen(void);
void VRestore(void);
void VFlush(void);
void VSync(void);
void VBeep(void);
void VRedraw(void);
int VSetDim(void);
int VSetBold(void);
void VSetNormal(void);
int VStandOut(void);
void VStandEnd(void);
void VMove(register y, register x);
void VClear(void);
void VClearBox(int r, int c, int nr, int nc);
void VClearLine(void);
void VPutChar(register c);
void VPutString(register char *str);
BOX *VGetBox(int y, int x, int ny, int nx);
void VFillBox(int r, int c, int nr, int nc, int sym);
void VDrawBox(register r, register c, register nr, register nc);
void VPrintBox(BOX *box);
void VUngetBox(BOX *box);
void VFreeBox(BOX *box);
void VHorLine (int r, register c, register nc);
void VInsLine (int n);
void VDelLine (int n);
CURSOR VGetCursor(void);
void VSetCursor(CURSOR c);
void VExpandString(register char *s, register char *d);
void VSetPrev (void);
void VVertLine (int c, register r, register nr);
void VCorner (int r, int c, register n);

# define VMPutChar(y,x,ch) (VMove (y,x), VPutChar (ch))
# define VMPutString(y,x,str) (VMove (y,x), VPutString (str))
# define VMGetChar(yy,x) (VMove (yy,x), (VScreen.y[VScreen.cury][VScreen.curx] & 0377))

/*------------------------------------------------*/
