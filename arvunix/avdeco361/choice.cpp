/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */
#include "stdio.h"
#include "string.h"
#include "unistd.h"

# include "scr.h"
# include "dir.h"
# include "deco.h"
# include "ut.h"
#include "key.h"

# ifndef NULL
#    define NULL        ((char *) 0)
# endif

# define SWAP(a,b)      { register t; t = a; a = b; b = t; }

struct choice {
	char *name;
	int namlen;
	int r, c;
};

static struct choice tab [3];
static cnum;
static void (*attron) (void);
static int (*attroff) (void);
static BOX *mbox;

static menuchoice ();

/* local */
static initchoice (int row, int col, char *c1, char *c2, char *c3) {
	register i, w;

	cnum = c2 ? (c3 ? 3 : 2) : 1;
	tab[0].name = c1;
	tab[1].name = c2;
	tab[2].name = c3;
	w = cnum-1;
	for (i=0; i<cnum; ++i) {
		w += tab[i].namlen = strlen (tab[i].name);
		tab[i].r = row;
	}
	tab[0].c = col-w/2;
	tab[1].c = tab[0].c + tab[0].namlen + 1;
	tab[2].c = tab[1].c + tab[1].namlen + 1;
}

/* cmd, edit */
int getchoice (int bold, char *head, char *msg, char *mesg2, char *c1, char *c2, char *c3) {
	int len, ch;
	int isattr;
	register r, c, w, h;
	BOX *box;
	char mesg[67];

	strncpy(mesg, msg, sizeof (mesg));
	mesg[sizeof(mesg)-1] = 0;
	w = strlen (mesg);
	if (mesg2) {
		len = strlen (mesg2);
		if (len > w)
			w = len;
	}
	len = strlen (head);
	if (len > w)
		w = len;
	len = 0;
	if (c1)
		len += strlen (c1);
	if (c2)
		len += strlen (c2);
	if (c3)
		len += strlen (c3);
	if (len > w)
		w = len;
	h = 6;
	w += 10;
	if (bold)
		r = LINES/2;
	else
		r = LINES/4;
	c = (80 - w) / 2;

	box = VGetBox (r-1, c-2, h+2, w+4);             /* save box */
	isattr = VStandOut ();
	if (bold)
		VSetBold ();
	else
		VSetDim ();
	VFillBox (r-1, c-2, h+2, w+4, ' ');             /* clear */
	VDrawBox (r, c, h, w);                          /* draw margins */
	VMPutString (r, c + (w-strlen(head)) / 2, head); /* head */
	if (mesg2) {
		VMPutString (r+1, c + (w-strlen(mesg)) / 2, mesg);
		VMPutString (r+2, c + (w-strlen(mesg2)) / 2, mesg2);
	} else
		VMPutString (r+2, c + (w-strlen(mesg)) / 2, mesg);

	if (c1) {
		if (isattr) {
			attron = VStandEnd;
			attroff = VStandOut;
		} else {
			attron = NULL;
			attroff = NULL;
		}

		initchoice (r+4, c+w/2, c1, c2, c3);

		ch = menuchoice ();
		VStandEnd ();
		VSetNormal ();
		VUngetBox (box);
		VFreeBox (box);
		return (ch);
	} else {
		/* message */
		VStandEnd ();
		VSetNormal ();
		mbox = box;
		hidecursor ();
		VSync ();
# ifdef DOC
		KeyGet ();
# endif
		return (0);
	}
}

/* VARARGS 1 */
/* cmd, com, dir, ex, rec.c, view */
void error (char *s, long aa, long bb, long cc, long dd) {
	char buf [100];

	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (1, " Error ", buf, NULL, " Ok ", NULL, NULL);
}

/* VARARGS 1 */
/* cmd, dir, edit, ex */
void message(char *name, char *s, long aa, long bb, long cc, long dd) {
	char buf [100];

	sprintf (buf, s, aa, bb, cc, dd);
	getchoice (0, name, buf, NULL, NULL, NULL, NULL);
}

/* cmd, edit, ex */
void endmesg(void) {
	if (mbox)
		VUngetBox (mbox);
	mbox = 0;
}

/* local */
static drawchoice (int ch) {
	register i;

	for (i=0; i<cnum; ++i) {
		if (i == ch) {
			if (attron)
				(*attron) ();
			VMPutString (tab[i].r, tab[i].c, tab[i].name);
			if (! attron) {
				VMPutChar (tab[i].r, tab[i].c, '[');
				VMPutChar (tab[i].r, tab[i].c + strlen (tab[i].name) - 1, ']');
			}
			if (attroff)
				(*attroff) ();
		} else
			VMPutString (tab[i].r, tab[i].c, tab[i].name);
	}
}

