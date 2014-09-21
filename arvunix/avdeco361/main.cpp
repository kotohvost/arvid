/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include "stdio.h"
# include "unistd.h"
#include "signal.h"
#include "fcntl.h"
# include "sys/types.h"
# include "sys/stat.h"

# include "key.h"
# include "scr.h"
# include "dir.h"
# include "deco.h"
# include "mem.h"

# ifndef NULL
#    define NULL ((char *) 0)
# endif

/*----------------------------*/
# include "menu.h"	/* class MENU */
# include "ex.h"	/* class InitFile, UserDefMenu */
//void excommand (char *cp, char *fname);
//# include "com.h"	/* class CMD */
# include "cmd.h"
//# include "run.h"
# include "draw.h"	/* class DRAW */
# include "help.h"
#include "pw.h"
#include "ut.h"
#include "cap.h"
#include "env.h"
#include "ReadWrite.h"	/* class ReadFiles */
/*----------------------------*/

int	H;				/* file window height */
int	gid;				/* real group, user and parent ids */
int	uid;				/* real group, user and parent ids */
int	ppid;				/* real group, user and parent ids */
int	fullwin;			/* full window mode */
char	*user;
char	*group;
char	*tty = 0;
char	*machine;
int	cpos;				/* position of command line cursor */
int	cmdreg;				/* cursor in command line */
int	visualwin = 1;			/* cursor in command line */

# ifdef GROUPS
/* unsigned */
int	gidlist [128];			/* groups list */
int	gidnum;				/* number of groups in list */
# endif

struct palette dflt_palette = { 7, 0, 7, 6, 15, 0, 15, 6, 3, 0, 0, 7, };
struct palette palette;

//extern char *home;

extern char *username (), *groupname ();

static struct KeyMap keymap [] = {
	"kl",           0,              meta ('l'),
	"kr",           0,              meta ('r'),
	"ku",           0,              meta ('u'),
	"kd",           0,              meta ('d'),
	"kN",           0,              meta ('n'),
	"kP",           0,              meta ('p'),
	"kh",           0,              meta ('h'),
	"kH",           0,              meta ('e'),
	"@7",           0,              meta ('e'),
	"kI",           0,              cntrl ('T'),
	"k.",           0,              cntrl ('G'),
	"kD",           0,              cntrl ('G'),
	"kE",           0,              meta ('h'),
	"kS",           0,              meta ('e'),
	"k1",           0,              meta ('A'),
	"k2",           0,              meta ('B'),
	"k3",           0,              meta ('C'),
	"k4",           0,              meta ('D'),
	"k5",           0,              meta ('E'),
	"k6",           0,              meta ('F'),
	"k7",           0,              meta ('G'),
	"k8",           0,              meta ('H'),
	"k9",           0,              meta ('I'),
	"k0",           0,              meta ('J'),
	"k;",           0,              meta ('J'),
	"f1",           0,              meta ('A'),
	"f2",           0,              meta ('B'),
	"f3",           0,              meta ('C'),
	"f4",           0,              meta ('D'),
	"f5",           0,              meta ('E'),
	"f6",           0,              meta ('F'),
	"f7",           0,              meta ('G'),
	"f8",           0,              meta ('H'),
	"f9",           0,              meta ('I'),
	"f0",           0,              meta ('J'),
	"kb",           "\b",           meta ('b'),
	0,              "\0331",        meta ('A'),
	0,              "\0332",        meta ('B'),
	0,              "\0333",        meta ('C'),
	0,              "\0334",        meta ('D'),
	0,              "\0335",        meta ('E'),
	0,              "\0336",        meta ('F'),
	0,              "\0337",        meta ('G'),
	0,              "\0338",        meta ('H'),
	0,              "\0339",        meta ('I'),
	0,              "\0330",        meta ('J'),

	0,              "\033l",        meta ('l'),
	0,              "\033r",        meta ('r'),
	0,              "\033u",        meta ('u'),
	0,              "\033d",        meta ('d'),
	0,              "\033n",        meta ('n'),
	0,              "\033p",        meta ('p'),
	0,              "\033h",        meta ('h'),
	0,              "\033e",        meta ('e'),

	0,              "\033\033",     cntrl ('C'),
	0,              0,              0,
};

