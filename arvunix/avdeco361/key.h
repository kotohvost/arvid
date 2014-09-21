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

struct KeyMap {
	char *tcap;
	char *str;
	short val;
};

void KeyInit(struct KeyMap *map, void (*flushproc)(void));
void KeyUnget(int key);
int KeyGet(void);
int KeyGetChar (void);
