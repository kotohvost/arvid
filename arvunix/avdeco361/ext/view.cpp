/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include "unistd.h"

# include "scr.h"
# include "ut.h"
# include "choice.h"
# include "key.h"
# include "help.h"
# include "view.h"
# include "mem.h"
# include "main.h"

int viewraw, viewhex, viewtabs;		/* modes */
long viewseek;                  /* current seek for hex/text switches */
int viewhlen;                   /* number of bytes */
char	viewsrch [SEARCHSZ];		/* search string */
//char viewsbuf [SEARCHSZ];       /* search string buffer */
int viewsbsz;                   /* search string size */
//REC *viewrec;

void VIEWFILE::viewchar(register c) {
	register hibit;

	if (c == ' ') {
		if (viewhex || viewtabs) {
			VSetDim ();
			VPutChar ('.');
			VSetNormal ();
		} else
			VPutChar (' ');
	} else if (viewraw) {
		c &= 0377;
		if (c >= ' ')
			VPutChar (c);
		else {
			VSetDim ();
			VPutChar ((c + 0100) & 0177);
			VSetNormal ();
		}
	} else {
		if (hibit = (c & 0200))
			VStandOut ();
		c &= 0177;
		if (c >= ' ' && c <= '~')
			VPutChar (c);
		else {
			VSetDim ();
			VPutChar ((c + 0100) & 0177);
			VSetNormal ();
		}
		if (hibit)
			VStandEnd ();
	}
}

/*
 * Line L is to be printed on screen, shifted by N colunms.
 * Compute the line offset to start with.
 */
int VIEWFILE::offset(int l, register n) {
	LINE *p;
	register char *s, *e;
	register len;

	if (! (p = rec.RecGet(viewrec, l)))
		return (0);
	s = p->ptr;
	e = s + p->len;
	for (len=0; len<n && s<e; ++s) {
		if (*s == '\t') {       /* handle tabs correctly */
			len = (len + 8) / 8 * 8;
			continue;
		}
		if (*s == '\b' && !viewtabs && len>0 && s<e-1 &&
		    s[-1] != '\b' && s[1] != '\b') {
			--len;
			continue;
		}
		++len;
		if (len > n)
			break;
	}
	return (s - p->ptr);
}

/*
 * N bytes of line L are printed on screen.
 * Compute the number of columns it will take.
 */
int VIEWFILE::column(int l, int n) {
	LINE *p;
	register char *s, *e;
	register len;

	if (! (p = rec.RecGet(viewrec, l)))
		return (0);
	if (n > p->len)
		n = p->len;
	s = p->ptr;
	e = s + n;
	/* count length of string as it appears on screen */
	for (len=0; s<e; ++s)
		if (*s == '\t')         /* handle tabs correctly */
			len = (len + 8) / 8 * 8;
		else if (*s == '\b' && !viewtabs && len>0 && s<e-1 &&
		    s[-1] != '\b' && s[1] != '\b')
			--len;
		else
			++len;
	return (len);
}

void VIEWFILE::viewline(int line, register col, int scrline) {
	register LINE *l;
	register char *p;
	char *e;
	register len;

	VMove (scrline, 0);                     /* move to line */
	VClearLine ();                          /* clear it */
	l = rec.RecGet(viewrec, line);
	if (! l || ! l->len)                    /* get line */
		return;                         /* check if empty */
	p = l->ptr;                             /* set ptr to line */
	e = p + l->len;                         /* set ptr to end */
	len = COLS;                             /* width of print */
	if (col)
		p += offset (line, col);
	if (p >= e)
		return;
	while (--len >= 0 && p < e)             /* print char loop */
		if (*p == '\t') {               /* print tab */
			if (viewtabs) {
				VSetDim ();
				VPutChar ('_');
				VSetNormal ();
			}
			len = len / 8 * 8;
			VMove (scrline, COLS-len);
			++p;
		} else if (*p == '\b' && !viewtabs && len<COLS-1 && p<e-1 &&
		    p[-1] != '\b' && p[1] != '\b') {
			VPutChar ('\b');        /* backspace */
			++len;
			++p;
			VSetBold ();
			viewchar (*p++);
			VSetNormal ();
		} else
			viewchar (*p++);        /* else print char */
	if (p < e) {                            /* put tag */
		if (len < 0)
			VMove (scrline, COLS-1);
		VSetBold ();
		VPutChar ('>');                 /* "continued" */
		VSetNormal ();
	} else if (l->noeoln) {
		VSetBold ();
		VPutChar ('\\');                /* "broken" */
		VSetNormal ();
	}
}

