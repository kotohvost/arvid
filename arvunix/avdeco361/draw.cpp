/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include "string.h"
# include "stdio.h"
# include "time.h"
#include "unistd.h"

# include "dir.h"
# include "file.h"
# include "deco.h"
# include "scr.h"
# include "ut.h"
# include "pw.h"
# include "tim.h"
# include "draw.h"
# include "main.h"

# define DECOVERSION "Version 3.6.1"

extern char *timestr ();
extern visualwin;

# ifdef GROUPS
extern gidlist [];                      /* groups list */
extern gidnum;                          /* number of groups in list */
# endif

/* local */
void DRAW::drawframe(register c, int full, int stawin) {
	/* draw frame of (one) window */
	VSetDim ();
	if (stawin) {
		VHorLine (4, c+1, PAGEWID-2);
		VHorLine (11, c+1, PAGEWID-2);
		VVertLine (c, 2, H+2);
		VVertLine (c+PAGEWID-1, 2, H+2);
		VCorner (1, c, 1);
		VCorner (1, c+PAGEWID-1, 3);
		VCorner (4, c, 4);
		VCorner (4, c+PAGEWID-1, 6);
		VCorner (11, c, 4);
		VCorner (11, c+PAGEWID-1, 6);
		VCorner (H+4, c, 7);
		VCorner (H+4, c+PAGEWID-1, 9);
	} else if (fullwin) switch (full) {
	case 2:
		VHorLine (H+2, 1, PAGEWID-2);
		VVertLine (0, 2, H+2);
		VVertLine (PAGEWID-1, 2, H+2);
		VCorner (1, 0, 1);
		VCorner (1, PAGEWID-1, 3);
		VCorner (H+2, 0, 4);
		VCorner (H+2, PAGEWID-1, 6);
		VCorner (H+4, 0, 7);
		VCorner (H+4, PAGEWID-1, 9);
		break;
	case 1:
		VHorLine (H+2, 1, PAGEWID-2);
		VVertLine (0, 2, H+2);
		VVertLine (38, 2, H);
		VVertLine (PAGEWID-1, 2, H+2);
		VCorner (1, 0, 1);
		VCorner (1, PAGEWID-1, 3);
		VCorner (H+2, 0, 4);
		VCorner (H+2, 38, 8);
		VCorner (H+2, PAGEWID-1, 6);
		VCorner (H+4, 0, 7);
		VCorner (H+4, PAGEWID-1, 9);
		break;
	case 0:
		VHorLine (H+2, 1, PAGEWID-2);
		VVertLine (0, 2, H+2);
		VVertLine (19, 2, H);
		VVertLine (38, 2, H);
		VVertLine (57, 2, H);
		VVertLine (PAGEWID-1, 2, H+2);
		VCorner (1, 0, 1);
		VCorner (1, PAGEWID-1, 3);
		VCorner (H+2, 0, 4);
		VCorner (H+2, 19, 8);
		VCorner (H+2, 38, 8);
		VCorner (H+2, 57, 8);
		VCorner (H+2, PAGEWID-1, 6);
		VCorner (H+4, 0, 7);
		VCorner (H+4, PAGEWID-1, 9);
		break;
	} else {
		VHorLine (H+2, c+1, PAGEWID-2);
		VVertLine (c, 2, H+2);
		if (! full)
			VVertLine (c+19, 2, H);
		VVertLine (c+38, 2, H+2);
		VCorner (1, c, 1);
		VCorner (1, c+38, 3);
		VCorner (H+2, c, 4);
		VCorner (H+2, c+38, 6);
		VCorner (H+4, c, 7);
		if (! full)
			VCorner (H+2, c+19, 8);
		VCorner (H+4, c+38, 9);
	}
	VSetNormal ();
}

/* local */
void DRAW::drawcwd(int col, register char *wd) {
	register len;

	if (! visualwin)
		return;
	len = strlen (wd);
	if (fullwin && col)
		col = 38;
	VMPutString (1, col + (39-len) / 2, wd);
}

/* local */
void DRAW::drawpat(int col, register char *pat, int width) {
	col += width - strlen (pat) - 1;
	VMove (H+4, col);
	VPutString (pat);
}

# define DELIM ','
/* local */
char *DRAW::ltoac (long l) {
	char num [15];
	static char buf [15];
	register char *p, *q;
	register n;

	sprintf (num, "%ld", l);
	for (p=num; *p; ++p);           /* find end of num */
	q = buf + sizeof (buf) - 1;     /* set q to the end of buf */
	*q-- = 0;
	for (n=0; p>num; ++n) {
		if (n && n%3 == 0)
			*q-- = DELIM;
		*q-- = *--p;
	}
	return (q+1);
}