/* Global class */
MENU		mymenu;
InitFile	initfile;
UserDefMenu	udm;
//CMD		cmd;
DRAW		draw;
V		v;
dir		*left, *right;		/* left and right directories */
dir		*cur;			/* current directory */
ReadFiles	rfiles;		// array of read files

#if 0
void inithome () {
	register char *s;

	if (s = EnvGet ("HOME"))
		home = s;
}
#endif

#if 0
void docmdreg (int c) {
	switch (c) {
	case meta ('h'):          /* home */
		cmd.homecmd();
		return;
	case meta ('e'):          /* end */
		cmd.endcmd();
		return;
	case meta ('l'):          /* left */
		cmd.leftcmd();
		return;
	case meta ('r'):          /* right */
		cmd.rightcmd();
		return;
	case meta ('u'):          /* up */
		cmd.upcmd();
		return;
	case meta ('d'):          /* down */
		cmd.downcmd();
		return;
	case meta ('n'):          /* next page */
		cmd.nextcmd();
		return;
	case meta ('p'):          /* prev page */
		cmd.prevcmd();
		return;
	}
}
#endif

int pagelen (int f) {
	register h = H;

	if (! fullwin)
		return (f ? h : h+h);
	switch (f) {
	case 0:
		h += h;
	case 1:
		return (h+h);
	default:
	case 2:
		return (h);
	}
}

void doscrreg (int c) {
	switch (c) {
	case meta ('h'):          /* home */
		cur->d.curfile = cur->d.topfile = 0;
		break;
	case meta ('e'):          /* end */
		cur->d.curfile = cur->d.num - 1;
		cur->d.topfile = cur->d.num - PAGELEN (cur);
		if (cur->d.topfile < 0)
			cur->d.topfile = 0;
		break;
	case meta ('u'):          /* up */
		if (cur->d.curfile <= 0)
			return;
		if (cur->d.curfile > cur->d.topfile) {
			--cur->d.curfile;
			return;
		}
		cur->d.topfile = --cur->d.curfile;
		break;
	case meta ('d'):          /* down */
		if (cur->d.curfile >= cur->d.num-1)
			return;
		if (cur->d.topfile + PAGELEN (cur) - 1 > cur->d.curfile) {
			++cur->d.curfile;
			return;
		}
		cur->d.topfile = ++cur->d.curfile - PAGELEN (cur) + 1;
		break;
	case meta ('l'):          /* left */
		if (cur->d.curfile < H) {
			if (cur->d.topfile <= 0)
				return;
			cur->d.curfile -= cur->d.topfile;
			cur->d.topfile = 0;
			break;
		}
		cur->d.curfile -= H;
		if (cur->d.topfile <= cur->d.curfile)
			return;
		cur->d.topfile -= H;
		if (cur->d.topfile <= 0) {
			cur->d.curfile -= cur->d.topfile;
			cur->d.topfile = 0;
		}
		break;
	case meta ('r'):          /* right */
		if (cur->d.curfile + H < cur->d.num) {
			cur->d.curfile += H;
			if (cur->d.topfile + PAGELEN (cur) > cur->d.curfile)
				return;
			cur->d.topfile += H;
			break;
		}
		if (cur->d.topfile + PAGELEN (cur) < cur->d.num) {
			cur->d.curfile = cur->d.num-1;
			cur->d.topfile += H;
			break;
		}
		if ((cur->d.curfile - cur->d.topfile) / H <
		    (cur->d.num - cur->d.topfile - 1) / H)
			cur->d.curfile = cur->d.num-1;
		return;
	case meta ('n'):          /* next page */
		if (cur->d.topfile + PAGELEN (cur) >= cur->d.num) {
			cur->d.curfile = cur->d.num-1;
		} else if (cur->d.topfile + 2 * PAGELEN (cur) >= cur->d.num) {
			cur->d.curfile = cur->d.num-1;
			cur->d.topfile = cur->d.num - PAGELEN (cur);
		} else {
			cur->d.curfile += PAGELEN (cur);
			cur->d.topfile += PAGELEN (cur);
		}
		break;
	case meta ('p'):          /* prev page */
		if (cur->d.topfile == 0) {
			cur->d.curfile = 0;
		} else {
			cur->d.curfile -= PAGELEN (cur);
			if (cur->d.topfile > cur->d.curfile)
				cur->d.topfile -= PAGELEN (cur);
			if (cur->d.topfile < 0) {
				cur->d.curfile -= cur->d.topfile;
				cur->d.topfile = 0;
			}
		}
		break;
	case cntrl ('K'):         /* find file */
		findname ();
		break;
	case cntrl ('R'):         /* reread catalog */
		reread (cur);
		break;
	case cntrl ('T'):         /* tag file */
		if ((cur->d.cat[cur->d.curfile].mode & S_IFMT) == (unsigned) S_IFREG) {
			cur->d.cat[cur->d.curfile].tag ^= 1;
			counttag (cur);
		}
		if (cur->d.curfile < cur->d.num-1) {
			/* move down */
			++cur->d.curfile;
			if (cur->d.topfile + PAGELEN (cur) - 1 < cur->d.curfile)
				cur->d.topfile = cur->d.curfile - PAGELEN (cur) + 1;
		}
		break;
//	case meta ('C'):          /* f3 */
//		if ((cur->d.cat[cur->d.curfile].mode & S_IFMT) != (unsigned) S_IFREG)
//			return;
//		view ();
//		draw.draw(cur, &left, &right);
//		return;
//	case meta ('D'):          /* f4 */
//		edit ();
//		setdir (cur==&left ? &right : &left, NULL);
//		setdir (cur, NULL);
//		draw.draw(cur, &left, &right);
//		return;
	case meta ('E'):          /* f5 */
		copy ();
		draw.draw(cur, left, right);
		return;
//	case meta ('F'):          /* f6 */
//		renmove ();
//		draw.draw(cur, &left, &right);
//		return;
//	case meta ('H'):          /* f8 */
//		mydelete ();
//		draw.draw(cur, &left, &right);
//		return;
	case cntrl ('L'):         /* status */
		setstatus ();
		draw.draw(cur, left, right);
		return;
	}
	draw.drawdir(cur, 1, left, right);
}