void VIEWFILE::viewpage(int line, int col) {
	register i;
	int len;

	len = viewrec->len - line;
	if (len > LINES-2)
		len = LINES-2;
	for (i=0; i<len; ++i)
		viewline (i+line, col, i+1);
}

void VIEWFILE::viewhead(char *filename, int line, int col) {
	register i;

	VSetDim ();
	VStandOut ();
	VMove (0, 0);
	for (i=80; --i>=0; VPutChar (' '));
	VMPutString (0, 1, "File ");
	VPutString (filename);
	printw ("  Lines %-5d Bytes %ld", viewrec->len, viewrec->size);
	VMove (0, 57);
	printw ("Line %-5d Col %d", line+1, col);
	VSetNormal ();
	VStandEnd ();
}

int VIEWFILE::strequal (register char *a, register char *b, register n) {
	for (;;) {
		if (--n < 0)
			return (1);
		if (*a++ != *b++)
			return (0);
	}
}

int VIEWFILE::search (char *str, int l, int c, int *pline, int *pcol) {
	LINE *p;
	register char *s, *e;
	int len = strlen (str);

	for (; l<viewrec->len && (p = rec.RecGet(viewrec, l)); ++l) {
		s = p->ptr;
		e = p->ptr + p->len - len + 1;
		if (c) {
			s += c;
			c = 0;
		}
		for (; s<e; ++s)
			if (strequal (s, str, len)) {
				*pline = l;
				*pcol = s - p->ptr;
				return (1);
			}
	}
	error ("String not found");
	return (0);
}

/*
 * text viewer.
 */
int VIEWFILE::textview (char *filename) {
	register baseline, basecol;
	int steplr = 8 + (COLS-1)/16*8;         /* step left-right */
	int stepud = LINES-3;                   /* step up-down */
	int ccol;                               /* cursor position */
	int sline, soff;                        /* search position */

	baseline = basecol = 0;
	while (viewrec->lindex[baseline+1].seek < viewseek)
		++baseline;
	while (viewrec->lindex[baseline].seek + offset (baseline, basecol) + 40 < viewseek)
		basecol += 40;
	ccol = 0;
	sline = soff = -1;
	VMPutString (LINES-1, 0, "\0011\16Help  \17 2\16      \17 3\16Raw   \17 4\16Hex   \17 5\16Top   \17 6\16Bottom\17 7\16Search\17 8\16      \17 9\16Spaces\01710\16Quit \17\2");
	for (;;) {
		viewpage (baseline, basecol);
		for (;;) {
			viewhead (filename, baseline, basecol);
			if (sline>=baseline && sline<baseline+LINES-2 &&
			    ccol>=basecol && ccol<basecol+80)
				VMove (sline-baseline+1, ccol-basecol);
			else {
				hidecursor ();
				sline = soff = -1;
			}
			VSync ();
			switch (KeyGet ()) {
			case meta ('A'):        /* f1 */
				runhelp ("view");
				continue;
			default:
				VBeep ();
				continue;
			case cntrl (']'):       /* redraw screen */
				VRedraw ();
				continue;
			case meta ('C'):        /* f3 - raw */
				viewraw ^= 1;
				break;
			case meta ('D'):        /* f4 - hex */
				viewseek = viewrec->lindex[baseline].seek + offset (baseline, basecol);
				return (1);
			case cntrl ('C'):
			case cntrl ('['):
			case meta ('J'):        /* f0 - quit */
				return (0);
			case meta ('I'):        /* f9 - spaces */
				viewtabs ^= 1;
				break;
			case meta ('G'): {      /* f7 - search */
				register char *p;

				if (! (p = getstring (SEARCHSZ-1, viewsrch, " Search ", "Search for the string")))
					continue;
				strcpy (viewsrch, p);
				if (sline < 0) {
					sline = baseline;
					soff = offset (baseline, basecol) - 1;
				}
				if (search (p, sline, soff+1, &sline, &soff)) {
					if (baseline > sline || baseline + LINES-2 <= sline)
						baseline = sline;
					if (baseline > viewrec->len - (LINES-2))
						baseline = viewrec->len - (LINES-2);
					if (baseline < 0)
						baseline = 0;
					ccol = column (sline, soff);
					if (ccol >= 40)
						basecol = (ccol + strlen (p) - 41) / 40 * 40;
					else
						basecol = 0;
					break;
				} else
					sline = soff = -1;
				continue;
			}
			case cntrl ('E'):
			case meta ('u'):        /* up */
				if (baseline <= 0)
					continue;
				--baseline;
				VDelLine (LINES-2);
				VInsLine (1);
				viewline (baseline, basecol, 1);
				continue;
			case cntrl ('M'):
			case cntrl ('J'):       /* down */
				if (basecol) {
					basecol = 0;
					break;
				}
				/* fall through */
			case cntrl ('X'):
			case meta ('d'):        /* down */
				if (baseline >= viewrec->len - (LINES-2))
					continue;
				VDelLine (1);
				VInsLine (LINES-2);
				viewline (++baseline+LINES-3, basecol, LINES-2);
				continue;
			case cntrl ('F'):
			case cntrl ('D'):
			case meta ('r'):        /* right */
				basecol += steplr;
				break;
			case cntrl ('S'):
			case cntrl ('A'):
			case cntrl ('W'):
			case meta ('l'):        /* left */
				if ((basecol -= steplr) < 0)
					basecol = 0;
				break;
			case cntrl ('B'):
			case meta ('n'):        /* next page */
				if (baseline >= viewrec->len - (LINES-2))
					continue;
				baseline += stepud;
				if (baseline > viewrec->len - (LINES-2))
					baseline = viewrec->len - (LINES-2);
				break;
			case cntrl ('R'):
			case meta ('p'):        /* prev page */
				if (baseline <= 0)
					continue;
				baseline -= stepud;
				if (baseline < 0)
					baseline = 0;
				break;
			case meta ('E'):        /* top */
			case meta ('h'):        /* home */
				if (baseline==0 && basecol==0)
					continue;
				baseline = basecol = 0;
				break;
			case meta ('F'):        /* bottom */
			case meta ('e'):        /* end */
				if (baseline >= viewrec->len - (LINES-2) && basecol==0)
					continue;
				baseline = viewrec->len - (LINES-2);
				basecol = 0;
				break;
			}
			break;
		}
	}
}