/* local */
static menuchoice ()
{
	int ch = 0;

	for (;;) {
		drawchoice (ch);
		hidecursor ();
		VSync ();
		switch (KeyGet ()) {
		default:
			VBeep ();
			continue;
		case cntrl (']'):       /* redraw screen */
			VRedraw ();
			continue;
		case cntrl ('['):
		case cntrl ('C'):
		case meta ('J'):        /* f0 */
			return (-1);
		case cntrl ('M'):
		case cntrl ('J'):
			return (ch);
		case ' ':
		case cntrl ('I'):
		case meta ('r'):        /* right */
			if (++ch >= cnum)
				ch = 0;
			continue;
		case meta ('b'):        /* backspace */
		case meta ('l'):        /* left */
			if (--ch < 0)
				ch = cnum-1;
			continue;
		}
	}
}

/* local */
static char *editstring (int r, int c, int w, char *str, register cp) {
	/* cursor position */
	register key, k;
	int firstkey = 1;

	if (cp) {
		for (cp=0; str[cp]; ++cp);
		firstkey = 0;
	}
	for (; ; firstkey=0) {
		VClearBox (r, c, 1, w);
		VMPutString (r, c, str);
		VMove (r, c+cp);
		VSync ();
		switch (key = KeyGet ()) {
		default:
			if (key<' ' || key>'~' && key<0300 || key>0376) {
				VBeep ();
				continue;
			}
			if (firstkey) {
				str[0] = key;
				str[1] = 0;
				cp = 1;
				continue;
			}
			for (k=cp; str[k]; ++k)
				SWAP (key, str[k]);
			str [k] = key;
			str [w] = str [k+1] = 0;
			/* fall through */
		case meta ('r'):        /* right */
			if (str [cp]) {
				++cp;
				if (cp >= w)
					cp = w-1;
			}
			continue;
		case meta ('l'):        /* left */
			if (--cp < 0)
				cp = 0;
			continue;
		case cntrl (']'):       /* redraw screen */
			VRedraw ();
			continue;
		case cntrl ('C'):
		case cntrl ('['):
		case meta ('J'):        /* f0 */
			return (0);
		case cntrl ('M'):
		case cntrl ('J'):
			return (str);
		case cntrl ('I'):
			if (str [cp])
				while (str [++cp]);
			else
				cp = 0;
			continue;
		case meta ('h'):        /* home */
			cp = 0;
			continue;
		case meta ('e'):        /* end */
			while (str [cp])
				++cp;
			continue;
		case meta ('b'):        /* back space */
			if (cp) {
				for (k=cp--; str[k]; ++k)
					str[k-1] = str[k];
				str [k-1] = 0;
			}
			continue;
		case cntrl ('G'):       /* delete */
			if (! str [cp])
				continue;
			for (k=cp+1; str[k]; ++k)
				str[k-1] = str[k];
			str [k-1] = 0;
			continue;
		case cntrl ('Y'):       /* clear line */
			str [cp = 0] = 0;
			continue;
		}
	}
}

/* cmd, edit, view */
char *getstring (register sizew, char *str, char *head, char *mesg) {
	register r, c, h;
	int len;
	BOX *box;
	static char buf [81];

	len = strlen (mesg);
	if (len > sizew)
		sizew = len;
	len = strlen (head);
	if (len > sizew)
		sizew = len;
	h = 4;
	sizew += 4;
	r = LINES/4;
	c = (78 - sizew) / 2;

	box = VGetBox (r-1, c-2, h+2, sizew+4);             /* save box */
	VStandOut ();
	VSetDim ();
	VFillBox (r-1, c-2, h+2, sizew+4, ' ');             /* clear */
	VDrawBox (r, c, h, sizew);                          /* draw margins */
	VMPutString (r, c + (sizew-strlen(head)) / 2, head); /* head */
	VMPutString (r+1, c+2, mesg);                   /* message */
	VStandEnd ();
	VSetNormal ();

	strncpy (buf, str ? str : "", 80);

	str = editstring (r+2, c+2, sizew-4, buf, 0);
	VUngetBox (box);
	VFreeBox (box);
	return (str);
}

/* cmd */
char *getwstring (register sizew, char *str, char *head, char *mesg) {
	register r, c, h;
	int len;
	BOX *box;
	static char buf [81];

	len = strlen (mesg);
	if (len > sizew)
		sizew = len;
	len = strlen (head);
	if (len > sizew)
		sizew = len;
	h = 4;
	sizew += 4;
	r = LINES/4;
	c = BASECOL (cur) + (PAGEWID - sizew) / 2;
	if (c < 3)
		c = 3;
	else if (c > 76-sizew)
		c = 76-sizew;

	box = VGetBox (r-1, c-2, h+2, sizew+4);             /* save box */
	VStandOut ();
	VSetDim ();
	VFillBox (r-1, c-2, h+2, sizew+4, ' ');             /* clear */
	VDrawBox (r, c, h, sizew);                          /* draw margins */
	VMPutString (r, c + (sizew-strlen(head)) / 2, head); /* head */
	VMPutString (r+1, c+2, mesg);                   /* message */
	VStandEnd ();
	VSetNormal ();

	strncpy (buf, str ? str : "", 80);

	str = editstring (r+2, c+2, sizew-4, buf, 1);
	VUngetBox (box);
	VFreeBox (box);
	return (str);
}