execute () {
	register struct file *file = &cur->d.cat[cur->d.curfile];
	register char *name = file->name;
	int updir, dev, ino;

	switch (file->mode & S_IFMT) {
	case S_IFDIR:
		if (! strcmp (name, ".")) {
//			setdir (cur, NULL);
			break;
		}
		if (updir = !strcmp (name, "..")) {
			dev = cur->d.dev;
			ino = cur->d.ino;
		}
		if (cur->chdir(name) < 0)
			break;
		setdir (cur, ".");
		if (updir) {
			for (cur->d.curfile=0; cur->d.curfile<cur->d.num; ++cur->d.curfile) {
				if (cur->d.cat[cur->d.curfile].dev == dev &&
				    cur->d.cat[cur->d.curfile].ino == ino)
					break;
			}
			if (cur->d.curfile >= cur->d.num)
				cur->d.curfile = 0;
			if (cur->d.topfile + PAGELEN (cur) <= cur->d.curfile)
				cur->d.topfile = cur->d.curfile - PAGELEN (cur) + 1;
		}
		if (visualwin) {
			draw.drawdir(left, 0, left, right);
			draw.drawdir(right, 0, left, right);
		}
		break;
//	case S_IFREG:
//		if (file->execable)
//			strcpy (command, file->name);
//		else
//			excommand (command, file->name);
//		cpos = strlen (command);
//		if (! command [0])
//			break;
//		cmd.exec(cur, &left, &right, file->execable ? 1 : 0, 1);
//		draw.draw(cur, &left, &right);
//		break;
	}
}

void init () {
	char *buf;

	buf = (char *) MemAlloc (2048);
	if (! CapInit (buf)) {
		outerr("cannot read termcap\n",0);
		exit (1);
	}
	if (! v.VInit ()) {
		outerr("cannot initialize terminal\n",0);
		exit (1);
	}
	if (COLS < 80) {
		outerr("must be 80 columns on terminal\n",0);
		exit (1);
	}
	KeyInit (keymap, VFlush);
	v.VOpen();
	MemFree ((mem *) buf);

	/* H is the number of lines at file window */

	H = LINES/2-1;

}