/* local */
void DRAW::drawsdir(register dir *curdir, dir *dleft, dir *dright) {
	/* draw status window */
	struct dir *d = curdir == dleft ? dright : dleft;
	char buf [80];

	/* print window name " Status " */
	VSetDim ();
	VHorLine (1, BASECOL (curdir) + 1, PAGEWID-2);
	drawcwd (curdir->d.basecol, " Status ");
	VHorLine (H+4, BASECOL (curdir) + 1, PAGEWID-2);
	VSetNormal ();
	VClearBox (2, BASECOL (curdir) + 1, 2, PAGEWID-2);
	VClearBox (5, BASECOL (curdir) + 1, 6, PAGEWID-2);
	VClearBox (12, BASECOL (curdir) + 1, H-8, PAGEWID-2);
	sprintf (buf, "The Demos Commander, %s", DECOVERSION);
	mvcaddstr (2, BASECOL (curdir) + PAGEWID/2 + 1, buf);
	mvcaddstr (3, BASECOL (curdir) + PAGEWID/2 + 1, "Copyright (C) 1989-94 by S.Vakulenko");
	mvcaddstr (5, BASECOL (curdir) + PAGEWID/2 + 1, "Directory");
	mvcaddstr (6, BASECOL (curdir) + PAGEWID/2 + 1, d->d.shortcwd);
	sprintf (buf, "\3%d\2 file%s", d->d.nfiles, d->d.nfiles==1 ? "" : "s");
	mvcaddstr (7, BASECOL (curdir) + PAGEWID/2 + 2, buf);
	sprintf (buf, "\3%s\2 byte%s", ltoac (d->d.nbytes), d->d.nbytes==1 ? "" : "s");
	mvcaddstr (8, BASECOL (curdir) + PAGEWID/2 + 2, buf);
	if (d->d.pattern [0]) {
		sprintf (buf, "\3%s\2 byte%s in \3%d\2 matched file%s",
			ltoac (d->d.mbytes), d->d.mbytes==1 ? "" : "s",
			d->d.mfiles, d->d.mfiles==1 ? "" : "s");
		mvcaddstr (9, BASECOL (curdir) + PAGEWID/2 + 3, buf);
	}
	if (d->d.tfiles) {
		sprintf (buf, "\3%s\2 byte%s in \3%d\2 selected file%s",
			ltoac (d->d.tbytes), d->d.tbytes==1 ? "" : "s",
			d->d.tfiles, d->d.tfiles==1 ? "" : "s");
		mvcaddstr (10, BASECOL (curdir) + PAGEWID/2 + 3, buf);
	}
# ifdef notyet
	mvcaddstr (13, BASECOL (curdir) + PAGEWID/2 + 1, "No .dirinfo file in this directory");
# endif
}

/* local */
void DRAW::drawlong(register struct file *f) {
	printw (" %8.8s.%-8.8s", username ((int) f->uid), groupname ((int) f->gid));
	VPutString (timestr (f->mtime));
}

/* local */
void DRAW::drawtime(register struct file *f) {
	printw ("%4d/%-5d", f->dev, f->ino);
	if (f->nlink > 1)
		printw (" %-3d ", f->nlink);
	else
		VPutString ("     ");
	VPutString (timestr (f->atime));
	VPutChar (' ');
	VPutChar (' ');
	VPutString (timestr (f->ctime));
}

/* local */
void DRAW::drawmode(register char *m, register u, register g) {
	u = (u == uid);
# ifdef GROUPS
	if (g == gid)
		g = 1;
	else {
		int i;

		for (i=0; i<gidnum; ++i)
			if (g == gidlist[i]) {
				g = 1;
				break;
			}
		if (i >= gidnum)
			g = 0;
	}
# else
	g = (g == gid);
# endif
	if (! u)
		VSetDim ();
	VPutChar (*m++);
	VPutChar (*m++);
	VPutChar (*m++);
	if (u)
		VSetDim ();
	else if (g)
		VSetPrev ();
	VPutChar (*m++);
	VPutChar (*m++);
	VPutChar (*m++);
	if (! u)
		if (g)
			VSetDim ();
		else
			VSetPrev ();
	VPutChar (*m++);
	VPutChar (*m++);
	VPutChar (*m++);
	if (u || g)
		VSetPrev ();
}

