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

/*
 *      int KeyInit (map, flushproc)
 *      struct KeyMap *map;
 *      int (*flushproc) ();
 *
 *              - set key map table.
 *              Flushproc is called before actual reading from terminal.
 *              Returns 1 if ok else 0.
 *
 *      int KeyGet ()
 *
 *              - reads key from terminal.
 *              Returns key (always >= 0).
 *
 *      int KeyGetChar ()
 *
 *              - reads char from terminal.
 *              Returns char (always >= 0).
 */

# include "key.h"
# include "cap.h"
# include "mem.h"
#include "tty.h"

static struct KeyMap *km;
static void (*flush) ();

# ifndef SIGTYPE
#    define SIGTYPE int
# endif

# ifdef __MSDOS__

# define CAP(a,b) ((a)<<8|(b))

# define KF1    0
# define KF2    1
# define KF3    2
# define KF4    3
# define KF5    4
# define KF6    5
# define KF7    6
# define KF8    7
# define KF9    8
# define KF10   9
# define KL     10
# define KR     11
# define KU     12
# define KD     13
# define KH     14
# define K1     15
# define KP     16
# define KN     17
# define K0     18
# define K_     19

static short fkey [20];
static remap [041];

void KeyInit(struct KeyMap *map, void (*flushproc)(void)) {
	register struct KeyMap *kp;
	register i;

	km = map;
	flush = flushproc;
	for (i=0; i<sizeof(remap)/sizeof(remap[0]); ++i)
		remap[i] = i;
	remap[040] = '\177';
	for (i=0; i<sizeof(fkey)/sizeof(fkey[0]); ++i)
		fkey[i] = 0;
	for (kp= &km[0]; kp->val; ++kp)
		if (kp->tcap) switch (CAP (kp->tcap[0], kp->tcap[1])) {
		case CAP ('f', '1'):    fkey [KF1] = kp->val;   break;
		case CAP ('f', '2'):    fkey [KF2] = kp->val;   break;
		case CAP ('f', '3'):    fkey [KF3] = kp->val;   break;
		case CAP ('f', '4'):    fkey [KF4] = kp->val;   break;
		case CAP ('f', '5'):    fkey [KF5] = kp->val;   break;
		case CAP ('f', '6'):    fkey [KF6] = kp->val;   break;
		case CAP ('f', '7'):    fkey [KF7] = kp->val;   break;
		case CAP ('f', '8'):    fkey [KF8] = kp->val;   break;
		case CAP ('f', '9'):    fkey [KF9] = kp->val;   break;
		case CAP ('f', '0'):    fkey [KF10] = kp->val;  break;
		case CAP ('k', 'l'):    fkey [KL] = kp->val;    break;
		case CAP ('k', 'r'):    fkey [KR] = kp->val;    break;
		case CAP ('k', 'u'):    fkey [KU] = kp->val;    break;
		case CAP ('k', 'd'):    fkey [KD] = kp->val;    break;
		case CAP ('k', 'h'):    fkey [KH] = kp->val;    break;
		case CAP ('k', '1'):    fkey [K1] = kp->val;    break;
		case CAP ('k', 'N'):    fkey [KN] = kp->val;    break;
		case CAP ('k', 'P'):    fkey [KP] = kp->val;    break;
		case CAP ('k', '0'):    fkey [K0] = kp->val;    break;
		case CAP ('k', '.'):    fkey [K_] = kp->val;    break;
		} else if (kp->str)
			if (kp->str[0]=='\177')
				remap [040] = kp->val;
			else if (kp->str[0]>=1 && kp->str[0]<=037)
				remap [kp->str[0]] = kp->val;
}

int KeyGet () {
	register c;

	if (flush)
		(*flush) ();

	if (c = getch ()) {
		if (c == '\177')
			return (remap [040]);
		else if (c>=1 && c<=037)
			return (remap [c]);
		return (c);
	}
	switch (c = getch ()) {
	case ';':       return (fkey [KF1]);
	case '<':       return (fkey [KF2]);
	case '=':       return (fkey [KF3]);
	case '>':       return (fkey [KF4]);
	case '?':       return (fkey [KF5]);
	case '@':       return (fkey [KF6]);
	case 'A':       return (fkey [KF7]);
	case 'B':       return (fkey [KF8]);
	case 'C':       return (fkey [KF9]);
	case 'D':       return (fkey [KF10]);
	case 'K':       return (fkey [KL]);
	case 'M':       return (fkey [KR]);
	case 'H':       return (fkey [KU]);
	case 'P':       return (fkey [KD]);
	case 'G':       return (fkey [KH]);
	case 'O':       return (fkey [K1]);
	case 'Q':       return (fkey [KN]);
	case 'I':       return (fkey [KP]);
	case 'R':       return (fkey [K0]);
	case 'S':       return (fkey [K_]);
	}
	return (0);
}

# else /* MSDOS */