VIEWFILE::VIEWFILE(int d, char *filename) {
	viewfd = d;
	viewrec = rec.RecOpen(viewfd, 0);
	if (! viewrec)
		return;
	viewseek = 0;
	viewhlen = (viewrec->size + 15) / 16;
	for (;;) {
		VClearBox (1, 0, LINES-2, COLS);
		if (viewhex) {
			if (! hexview (filename))
				break;
			viewhex = 0;
		} else {
			if (! textview (filename))
				break;
			viewhex = 1;
		}
	}
	VClearBox (1, 0, LINES-2, COLS);
	rec.RecClose(viewrec);
}

# define GETHEX(x)      ((x)>='A' ? ((x)&15)+9 : (x)&15)

void VIEWFILE::viewhhead(char *filename, int line) {
	register i;

	VSetDim ();
	VStandOut ();
	VMove (0, 0);
	for (i=80; --i>=0; VPutChar (' '));
	VMPutString (0, 1, "File ");
	VPutString (filename);
	VMove (0, 22);
	printw ("Lines %-5d Bytes %ld", viewrec->len, viewrec->size);
	VMove (0, 68);
	printw ("Byte %ld", line*16L);
	VSetNormal ();
	VStandEnd ();
}

void VIEWFILE::prhline(register char *p, int len) {
	register i, c;

	if (len > 16)
		len = 16;
	for (i=0; i<16; ++i)
		if (i < len) {
			c = p [i];
			VPutChar ("0123456789abcdef" [c>>4&0xf]);
			VPutChar ("0123456789abcdef" [c&0xf]);
			VPutChar (' ');
		} else {
			VPutChar (' ');
			VPutChar (' ');
			VPutChar (' ');
		}
	for (i=0; i<len; ++i) {
		viewchar (*p++);
		if (i < len-1)
			VPutChar (' ');
	}
}

void VIEWFILE::viewhline(int line, int i) {
	char buf [16], *p;
	int n;

	lseek (viewfd, line*16L, 0);
	n = read (viewfd, buf, sizeof (buf));
	p = buf;
	VMove (i, 0);                           /* move to line */
	if (n > 0)
		prhline (p, n);
	VClearLine ();                          /* clear it */
}

void VIEWFILE::viewhpage(int line) {
	char buf [1024], *p;
	register i;
	int n;

	lseek (viewfd, line*16L, 0);
	n = read (viewfd, buf, (unsigned) LINES*16);
	p = buf;
	for (i=1; i<LINES-1; ++i, n-=16, p+=16) {
		VMove (i, 0);                   /* move to line */
		if (n > 0)
			prhline (p, n);
		VClearLine ();          /* clear rest of line */
	}
}