/* local */
char *DRAW::filnam(register char *p) {
	static char buf [20];
	register char *s, *e;

	if (strlen (p) > 14) {
		strncpy (buf, p, 13);
		buf [13] = '~';
		return (buf);
	}
	for (s=buf; s<buf+14; *s++=' ');        /* clear buf */
	if (cdir->d.alignext) {
		e = extension (p);                      /* make ptr to extension */
		for (s=buf; p<e; *s++= *p++);           /* copy file name */
		for (s=e; *s; ++s);                     /* search extension end */
		for (p=buf+13, --s; s>=e; *p--= *s--);  /* copy extension */
	} else {
		e = p + 14;                             /* make ptr to end */
		for (s=buf; *p && p<e; *s++= *p++);     /* copy file name */
	}
	buf [14] = 0;
	return (buf);
}
  
/* local */
void DRAW::drawfull(register struct file *f) {
	VPutString (filnam (f->name));                          /* 14 */
	VPutString (filesize (f));                              /* 8 */
	VPutChar (' ');                                         /* 1 */
	VPutChar (' ');                                         /* 1 */
	drawmode (filemode ((int) f->mode), (int) f->uid, (int) f->gid);
}

/* local */
void DRAW::drawstatus(int col, struct file *f, int full) {
	/* draw current file status box */
	VMove (H+3, col+2);
	VSetDim ();
	VPutChar (filetype (f));
	VSetNormal ();
	VPutChar (' ');
	if (fullwin) switch (full) {
	case 2:
		VPutString (filnam (f->name));
		drawtime(f);
		break;
	case 1:
	case 0:
		drawfull(f);
		drawlong(f);
		break;
	} else
		drawfull(f);
}

/* local */
void DRAW::drawpage(int col, register struct file *c, register n, int full) {
	/* redraw file window */
	/* c - catalog */
	/* n - number of names to print */
	register i;

	if (n > H)
		n = H;
	++col;
	for (i=0; i<n; ++i, ++c) {
		VSetDim ();
		VMPutChar (i+2, col, filetype (c));
		if (c->tag) {
			VPutChar ('>');
			VSetBold ();
		} else if (c->link) {
			VPutChar ('@');
			VSetNormal ();
		} else {
			VPutChar (' ');
			VSetNormal ();
		}
		switch (full) {
		case 2:
			if (fullwin) {
				drawfull (c);
				drawlong (c);
				break;
			}
			/* fall through */
		case 1:
			drawfull(c);
			break;
		case 0:
			VPutString (filnam (c->name));
			break;
		}
		if (c->tag)
			VSetNormal ();
	}
}

/* main */
void DRAW::drawbanners() {
	register i;
	register char *curtime;

	VSetDim ();
	VStandOut ();
	VMove (0, 0);
	for (i=80; --i>=0; VPutChar (' '));
	VMove (0, 1);
	if (machine) {
		VPutString (machine);
		VPutChar (':');
	}
	if (tty)
		VPutString (tty);
	if (user)
		VMPutString (0, 39 - strlen (user), user);
	VPutChar ('.');
	if (group)
		VPutString (group);
	if (curtime = timestr (time ((long *) 0)))
		VMPutString (0, 79 - strlen (curtime), curtime);
	VStandEnd ();
	VMPutString (LINES-1, 0, cmdreg ?
"1\16Help  \17 2\16User  \17 3\16      \17 4\16      \17 5\16    \
  \17 6\16      \17 7\16      \17 8\16      \17 9\16Menu  \017 10\16Quit \17" :
"1\16Help  \17 2\16User  \17 3\16View  \17 4\16Edit  \17 5\16Copy\
  \17 6\16RenMov\17 7\16Mkdir \17 8\16Delete\17 9\16Menu  \017 10\16Quit \17");
	VSetNormal ();
}

