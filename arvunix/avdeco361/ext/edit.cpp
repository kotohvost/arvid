/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include "scr.h"
# include "mem.h"
# include "choice.h"
# include "key.h"
# include "help.h"
# include "edit.h"
# include "main.h"

# ifndef NULL
#    define NULL        ((char *) 0)
# endif

extern viewtabs;			/* visible spaces mode */

int EDITFILE::charcode(int l, int n) {
	LINE *p;

	p = rec.RecGet(viewrec, l);
	if (! p)
		return (0);
	if (n >= p->len)
		return ('\n');
	return (p->ptr [n] & 0xff);
}

void EDITFILE::edithead(char *filename, int line, int col, int off) {
	register i;

	VSetDim ();
	VStandOut ();
	VMove (0, 0);
	for (i=80; --i>=0; VPutChar (' '));
	VMPutString (0, 1, "File ");
	VPutString (filename);
	printw ("  Lines %d", viewrec->len);
	VMove (0, 50);
	printw ("Line %-5d Col %d", line+1, col);
	i = charcode (line, off);
	VMove (0, 70);
	if (i)
		printw ("Char 0%-3o", i);
	else
		printw ("Char 0");
	VSetNormal ();
	VStandEnd ();
}

int EDITFILE::lastpos(int n) {
	register LINE *p;

	p = rec.RecGet(viewrec, n);
	if (! p)
		return (0);
	return (p->len);
}

int EDITFILE::ischar(register c) {
	c &= 0xff;
	if (c>='A' && c<='Z')
		return (1);
	if (c>='a' && c<='z')
		return (1);
	if (c>='0' && c<='9')
		return (1);
	if (c>=0200 && c<=0376)
		return (1);
	if (c=='_' || c=='$')
		return (1);
	return (0);
}

int EDITFILE::wordbeg(register LINE *p, register off) {
	register char *s;

	if (off <= 0)
		return (0);
	--off;
	s = p->ptr + off;
	while (off>0 && ! ischar (*s))
		--s, --off;
	if (off <= 0)
		return (0);
	while (off>=0 && ischar (*s))
		--s, --off;
	if (off < 0)
		return (0);
	return (off + 1);
}

void EDITFILE::clearline(register REC *r, int line) {
	register LINE *p;

	if (! (p = rec.RecGet(r, line)))
		return;
	if (p->len <= 0)
		return;
	MemFree ((mem *) p->ptr);
	p->ptr = "";
	rec.RecPut(p, 0);
}

void EDITFILE::joinlines(register REC *r, int line) {
	register LINE *p, *q;
	char *s;

	if (! (q = rec.RecGet(r, line+1)) || ! q->len)
		return;
	if (! (p = rec.RecGet(r, line)))
		return;
	if (! p->len) {
		p->ptr = q->ptr;
		q->ptr = "";
		rec.RecPut(p, q->len);
		rec.RecPut(q, 0);
		return;
	}
	s = (char *) MemAlloc (p->len + q->len);
	MemCopy (s, p->ptr, p->len);
	MemCopy (s+p->len, q->ptr, q->len);
	MemFree ((mem *) p->ptr);
	MemFree ((mem *) q->ptr);
	p->ptr = s;
	q->ptr = "";
	rec.RecPut(p, p->len + q->len);
	rec.RecPut(q, 0);
}

void EDITFILE::breakline(register REC *r, int line, int off) {
	register LINE *p, *q;
	char *s;

	if (off <= 0) {
		rec.RecInsLine(r, line);
		return;
	}
	if (! (p = rec.RecGet(r, line)))
		return;
	rec.RecInsLine(r, line+1);
	if (off >= p->len)
		return;
	if (! (q = rec.RecGet(r, line+1)))
		return;
	q->ptr = (char *) MemAlloc (p->len - off);
	MemCopy (q->ptr, p->ptr + off, p->len - off);
	rec.RecPut(q, p->len - off);
	s = (char *) MemAlloc (off);
	MemCopy (s, p->ptr, off);
	MemFree ((mem *) p->ptr);
	p->ptr = s;
	rec.RecPut(p, off);
}