int VIEWFILE::hsearch(int l, int c, int *pline, int *pcol) {
	char buf [2*SEARCHSZ], *p;
	register char *s, *e;
	int n;

	for (;;) {
		lseek (viewfd, l*16L, 0);
		n = read (viewfd, buf, sizeof (buf));
		if (n <= 0)
			break;
		p = buf;
		e = buf + SEARCHSZ;
		for (s=p+c; s<e; ++s)
			if (! MemCompare (s, viewsbuf, n<viewsbsz ? n : viewsbsz)) {
				*pline = l + (s - p) / 16;
				*pcol = (s - p) % 16;
				return (1);
			}
		if (n < sizeof (buf))
			break;
		l += SEARCHSZ / 16;
		c = 0;
	}
	error ("String not found");
	return (0);
}

int VIEWFILE::cvtsrch(register char *from, register char *to) {
	register c, x, count;

	count = 0;
	for (count=0; c= *from++; *to++=c, ++count)
		if (c == '\\' && (x = *from)) {
			++from;
			if (x != '\\') {
				c = GETHEX (x);
				if ((x = *from) && x!='\\') {
					++from;
					c = c<<4 | GETHEX (x);
				}
			}
		}
	return (count);
}

/*
 * hexadecimal viewer
 */
int VIEWFILE::hexview(char *filename) {
	register baseline;
	register char *p;
	int stepud = LINES-3;                   /* step up-down */
	int sline, soff;                        /* search position */
	int hexcursor;                          /* hex cursor mode */

	baseline = (viewseek+15)/16;
	hexcursor = 0;
	sline = soff = -1;
	VMPutString (LINES-1, 0, "\0011\16Help  \17 2\16      \17 3\16Raw   \17 4\16Hex   \17 5\16Top   \17 6\16Bottom\17 7\16Search\17 8\16      \17 9\16      \01710\16Quit \17\2");
	for (;;) {
		viewhpage (baseline);
		for (;;) {
			viewhhead (filename, baseline);
			if (sline>=baseline && sline<baseline+LINES-2)
				VMove (sline-baseline+1,
					hexcursor ? soff*3 : soff*2+48);
			else {
				hidecursor ();
				sline = -1;
			}
			VSync ();
			switch (KeyGet ()) {
			case meta ('A'):        /* f1 */
				runhelp ("hexview");
				continue;
			default:
				VBeep ();
				continue;
			case cntrl ('I'):       /* hex/text cursor */
				hexcursor ^= 1;
				continue;
			case cntrl (']'):       /* redraw screen */
				VRedraw ();
				continue;
			case meta ('C'):        /* f3 - raw */
				viewraw ^= 1;
				break;
			case meta ('D'):        /* f4 - hex */
				viewseek = baseline*16L;
				return (1);
			case cntrl ('C'):
			case cntrl ('['):
			case meta ('J'):        /* f0 - quit */
				return (0);
			case meta ('u'):        /* up */
				if (baseline <= 0)
					continue;
				--baseline;
				VDelLine (LINES-2);
				VInsLine (1);
				viewhline (baseline, 1);
				continue;
			case cntrl ('M'):       /* down */
			case cntrl ('J'):       /* down */
			case meta ('d'):        /* down */
				if (baseline >= viewhlen - (LINES-2))
					continue;
				VDelLine (1);
				VInsLine (LINES-2);
				viewhline (++baseline+LINES-3, LINES-2);
				continue;
			case meta ('n'):        /* next page */
				if (baseline >= viewhlen - (LINES-2))
					continue;
				baseline += stepud;
				if (baseline > viewhlen - (LINES-2))
					baseline = viewhlen - (LINES-2);
				break;
			case meta ('p'):        /* prev page */
				if (baseline <= 0)
					continue;
				baseline -= stepud;
				if (baseline < 0)
					baseline = 0;
				break;
			case meta ('E'):        /* top */
			case meta ('h'):        /* home */
				if (baseline == 0)
					continue;
				baseline = 0;
				break;
			case meta ('F'):        /* bottom */
			case meta ('e'):        /* end */
				if (baseline >= viewhlen - (LINES-2))
					continue;
				baseline = viewhlen - (LINES-2);
				break;
			case meta ('G'):        /* f7 - search */
				if (! (p = getstring (SEARCHSZ-1, viewsrch, " Search ", "Search for the string")))
					continue;
				strcpy (viewsrch, p);
				viewsbsz = cvtsrch (viewsrch, viewsbuf);
				if (sline < 0) {
					sline = baseline;
					soff = -1;
				}
				if (hsearch (sline, soff+1, &sline, &soff)) {
					if (baseline>sline || baseline+LINES-2<=sline)
						baseline = sline;
					if (baseline > viewhlen - (LINES-2))
						baseline = viewhlen - (LINES-2);
					if (baseline < 0)
						baseline = 0;
					break;
				}
				sline = -1;
				continue;
			}
			break;
		}
	}
}