int quote(void) {
	register c, i;

	c = KeyGet ();
	if (c<'0' || c>'3')
		return (c);
	i = c & 3;
	c = KeyGet ();
	if (c<'0' || c>'7') {
		KeyUnget (c);
		return (i);
	}
	i = i << 3 | c & 7;
	c = KeyGet ();
	if (c<'0' || c>'7') {
		KeyUnget (c);
		return (i);
	}
	return (i << 3 | c & 7);
}

# define SYSTEMID "/etc/systemid"
/* main */
char *getmachine(void) {
	static char buf [16];
	register f;
	register char *p;

# ifdef HOSTNAME
	if (gethostname (buf, sizeof (buf) - 1) >= 0)
		return (buf);
# endif
	if ((f = open (SYSTEMID, 0)) >= 0) {
		read (f, buf, sizeof (buf) - 1);
		close (f);
		for (p=buf; *p; ++p)
			if (*p<' ' || *p>'~') {
				*p = 0;
				break;
			}
		return (buf);
	}
	return (0);
}

/* cmd, com */
void quitdeco(void) {
	VMove (LINES-1, 0);
	VClearLine ();
	VSync ();
	v.VClose ();
	exit (0);
}

main (int argc, char **argv, char **envp) {
	register c;

	if (argc > 2) {
		outerr("Usage: deco [dirname]\n",0);
		exit (1);
	}
	outerr("Demos Commander, Copyright (C) 1989-1994 Serge Vakulenko\n",0);
	palette = dflt_palette;
	EnvInit (envp);
	uid = getuid ();
	gid = getgid ();
# ifdef GROUPS
	gidnum = getgroups (sizeof(gidlist)/sizeof(gidlist[0]), (unsigned int *)gidlist);
# endif
	ppid = getppid ();
	user = username (uid);
	group = groupname (gid);
	tty = ttyname (0);
	machine = getmachine ();
#if 0
	sigign();
#else
	signal(SIGTERM, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
# ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
# endif
#endif
	init ();
//	inithome ();
	VClear ();
/* init class dir */
	if (argc > 1)
//		chdir (argv [1]);
		left = new dir(argv [1]);
	else
		left = new dir;
	right = new dir;
	left->d.basecol = 0;
	right->d.basecol = 40;
/*-----------*/
	initfile.read();
	if (uid == 0)
		palette.dimfg = 6;
	v.VSetPalette (palette.fg, palette.bg, palette.revfg, palette.revbg,
		palette.boldfg, palette.boldbg, palette.boldrevfg, palette.boldrevbg,
		palette.dimfg, palette.dimbg, palette.dimrevfg, palette.dimrevbg);
	setdir (left, ".");
	setdir (right, ".");
	left->chdir(left->d.cwd);
	cur = left;
	draw.draw(cur, left, right);
	for (;;) {
		if (! cmdreg)
			draw.drawcursor(cur);
//		cmd.drawcmd(cur, &left, &right);
		VSync ();
		c = KeyGet ();
		if (! cmdreg)
			draw.undrawcursor(cur);
		switch (c) {
		case '+':               /* select */
		case '-':               /* unselect */
			if (! cpos && ! cmdreg && ! cur->d.status) {
				if (c == '+')
					tagall ();
				else
					untagall ();
				draw.draw(cur, left, right);
				continue;
			}
		default:
//			if (c>=' ' && c<='~' || c>=0300 && c<=0376) {
//				if (cpos || c!=' ')
//					cmd.inscmd(c);
//				continue;
//			}
			VBeep ();
			continue;
//		case cntrl ('V'):       /* quote next char */
//			cmd.inscmd(quote ());
//			continue;
//		case cntrl ('J'):       /* insert file name */
//			if (! cmdreg && ! cur->status)
//				cmd.namecmd(cur);
//			continue;
//		case cntrl ('G'):
//			cmd.delcmd();
//			continue;
//		case meta ('b'):        /* backspace */
//			if (cpos) {
//				cmd.leftcmd();
//				cmd.delcmd();
//			}
//			continue;
		case cntrl ('O'):       /* set/unset command mode */
		case cntrl ('P'):       /* set/unset command mode */
			switchcmdreg ();
			if (! cmdreg)
				visualwin = 1;
			draw.draw(cur, left, right);
			continue;
		case cntrl ('M'):         /* return */
//			if (command [0]) {
//				cmd.exec(cur, &left, &right, 1, 1);
//				draw.draw(cur, &left, &right);
//				continue;
//			}
			if (cmdreg) {
				cmdreg = 0;
				if (! visualwin) {
					visualwin = 1;
					setdir (cur==left ? right : left, NULL);
					setdir (cur, NULL);
				}
				draw.draw(cur, left, right);
				continue;
			}
			execute ();
			continue;
		case cntrl (']'):       /* redraw screen */
			VRedraw ();
			continue;
//		case cntrl ('B'):        /* history */
//			if (! visualwin)
//				VClearBox (1, 0, LINES-2, 80);
//			cmd.histmenu(cur, &left, &right);
//			draw.draw(cur, &left, &right);
//			continue;
		case meta ('A'):        /* f1 */
			genhelp ();
			draw.draw(cur, left, right);
			continue;
		case meta ('B'):          /* f2 */
			udm.menu();
			draw.draw(cur, left, right);
			continue;
		case meta ('I'):        /* f9 */
			mymenu.runmenu (cur==left ? 'l' : 'r');
			draw.draw(cur, left, right);
			continue;
		case meta ('J'):        /* f0 */
		case cntrl ('C'):       /* quit */
			quit ();
			continue;
		case cntrl ('U'):       /* swap panels */
			swappanels ();
			draw.draw(cur, left, right);
			continue;
		case cntrl ('F'):       /* full screen */
			fullscreen ();
			draw.draw(cur, left, right);
			continue;
		case cntrl ('^'):       /* cd / */
			directory (0, 'r');
			if (! cur->d.status)
				draw.drawdir(cur, 1, left, right);
			continue;
		case cntrl ('\\'):      /* cd $HOME */
			directory (0, 'o');
			if (! cur->d.status)
				draw.drawdir(cur, 1, left, right);
			continue;
//		case cntrl ('Y'):       /* clear line */
//			command [cpos = 0] = 0;
//			continue;
//		case cntrl ('X'):       /* next history */
//			cmd.nextcmd();
//			continue;
//		case cntrl ('E'):       /* prev history */
//			cmd.prevcmd();
//			continue;
//		case cntrl ('S'):       /* char left */
//		case cntrl ('A'):       /* char left */
//			cmd.leftcmd();
//			continue;
//		case cntrl ('D'):       /* char right */
//			cmd.rightcmd();
//			continue;
		case cntrl ('I'):       /* tab */
			if (cmdreg) {}
//				if (command [cpos])
//					cmd.endcmd();
//				else
//					cmd.homecmd();
			else {
				switchpanels ();
				if (fullwin) {
					draw.drawbanners();
					draw.drawdir(cur, 0, left, right);
					break;
				}
			}
			continue;
		case cntrl ('W'):       /* double width */
			if (! cmdreg) {
				setdwid ();
				draw.draw(cur, left, right);
			}
			continue;
//		case meta ('G'):        /* f7 */
//			makedir ();
//			draw.draw(cur, &left, &right);
//			continue;
		case meta ('h'):        /* home */
		case meta ('e'):        /* end */
		case meta ('u'):        /* up */
		case meta ('d'):        /* down */
		case meta ('l'):        /* left */
		case meta ('r'):        /* right */
		case meta ('n'):        /* next page */
		case meta ('p'):        /* prev page */
		case cntrl ('K'):       /* find file */
		case cntrl ('R'):       /* reread catalog */
		case cntrl ('T'):       /* tag file */
		case meta ('C'):        /* f3 */
		case meta ('D'):        /* f4 */
		case meta ('E'):        /* f5 */
		case meta ('F'):        /* f6 */
		case meta ('H'):        /* f8 */
		case cntrl ('L'):       /* status */
			if (cmdreg || cur->d.status) {}
//				docmdreg (c);
			else
				doscrreg (c);
			continue;
		}
	}
}

/* VARARGS1 */
/* draw, edit, menu, view */
void printw(char *fmt, int a, int b, int c, int d, int e, int f, int g, int h) {
	char buf [512];

	sprintf (buf, fmt, a,b,c,d,e,f,g,h);
	VPutString (buf);
}