#include "stdlib.h"
#include "unistd.h"
# include <setjmp.h>
# include <signal.h>

# define NKEY   100

static jmp_buf wakeup;
static keyback;
static unsigned oldalarm;

# ifdef CYRILLIC
static cyrinput;
extern char VCyrInputTable [];
# endif

/*
 *      Compare keys. Used in call to qsort.
 *      Return -1, 0, 1 iff a is less, equal or greater than b.
 *      First, check if there is ->str.
 *      Then compare lengths, then strings.
 *      If equal, check ->tcap field.
 */
/* local */
int compkeys (register struct KeyMap *a, register struct KeyMap *b) {
	register cmp;

	if (! a->str) {
		if (! b->str)
			return (0);
		return (1);
	}
	if (! b->str)
		return (-1);
	cmp = strcmp (a->str, b->str);
	if (cmp)
		return (cmp);
	if (! a->tcap) {
		if (! b->tcap)
			return (0);
		return (1);
	}
	if (! b->tcap)
		return (-1);
	return (0);
}

/* main */
void KeyInit (struct KeyMap *map, void (*flushproc)(void)){
	struct CapTab tab [NKEY];
	register struct KeyMap *kp;
	register struct CapTab *t;

	km = map;
	flush = flushproc;
	for (t=tab, kp=km; kp->val && t<tab+NKEY-1; ++kp, ++t) {
		if (! kp->tcap)
			continue;
		t->tname[0] = kp->tcap[0];
		t->tname[1] = kp->tcap[1];
		t->ttype = CAPSTR;
		t->tdef = 0;
		t->tc = 0;
		t->ti = 0;
		t->ts = &kp->str;
	}
	kp->val = 0;
	t->tname[0] = 0;
	CapGet (tab);
	qsort ((char *) km, (unsigned) (kp - km), sizeof (km[0]), compkeys);
# ifdef notdef
	{
		register struct KeyMap *p;
		for (p=km; p<kp; ++p)
			if (kp->str)
				printf ("'\\%03o%s' -> %x\n", p->str[0], p->str+1, p->val);
	}
# endif
}

/* edit */
int KeyGetChar (void) {
	char c;

	while (read (0, &c, 1) != 1)
		if (! isatty (0))
			exit (0);
	return (c & 0377);
}

/* local */
void badkey(int sig) {
	alarm(oldalarm);
	longjmp(wakeup, -1);
}

/* local */
static nmgetch (register struct KeyMap *kp) {
	register mach, c;
	register struct KeyMap *lp;

	for (mach=1; ; ++mach) {
		if (! kp->str [mach])
			return (kp->val);
		c = KeyGetChar ();
		if (kp->str [mach] == c)
			continue;
		lp = kp;
		do {
			++kp;
			if (! kp->str)
				goto unknown;
		} while (kp->str [mach] != c);
		if (lp->str [mach-1] != kp->str [mach-1])
			goto unknown;
# ifdef HARDKEYS
		if (mach>1 && strncmp (lp->str, kp->str, mach-1))
			goto unknown;
# endif
	}
unknown:
	alarm (oldalarm);
	longjmp (wakeup, 1);
	/* NOTREACHED */
}

/* coice, com, edit, ex, help, main, menu, view */
int KeyGet () {
	register struct KeyMap *kp;
	register c, j;

	if (keyback) {
		c = keyback;
		keyback = 0;
		return (c);
	}
	if (flush)
		(*flush) ();
nextkey:
	c = KeyGetChar ();
	for (kp=km; kp->str; ++kp)
		if ((char) c == kp->str[0])
			break;
	if (! kp->str) {
# ifdef DOC
		if (c == cntrl ('_')) {
			_prscreen ();
			goto nextkey;
		}
# endif
# ifdef CYRILLIC
		/*
		 * Handle cyrillic input.
		 * ^N turns on cyrillic input mode,
		 * after that all input characters
		 * in range ' '...'~' must be recoded
		 * by special cyrillic input table.
		 * It is specified by Ct termcap descriptor.
		 * ^O turns off cyrillic input mode.
		 */
		if (c == cntrl ('N')) {
			cyrinput = 1;
			goto nextkey;
		} else if (c == cntrl ('O')) {
			cyrinput = 0;
			goto nextkey;
		} else if (cyrinput && c>=' ' && c<='~')
			c = VCyrInputTable [c - ' '] & 0xff;
# endif
		return (c);
	}
	if (! kp->str [1])
		return (kp->val);
	/* look for escape sequence */
	if (j = setjmp (wakeup)) {
		/* time out or unknown escape sequence */
		TtyFlushInput ();
		if (j < 0)
			return (c);
		goto nextkey;
	}
	signal (SIGALRM, badkey);
	oldalarm = alarm (2);
	c = nmgetch (kp);
	alarm (oldalarm);
	return (c);
}

/* com, main */
void KeyUnget(int key) {
	keyback = key;
}

# endif /* MSDOS */