/* local */
void DRAW::draw1dir(register dir *curdir, dir *dleft, dir *dright) {
	/* redraw window */
	register len = curdir->d.num - curdir->d.topfile;

	/* check visibility in full window mode */
	if (! visualwin || fullwin && curdir != cur)
			return;
	if (visualwin)
		if (fullwin)
			drawframe(BASECOL (curdir), curdir->d.full, curdir->d.status);
		else
			drawframe(curdir->d.basecol, curdir->d.full, curdir->d.status);
	cdir = curdir;

	/* if it is status window - draw status of another one */
	if (curdir->d.status) {
		drawsdir(curdir, dleft, dright);
		return;
	}

	/* reprint dir name */
	VSetDim ();
	VHorLine (1, BASECOL (curdir) + 1, PAGEWID-2);
	VHorLine (H+4, BASECOL (curdir) + 1, PAGEWID-2);
	if (fullwin)
		if (curdir->d.full == 0) {
			VCorner (1, 19, 2);
			VCorner (1, 38, 2);
			VCorner (1, 57, 2);
		} else if (curdir->d.full == 1)
			VCorner (1, 38, 2);
	drawcwd (curdir->d.basecol, curdir->d.shortcwd);
	if (curdir->d.pattern [0])
		drawpat(BASECOL (curdir), curdir->d.pattern, PAGEWID);
	VSetNormal ();

	switch (curdir->d.full) {
	case 2:
		if (fullwin) {
			VClearBox (2, 1, H, 75);
			drawpage(1, &curdir->d.cat[curdir->d.topfile], len, 2);
			break;
		}
		/* fall through */
	case 1:
		VClearBox (2, BASECOL (curdir) + 1, H, 37);
		drawpage(BASECOL (curdir) + 1, &curdir->d.cat[curdir->d.topfile], len, 1);
		if (fullwin) {
			VClearBox (2, BASECOL (curdir) + 39, H, 37);
			if (len > H)
				drawpage(BASECOL (curdir) + 39, &curdir->d.cat[curdir->d.topfile+H], len-H, 1);
		}
		break;
	default:
		VClearBox (2, BASECOL (curdir) + 1, H, 18);
		drawpage(BASECOL (curdir) + 1, &curdir->d.cat[curdir->d.topfile], len, 0);

		VClearBox (2, BASECOL (curdir) + 20, H, 18);
		if (len > H)
			drawpage(BASECOL (curdir) + 20, &curdir->d.cat[curdir->d.topfile+H], len-=H, 0);
		if (fullwin) {
			VClearBox (2, BASECOL (curdir) + 39, H, 18);
			if (len > H)
				drawpage(BASECOL (curdir) + 39, &curdir->d.cat[curdir->d.topfile+H+H], len-=H, 0);

			VClearBox (2, BASECOL (curdir) + 58, H, 18);
			if (len > H)
				drawpage(BASECOL (curdir) + 58, &curdir->d.cat[curdir->d.topfile+H+H+H], len-H, 0);
		}
		break;
	}
	drawstatus(BASECOL (curdir), &curdir->d.cat[curdir->d.curfile], curdir->d.full);
}

/* com, main */
void DRAW::draw(struct dir *curdir, struct dir *dleft, struct dir *dright) {
	if (visualwin) {
		drawbanners();
		draw1dir(dleft, dleft, dright);
		draw1dir(dright, dleft, dright);
	} else {
		VClearBox (1, 0, LINES-2, 80);
		drawbanners();
	}
}

/* main */
void DRAW::drawdir(register struct dir *curdir, int statusflag, struct dir *dleft, struct dir *dright) {
	/* redraw window */
	draw1dir(curdir, dleft, dright);
	if (statusflag) {
		curdir = (curdir == dleft ? dright : dleft);
		if (curdir->d.status)
			draw1dir(curdir, dleft, dright);
	}
}

/* main */
void DRAW::drawcursor(struct dir *curdir) {
	register col, line, w;

	cdir = curdir;
	if (curdir->d.status) {
		VStandOut ();
		drawcwd(curdir->d.basecol, " Status ");
		VStandEnd ();
		return;
	}
	w = curdir->d.full==2 ? PAGEWID-2 : curdir->d.full ? 37 : 18;
	line = curdir->d.curfile - curdir->d.topfile;
	if (line < H)
		col = BASECOL (curdir) + 1;
	else {
		col = BASECOL (curdir) + line/H * (w+1) + 1;
		line %= H;
	}
	line += 2;
	if (cbox)
		VFreeBox (cbox);
	cbox = VGetBox (line, col, 1, w);
	if (VStandOut ())
		if (curdir->d.cat[curdir->d.curfile].tag) {
			VSetBold ();
			VPrintBox (cbox);
			VSetNormal ();
		} else
			VPrintBox (cbox);
	else {
		VMPutChar (line, col, '[');
		VMPutChar (line, col+w-1, ']');
	}
	drawcwd(curdir->d.basecol, curdir->d.shortcwd);
	VStandEnd ();
	drawstatus(BASECOL (curdir), &curdir->d.cat[curdir->d.curfile], curdir->d.full);
}

/* main */
void DRAW::undrawcursor(struct dir *curdir) {
	if (cbox) {
		VUngetBox (cbox);
		cbox = 0;
	}
	VSetDim();
	drawcwd(curdir->d.basecol, curdir->d.status ? " Status " : curdir->d.shortcwd);
	VSetNormal();
}