void EDITFILE::cleartoeoln(register REC *r, int line, int off) {
	register LINE *p;
	char *s;

	if (! (p = rec.RecGet(r, line)))
		return;
	if (off >= p->len)
		return;
	if (off <= 0) {
		p->ptr = "";
		rec.RecPut(p, 0);
		return;
	}
	s = (char *) MemAlloc (off);
	MemCopy (s, p->ptr, off);
	MemFree ((mem *) p->ptr);
	p->ptr = s;
	rec.RecPut(p, off);
}

int EDITFILE::wordend(register LINE *p, register off) {
	register char *s;

	if (off >= p->len)
		return (p->len);
	s = p->ptr + off;
	while (off<p->len && ! ischar (*s))
		++s, ++off;
	if (off >= p->len)
		return (p->len);
	while (off<p->len && ischar (*s))
		++s, ++off;
	return (off);
};

EDITFILE::EDITFILE(int d, char *filename, char *basename) {
	register baseline, basecol;
	int stepud = LINES-3;                   /* step up-down */
	int sline, soff;                        /* search position */
	int ccol, lastccol, c;

	viewfd = d;
	viewrec = rec.RecOpen(d, 1);
	if (! viewrec)
		return;
	editmodflg = 0;
	if (viewrec->broken) {
		if (0 != getchoice (1, " Edit ",
		    "File contains too long lines.",
		    NULL, " Break lines ", " Cancel editing ", NULL))
			return;
		rec.RecBreak(viewrec);
		editmodflg = 1;
	}
	baseline = basecol = 0;
	sline = ccol = 0;
	VClearBox (1, 0, LINES-2, COLS);
	VMPutString (LINES-1, 0, "\0011\16Help  \17 2\16Save  \17 3\16Raw   \17 4\16      \17 5\16Top   \17 6\16Bottom\17 7\16Search\17 8\16Home  \17 9\16Spaces\01710\16Quit \17\2");
	for (;;) {
		viewpage (baseline, basecol);
		for (;;) {
			soff = offset (sline, ccol);
			if (soff > lastpos (sline))
				--soff;
			lastccol = column (sline, soff);
			if (lastccol < basecol || lastccol >= basecol+80) {
				basecol = lastccol / 40 * 40 - 40;
				if (basecol < 0)
					basecol = 0;
				break;
			}
			edithead (basename, sline, lastccol, soff);
			VMove (sline-baseline+1, lastccol-basecol);
			VSync ();
			switch (c = KeyGet ()) {
			case meta ('A'):        /* f1 */
				runhelp ("edit");
				continue;
			case cntrl (']'):       /* redraw screen */
				VRedraw ();
				continue;
			case cntrl ('Q'):
			case cntrl ('V'):       /* quote next char */
				VPutString ("\3\16#\2\17");
				ccol = column (sline, soff);
				VMove (sline-baseline+1, ccol-basecol);
				VSync ();
				c = KeyGetChar ();
				goto inschar;
			case meta ('b'):        /* back space */
				if (soff) {
					ccol = column (sline, --soff);
					editmodflg = 1;
					rec.RecDelChar(viewrec, sline, soff);
					viewline (sline, basecol, sline-baseline+1);
					continue;
				}
				if (! sline)
					continue;
				--sline;
				soff = lastpos (sline);
				ccol = column (sline, soff);
join:
				editmodflg = 1;
				joinlines (viewrec, sline);
				rec.RecDelLine(viewrec, sline+1);
				if (ccol < basecol || ccol >= basecol+80) {
					basecol = ccol<40 ? 0 : ((ccol - 40) / 40 * 40);
					break;
				}
				if (sline < baseline) {
					--baseline;
					viewline (baseline, basecol, 1);
					continue;
				}
				if (sline < baseline+LINES-4) {
					if (soff)
						VDelLine (2+sline-baseline);
					else
						VDelLine (1+sline-baseline);
					VInsLine (LINES-2);
					viewline (sline, basecol, sline-baseline+1);
					viewline (baseline+LINES-3, basecol, LINES-2);
					continue;
				}
				if (sline == baseline+LINES-4) {
					viewline (sline, basecol, LINES-3);
					viewline (sline+1, basecol, LINES-2);
					continue;
				}
				/* now sline equals to baseline+LINES-3 */
				viewline (sline, basecol, LINES-2);
				continue;
			case cntrl ('G'):       /* delete char */
				if (soff == lastpos (sline)) {
					if (sline == viewrec->len-1)
						continue;
					goto join;
				}
				editmodflg = 1;
				rec.RecDelChar(viewrec, sline, soff);
				viewline (sline, basecol, sline-baseline+1);
				continue;
			case cntrl ('K'):       /* clear to end of line */
				if (soff == lastpos (sline))
					continue;
				editmodflg = 1;
				cleartoeoln (viewrec, sline, soff);
				viewline (sline, basecol, sline-baseline+1);
				continue;
			case cntrl ('Y'):       /* delete line */
				editmodflg = 1;
				ccol = 0;
				if (sline == viewrec->len-1) {
					clearline (viewrec, sline);
					viewline (sline, basecol, sline-baseline+1);
					continue;
				}
				rec.RecDelLine(viewrec, sline);
				if (sline < baseline+LINES-3) {
					VDelLine (1+sline-baseline);
					VInsLine (LINES-2);
				}
				viewline (baseline+LINES-3, basecol, LINES-2);
				continue;
			case cntrl ('J'):
			case cntrl ('M'):       /* insert new line */
				editmodflg = 1;
				if (viewrec->len <= 0)
					rec.RecInsLine(viewrec, 0);
				breakline (viewrec, sline, soff);
				ccol = 0;
				++sline;
				if (sline < baseline+LINES-2) {
					VDelLine (LINES-2);
					if (soff)
						VInsLine (1+sline-baseline);
					else
						VInsLine (sline-baseline);
					viewline (sline-1, basecol, sline-baseline);
					viewline (sline, basecol, 1+sline-baseline);
					continue;
				}
				++baseline;
				VDelLine (1);
				VInsLine (LINES-2);
				viewline (sline-1, basecol, LINES-3);
				viewline (sline, basecol, LINES-2);
				continue;
			default:                /* insert char */
				if (c<' ' && c!='\t' || c>0376) {
					VBeep ();
					continue;
				}
inschar:
				if (viewrec->len <= 0)
					rec.RecInsLine(viewrec, 0);
				rec.RecInsChar(viewrec, sline, soff, c);
				editmodflg = 1;
				viewline (sline, basecol, sline-baseline+1);
				ccol = column (sline, soff+1);
				continue;
			case meta ('B'):        /* f2 - save */
				message (" Save ", "Saving file '%s'...", basename);
				rec.RecSave(viewrec, filename);
				editmodflg = 0;
				endmesg ();
				continue;
			case meta ('C'):        /* f3 - raw */
				viewraw ^= 1;
				break;
			case meta ('I'):        /* f9 - spaces */
				viewtabs ^= 1;
				break;
			case meta ('H'):        /* f8 - home */
				if (sline==baseline && ccol==0)
					continue;
				sline = baseline;
				ccol = 0;
				break;
			case meta ('E'):        /* f5 - top */
				if (sline==0 && baseline==0 && ccol==0)
					continue;
				sline = baseline = 0;
				ccol = 0;
				break;
			case meta ('F'):        /* f6 - bottom */
				if (soff == lastpos (sline) &&
				    sline >= viewrec->len - 1)
					continue;
				baseline = viewrec->len - (LINES-2);
				if (baseline < 0)
					baseline = 0;
				sline = viewrec->len - 1;
				if (sline < 0)
					sline = 0;
				soff = lastpos (sline);
				ccol = column (sline, soff);
				if (ccol < basecol || ccol >= basecol+80)
					basecol = ccol<40 ? 0 : ((ccol - 40) / 40 * 40);
				break;
			case cntrl ('C'):
			case cntrl ('['):
			case meta ('J'):        /* f0 - quit */
				if (editmodflg) switch (getchoice (1, " Edit ",
					"You've made changes since the last save.",
					NULL, " Save ", " Don't save ", " Continue editing ")) {
				case 0:
					if (rec.RecSave(viewrec, filename) < 0)
						continue;
				case 1:
					break;
				default:
					continue;
				}
				VClearBox (1, 0, LINES-2, COLS);
				rec.RecClose(viewrec);
				return;
			case meta ('G'): {      /* f7 - search */
				register char *p;

				if (! (p = getstring (SEARCHSZ-1, viewsrch, " Search ", "Search for the string")))
					continue;
				strcpy (viewsrch, p);
				if (! search (p, sline, soff+1, &sline, &soff))
					continue;
				if (baseline > sline || baseline + LINES-2 <= sline)
					baseline = sline;
				if (baseline > viewrec->len - (LINES-2))
					baseline = viewrec->len - (LINES-2);
				if (baseline < 0)
					baseline = 0;
				ccol = column (sline, soff);
				basecol = ccol<40 ? 0 : ((ccol + strlen (p) - 41) / 40 * 40);
				break;
			}
			case cntrl ('E'):       /* up */
			case meta ('u'):        /* up */
				if (sline <= 0)
					continue;
				--sline;
				if (baseline <= sline)
					continue;
				--baseline;
				VDelLine (LINES-2);
				VInsLine (1);
				viewline (baseline, basecol, 1);
				continue;
			case cntrl ('X'):       /* down */
			case meta ('d'):        /* down */
				if (sline >= viewrec->len-1)
					continue;
				++sline;
				if (baseline + (LINES-2) > sline)
					continue;
				++baseline;
				VDelLine (1);
				VInsLine (LINES-2);
				viewline (baseline+LINES-3, basecol, LINES-2);
				continue;
			case cntrl ('F'):       /* word right */
				soff = wordend (rec.RecGet(viewrec, sline), soff);
				ccol = column (sline, soff);
				continue;
			case cntrl ('W'):       /* word left */
				soff = wordbeg (rec.RecGet(viewrec, sline), soff);
				ccol = column (sline, soff);
				continue;
			case cntrl ('D'):
			case meta ('r'):        /* right */
				if (soff == lastpos (sline)) {
					if (sline == viewrec->len-1)
						continue;
					++sline;
					ccol = 0;
					if (baseline+LINES-2 <= sline) {
						++baseline;
						VDelLine (1);
						VInsLine (LINES-2);
						viewline (baseline+LINES-3, basecol, LINES-2);
					}
					continue;
				}
				ccol = column (sline, ++soff);
				continue;
			case cntrl ('A'):
			case cntrl ('S'):
			case meta ('l'):        /* left */
				if (soff <= 0) {
					if (sline <= 0)
						continue;
					--sline;
					soff = lastpos (sline);
					ccol = column (sline, soff);
					if (ccol < basecol || ccol >= basecol+80) {
						basecol = ccol<40 ? 0 : ((ccol - 40) / 40 * 40);
						if (baseline > sline)
							baseline = sline;
						break;
					}
					if (baseline > sline) {
						baseline = sline;
						VDelLine (LINES-2);
						VInsLine (1);
						viewline (baseline, basecol, 1);
					}
					continue;
				}
				ccol = column (sline, --soff);
				continue;
			case cntrl ('B'):
			case meta ('n'):        /* next page */
				if (baseline >= viewrec->len - (LINES-2))
					continue;
				baseline += stepud;
				sline += stepud;
				if (baseline > viewrec->len - (LINES-2)) {
					sline -= baseline - (viewrec->len - (LINES-2));
					baseline = viewrec->len - (LINES-2);
				}
				break;
			case cntrl ('R'):
			case meta ('p'):        /* prev page */
				if (baseline <= 0)
					continue;
				baseline -= stepud;
				sline -= stepud;
				if (baseline < 0) {
					sline -= baseline;
					baseline = 0;
				}
				break;
			case meta ('h'):        /* home */
				if (! ccol)
					continue;
				ccol = basecol = 0;
				break;
			case meta ('e'):        /* end */
				c = lastpos (sline);
				if (soff == c)
					continue;
				soff = c;
				ccol = column (sline, soff);
				if (ccol < basecol || ccol >= basecol+80)
					basecol = ccol<40 ? 0 : ((ccol - 40) / 40 * 40);
				break;
			}
			break;
		}
	}
}
