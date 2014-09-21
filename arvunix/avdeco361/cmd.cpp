/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include "stdio.h"
# include "unistd.h"
# include "fcntl.h"
# include "string.h"
# include <sys/types.h>
# include <sys/stat.h>

# include "dir.h"
# include "file.h"
# include "deco.h"
# include "scr.h"
# include "mem.h"
# include "env.h"
# include "choice.h"
# include "ut.h"
# include "run.h"
# include "match.h"
//# include "view.h"	/* class VIEWFILE */
//# include "edit.h"	/* class EDITFILE */
# include "main.h"	// rfiles;
#include "TapeStill.h"

# ifndef NULL
#    define NULL        ((char *) 0)
# endif

# define SHELL          (usecshell ? cshname : shname)
# define ABSSHELL       (usecshell ? cshabsname : shabsname)

# define MAXARGV 400

static char shname [] = "sh";
static char cshname [] = "csh";
static char shabsname [] = "/bin/sh";
static char cshabsname [] = "/bin/csh";

int usecshell;
int useredit, userview;
int showhidden = 1;
char editname [40], viewname [40];

extern char *home;

char *sysmsg [] = {
	0,
	"Hangup",               /* SIGHUP  1  hangup */
	"Interrupt",            /* SIGINT  2  interrupt (rubout) */
	"Quit",                 /* SIGQUIT 3  quit */
	"Illegal instruction",  /* SIGILL  4  illegal instruction */
	"Trace/BPT trap",       /* SIGTRAP 5  trace trap */
	"IOT trap",             /* SIGIOT  6  IOT instruction */
	"EMT trap",             /* SIGEMT  7  EMT instruction */
	"Floating exception",   /* SIGFPE  8  floating point exception */
	"Killed",               /* SIGKILL 9  kill */
	"Bus error",            /* SIGBUS  10 bus error */
	"Memory fault",         /* SIGSEGV 11 segmentation violation */
	"Bad system call",      /* SIGSYS  12 bad argument to system call */
	"Broken pipe",          /* SIGPIPE 13 write on a pipe with no one to read it */
	"Alarm call",           /* SIGALRM 14 alarm clock */
	"Terminated",           /* SIGTERM 15 software termination signal from kill */
};

int numsysmsg = (sizeof sysmsg / sizeof sysmsg[0]);

/* ARGSUSED */

/* main, menu */
void directory(int k, int sk) {
	setdir (cur, sk == 'r' ? "/" : home);
}

/* menu */
void switchalign (int k) {
	dir *d = k=='l' ? left : right;

	d->d.alignext ^= 1;
}

/* main, menu */
void switchcmdreg (void) {
	cmdreg ^= 1;
}

/* menu */
void switchhidden (void) {
	showhidden ^= 1;
	setdir (cur == left ? right : left, NULL);
	setdir (cur, NULL);
}

/* main, menu */
void switchpanels (void) {
	cur = cur==left ? right : left;
	cur->chdir(cur->d.cwd);
}

/* main, menu */
void setstatus (void) {
	if (cur == left)
		right->d.status ^= 1;
	else
		left->d.status ^= 1;
}

/* com, main, menu */
void fullscreen (void) {
	VClearBox (1, 0, LINES-2, 80);
	if (H == LINES/2-1)
		H = LINES-7;
	else
		H = LINES/2-1;
	while (left->d.topfile + PAGELEN (left) <= left->d.curfile)
		left->d.topfile += H;
	while (right->d.topfile + PAGELEN (right) <= right->d.curfile)
		right->d.topfile += H;
}

/* main, menu */
void reread (dir *d) {
	register char *p;

	if (! (p = getwstring (50, d->d.cwd, " Change directory ", "Change directory to")))
		return;
	if (! strcmp (p, d->d.cwd))
		setdir (d, NULL);
	else if (cur->chdir(p) < 0)
		error ("Cannot chdir to %s", p);
	else
		setdir (d, ".");
	d->chdir(d->d.cwd);
}

/* main, menu */
void setdwid (void) {
	VClearBox (1, 0, LINES-2, 80);
	fullwin ^= 1;
	while (left->d.topfile + PAGELEN (left) <= left->d.curfile)
		left->d.topfile += H;
	while (right->d.topfile + PAGELEN (right) <= right->d.curfile)
		right->d.topfile += H;
}

/* menu */
void setfull (int k, int sk) {
	register dir *c = k=='l' ? left : right;

	switch (sk) {
	case 'b':
		c->d.full = 0;
		break;
	case 'l':
		c->d.full = 1;
		break;
	case 'f':
		c->d.full = 2;
		if (! fullwin)
			setdwid ();
		break;
	}
	while (c->d.topfile + PAGELEN (c) <= c->d.curfile)
		c->d.topfile += H;
}

/* menu */
void setsort (int k, int sk) {
	register dir *c = k=='l' ? left : right;

	switch (sk) {
	case 'n':       c->d.sort = SORTNAME;     break;
	case 'x':       c->d.sort = SORTEXT;      break;
	case 't':       c->d.sort = SORTTIME;     break;
	case 'z':       c->d.sort = SORTSIZE;     break;
	case 'u':       c->d.sort = SORTSKIP;     break;
	}
	setdir (c, NULL);
	cur->chdir(cur->d.cwd);
}

/* menu */
void settypesort (int k) {
	register dir *c = k=='l' ? left : right;

	c->d.typesort ^= 1;
	setdir (c, NULL);
	cur->chdir(cur->d.cwd);
}

/* menu */
void setrevsort (int k) {
	register dir *c = k=='l' ? left : right;

	c->d.revsort ^= 1;
	setdir (c, NULL);
	cur->chdir(cur->d.cwd);
}

/* menu */
void setviewname (void) {
	register char *p;

	switch (getchoice (0, " View ", "Select which viewer to use for F3:",
	    NULL, " Built-in ", " External ", NULL)) {
	case 0:
		userview = 0;
	default:
		return;
	case 1:
		break;
	}
	if (! (p = getstring (40, viewname, " View ", "Viewer to use")))
		return;
	strncpy (viewname, p, 40);
	viewname [39] = 0;
	userview = 1;
}

/* menu */
void setshellname (void) {
	switch (getchoice (0, " Shell ", "Select which shell to use:",
	    NULL, " Shell ", " Cshell ", NULL)) {
	case 0:
		usecshell = 0;
		break;
	case 1:
		usecshell = 1;
		break;
	}
}

#if 0
/* menu */
void seteditname (void) {
	register char *p;

	switch (getchoice (0, " Edit ", "Select which editor to use for F4:",
	    NULL, " Built-in ", " External ", NULL)) {
	case 0:
		useredit = 0;
	default:
		return;
	case 1:
		break;
	}
	if (! (p = getstring (40, editname, " Edit ", "Editor to use")))
		return;
	strncpy (editname, p, 40);
	editname [39] = 0;
	useredit = 1;
}
#endif

/* local */
int metas (register char *s) {
	/* Are there are any Shell meta-characters? */
	while (*s)
		switch (*s++) {
		case '|':       case '^':       case ';':       case '&':
		case '(':       case ')':       case '<':       case '>':
		case '[':       case ']':       case '*':       case '?':
		case '\'':      case '\"':      case '\\':      case '`':
		case '$':       case '~':
		/* case '\n': case '=': case ':': */
		return (1);
		}
	return (0);
}

#if 0
/* local */
int doexec (register char *str) {
	register char *t;
	char *argv [MAXARGV+1];
	register char **p;
	int status;

	while (*str == ' ' || *str == '\t')
		++str;
	if (! *str)
		return (-1);		/* no command */
	t = (char *) MemAlloc (strlen (str) + 1);
	strcpy (t, str);
	str = t;
	p = argv;
	for (t = str; *t;) {
		if (p >= argv+MAXARGV) {
			error ("%s: Too many arguments", str);
			break;
		}
		*p++ = t;
		while (*t && *t!=' ' && *t!='\t')
			++t;
		if (*t)
			for (*t++ =0; *t==' ' || *t=='\t'; ++t);
	}
	*p = 0;
	status = runv (0, str, argv);
	MemFree ((mem *) str);
	return (status);
}
#endif

#if 0
/* com */
void syscmd (char *s) {
	register status, sig;

	if (metas (s)) {
		if (usecshell)
			runl(0, cshabsname, cshname, "-f", "-c", s, NULL);
		else
			runl(0, shabsname, shname, "-c", s, NULL);
		return;
	}
	status = doexec (s);
	sig = status & 0177;
	if (! sig)
		return;
	if (sig == 0177)
		outerr("ptrace: ",0);
	else if (sig < numsysmsg && sysmsg [sig])
		outerr(sysmsg [sig],0);
	else
		outerr("Signal %d", sig);
	if (status & 0200)
		outerr(" - core dumped\n",0);
	else
		outerr("\n",0);
}
#endif

#if 0
/* main, menu */
void view (void) {
char buf [80];
register char *name = cur->d.cat[cur->d.curfile].name;
register d;

	if (userview) {
		strcpy (buf, viewname);
		strcat (buf, " ");
		strcat (buf, name);
		VRestore ();
		syscmd (buf);
		VReopen ();
		VRedraw ();
		setdir (cur == left ? right : left, NULL);
		setdir (cur, NULL);
		return;
	}
	if ((d = open (name, 0)) < 0) {
		error ("Cannot open %s", name);
		return;
	}
	VIEWFILE viewfile(d, name);
	close (d);
}
#endif

#if 0
/* local */
void editfnam (register char *name) {
register d;
register char *p;
char buf [80];
struct stat st;

	if (stat (name, &st) >= 0) {
		if ((st.st_mode & S_IFMT) != S_IFREG) {
			error ("Cannot edit special files");
			return;
		}
	} else {
		d = creat (name, 0644);
		if (d < 0) {
			error ("Cannot create %s", name);
			return;
		}
		close (d);
	}
	if (useredit) {
		strcpy (buf, editname);
		strcat (buf, " ");
		strcat (buf, name);
		VRestore ();
		syscmd (buf);
		VReopen ();
		VRedraw ();
		setdir (cur == left ? right : left, NULL);
		setdir (cur, NULL);
		return;
	}
	if ((d = open (name, 2)) < 0) {
		error ("Cannot open %s for writing", name);
		return;
	}
	/* skip directory name */
	for (p=name; *p; ++p);
	for (; p>=name && *p!='/'; --p);
	EDITFILE editfile(d, name, p+1);
	close (d);
}
#endif

#if 0
/* main, menu */
void edit (void) {
	register char *name = cur->d.cat[cur->d.curfile].name;
	char namebuf [50];

	if (! strcmp (name, ".")) {
		if (! (name = getstring (40, NULL, " Edit ", "Enter name of file to edit")))
			return;
		strcpy (namebuf, name);
		editfnam (namebuf);
		return;
	}
	editfnam (cur->d.cat[cur->d.curfile].name);
}
#endif

#if 0
/* menu */
void menuedit (void) {
	char buf [80];

	switch (getchoice (0, " Menu Edit ", "Select which menu to edit:",
	    NULL, " Current menu ", " Home menu ", NULL)) {
	case 0:
		editfnam (".menu");
		break;
	case 1:
		strcpy (buf, home);
		strcat (buf, "/.menu");
		editfnam (buf);
		break;
	}
}
#endif

#if 0
/* menu */
void extedit (void) {
	char buf [80];

	strcpy (buf, home);
	strcat (buf, "/.deco");
	editfnam (buf);
}
#endif

/* main, menu */
void quit (void) {
	int choice;

	choice = getchoice (ppid == 1, " Demos Commander ",
		ppid == 1 ? "Do you want to log out ?" :
		"Do you want to quit the Demos Commander ?",
		NULL, " Yes ", " No ", " Exec shell ");
	if (choice == 0)
		quitdeco ();
	if (choice == 2) {
		/* exec shell */
		VClear ();
		VSync ();
		v.VClose();
		execle (ABSSHELL, SHELL, "-i", NULL, EnvVector);
		exit (0);
	}
	/* else stay here */
}

/* main, menu */
void swappanels (void) {
	dir *dir;
	int ro, lo;

	ro = right->d.shortcwd - right->d.cwd;
	lo = left->d.shortcwd - left->d.cwd;
	dir = left;
	left = right;
	right = dir;
	left->d.basecol = 0;
	right->d.basecol = 40;
	right->d.shortcwd = lo + right->d.cwd;
	left->d.shortcwd = ro + left->d.cwd;
	cur = (cur == left ? right : left);
}

#if 0
/* menu */
void shell (void) {
	/* run shell */
	VClear ();
	VSync ();
	VRestore ();
	runl(0, ABSSHELL, SHELL, "-i", NULL);
	VReopen ();
	VClear ();
	setdir (cur == left ? right : left, NULL);
	setdir (cur, NULL);
}
#endif

/* com */
int tagged (void) {
	register i, n;

	for (i=n=0; i<cur->d.num; ++i)
		if (cur->d.cat[i].tag)
			++n;
	return (n);
}

int exist(dir *d, char *name) {
	struct stat st;

	if (d->stat(name, &st) < 0)
		return (0);
	switch (st.st_mode & S_IFMT) {
	case S_IFDIR:
		return ('d');
	case S_IFREG:
		return ('f');
	default:
		return ('?');
	}
}

/* main, menu */
void copy (void) {
	char buf [80];
	char *name;
	register struct file *p;
	register c;
	int done, all, ch;
	int exd, exf;

	if (c = tagged ()) {
		/* copy group of files */

		sprintf (buf, "Copy %d file%s to", c, c>1 ? "s" : "");
		if (! (name = getstring (60, (cur==left?right:left)->d.cwd,
		    " Copy ", buf)))
			return;
#if 0
		runset (name);
		exd = exist (name);
		all = 0;
		for (p = cur->d.cat; p < cur->d.cat + cur->d.num; ++p)
			if (p->tag) {
				sprintf (buf, "%s/%s", name, p->name);
				exf = exd=='d' ? exist (buf) : 0;
				if (exd=='f' || exf=='f') {
					if (! all) {
						ch = getchoice (1, " Copy ", "File exists",
							exf=='f' ? buf : name,
							" Overwrite ", " All ", " Cancel ");
						switch (ch) {
						case 2:         /* cancel */
							--c;
							continue;
						case 1:         /* all */
							all = 1;
						case 0:         /* all */
							break;
						default:
							runcancel ();
							return;
						}
					}
				} else if (exf == 'd') {
					error ("%s is a directory", buf);
					continue;
				}
				runarg (p->name);
				p->tag = 0;
			}
		if (c <= 0) {
			runcancel ();
			return;
		}
		message (" Copy ", "Copying %d file%s to %s ...", c, c>1 ? "s" : "", name);
		done = rundone ("/bin/cp", "cp");
		if (done) {
			endmesg ();
			message (" Copy ", "Done");
		} else
			error ("Error while copying %d file%s to %s",
				c, c>1 ? "s" : "", name);
		endmesg ();
#endif
	} else if ((c = (p = &cur->d.cat[cur->d.curfile])->mode & S_IFMT) == S_IFDIR) {
		/* copying directory */
	} else if (c == S_IFREG) {
		/* copy regular file */

		sprintf (buf, "Copy \"%s\" to", p->name);
		if (! (name = getstring (60, (cur==left?right:left)->d.cwd, " Copy ", buf)))
			return;
		exd = exist (cur==left?right:left, name);
		sprintf (buf, "%s/%s", name, p->name);
		exf = exd=='d' ? exist (cur==left?right:left, buf) : 0;
		if (exd=='f' || exf=='f') {
			if (getchoice (1, " Copy ", "File exists",
			    exf=='f' ? buf : name,
			    " Overwrite ", " Cancel ", NULL))
				return;
		} else if (exf == 'd') {
			error ("%s is a directory", buf);
			return;
		}
		/* copying */
		ReadWrite rf;
		strncpy(rf.from, cur->d.cwd, sizeof(rf.from));
		strncat(rf.from, "/", sizeof(rf.from));
		strncat(rf.from, p->name, sizeof(rf.from));
		strncpy(rf.to, buf, sizeof(rf.to));
		rfiles.AddReadFile(&rf);
	}
	setdir (cur == left ? right : left, NULL);
	setdir (cur, NULL);
}

#if 0
/* menu */
void makelink (void) {
	char buf [80];
	char *name;
	register struct file *p;
	register c;
	int done;
	int exd, exf;

	if (c = tagged ()) {
		/* link group of files */

		sprintf (buf, "Link %d file%s to", c, c>1 ? "s" : "");
		if (! (name = getstring (60, (cur==left?right:left)->cwd,
		    " Link ", buf)))
			return;
		runset (name);
		exd = exist (name);
		for (p=cur->d.cat; p<cur->d.cat+cur->d.num; ++p)
			if (p->tag) {
				sprintf (buf, "%s/%s", name, p->name);
				exf = exd=='d' ? exist (buf) : 0;
				if (exd=='f' || exf=='f') {
					if (getchoice (1, " Link ", "File exists",
					    exf=='f' ? buf : name,
					    " Overwrite ", " Cancel ", NULL))
						continue;
				} else if (exf == 'd') {
					error ("%s is a directory", buf);
					continue;
				}
				runarg (p->name);
				p->tag = 0;
			}
		message (" Link ", "Linking %d file%s to %s ...", c, c>1 ? "s" : "", name);
		done = rundone ("/bin/ln", "ln");
		if (done) {
			endmesg ();
			message (" Link ", "Done");
		} else
			error ("Error while linking %d file%s to %s",
				c, c>1 ? "s" : "", name);
		endmesg ();
	} else if ((c = (p = &cur->d.cat[cur->d.curfile])->mode & S_IFMT) == S_IFDIR) {
		/* linking directory */
	} else if (c == S_IFREG) {
		/* link regular file */

		sprintf (buf, "Link \"%s\" to", p->name);
		if (! (name = getstring (60, (cur==left?right:left)->cwd, " Link ", buf)))
			return;
		exd = exist (name);
		sprintf (buf, "%s/%s", name, p->name);
		exf = exd=='d' ? exist (buf) : 0;
		if (exd=='f' || exf=='f') {
			if (getchoice (1, " Link ", "File exists",
			    exf=='f' ? buf : name,
			    " Overwrite ", " Cancel ", NULL))
				return;
		} else if (exf == 'd') {
			error ("%s is a directory", buf);
			return;
		}
		if (runl(1, "/bin/ln", "ln", p->name, name, NULL)) {
			error ("Cannot link %s to %s", p->name, name);
			return;
		}
	}
	setdir (cur == left ? right : left, NULL);
	setdir (cur, NULL);
}
#endif

#if 0
# ifdef S_IFLNK
/* menu */
void makeslink (void) {
	char buf [80];
	char *name;
	register struct file *p;
	register c;
	int done;
	int exd, exf;

	if (c = tagged ()) {
		/* symlink group of files */

		sprintf (buf, "Symlink %d file%s to", c, c>1 ? "s" : "");
		if (! (name = getstring (60, (cur==left?right:left)->cwd,
		    " Symlink ", buf)))
			return;
		runset (name);
		exd = exist (name);
		for (p=cur->d.cat; p<cur->d.cat+cur->d.num; ++p)
			if (p->tag) {
				sprintf (buf, "%s/%s", name, p->name);
				exf = exd=='d' ? exist (buf) : 0;
				if (exd=='f' || exf=='f') {
					if (getchoice (1, " Symlink ", "File exists",
					    exf=='f' ? buf : name,
					    " Overwrite ", " Cancel ", NULL))
						continue;
				} else if (exf == 'd') {
					error ("%s is a directory", buf);
					continue;
				}
				runarg (p->name);
				p->tag = 0;
			}
		runarg ("-s");
		message (" Symlink ", "Linking %d file%s to %s ...", c, c>1 ? "s" : "", name);
		done = rundone ("/bin/ln", "ln");
		if (done) {
			endmesg ();
			message (" Symlink ", "Done");
		} else
			error ("Error while linking %d file%s to %s",
				c, c>1 ? "s" : "", name);
		endmesg ();
	} else if ((c = (p = &cur->d.cat[cur->d.curfile])->mode & S_IFMT) == S_IFDIR) {
		/* symlinking directory */
	} else if (c == S_IFREG) {
		/* symlink regular file */

		sprintf (buf, "Symlink \"%s\" to", p->name);
		if (! (name = getstring (60, (cur==left?right:left)->cwd, " Symlink ", buf)))
			return;
		exd = exist (name);
		sprintf (buf, "%s/%s", name, p->name);
		exf = exd=='d' ? exist (buf) : 0;
		if (exd=='f' || exf=='f') {
			if (getchoice (1, " Symlink ", "File exists",
			    exf=='f' ? buf : name,
			    " Overwrite ", " Cancel ", NULL))
				return;
		} else if (exf == 'd') {
			error ("%s is a directory", buf);
			return;
		}
		if (runl(1, "/bin/ln", "ln", "-s", p->name, name, NULL)) {
			error ("Cannot symlink %s to %s", p->name, name);
			return;
		}
	}
	setdir (cur == left ? right : left, NULL);
	setdir (cur, NULL);
}
# endif /* S_IFLNK */
#endif

/* local */
void findfile(register struct dir *d, char *name) {
	/* set current file by name */
	register struct file *f;

	for (f=d->d.cat; f<d->d.cat+d->d.num; ++f)
		if (! strncmp (f->name, name, NAMESZ-1)) {
			d->d.curfile = f - d->d.cat;
			break;
		}
	while (d->d.topfile > d->d.curfile)
		d->d.topfile -= H;
	if (d->d.topfile < 0)
		d->d.topfile = 0;
	while (d->d.topfile + PAGELEN (d) <= d->d.curfile)
		cur->d.topfile += H;
}

#if 0
/* main, menu */
void renmove (void) {
	char buf [MAXPATHLEN+20];
	char *name;
	register struct file *p;
	register c;
	int done, all, ch;
	int exd, exf;

	if (c = tagged ()) {
		/* move group of files */

		sprintf (buf, "Rename or move %d file%s to", c, c>1 ? "s" : "");
		if (! (name = getstring (60, (cur==left?right:left)->cwd,
		    " Rename ", buf)))
			return;
		runset (name);
		exd = exist (name);
		all = 0;
		for (p=cur->d.cat; p<cur->d.cat+cur->d.num; ++p)
			if (p->tag) {
				sprintf (buf, "%s/%s", name, p->name);
				exf = exd=='d' ? exist (buf) : 0;
				if (exd=='f' || exf=='f') {
					if (! all) {
						ch = getchoice (1, " Rename ", "File exists",
							exf=='f' ? buf : name,
							" Overwrite ", " All ", " Cancel ");
						switch (ch) {
						default:
						case 2:         /* cancel */
							continue;
						case 1:         /* all */
							all = 1;
						}
					}
				} else if (exf == 'd') {
					error ("%s is a directory", buf);
					continue;
				}
				runarg (p->name);
			}
		message (" Move ", "Moving %d file%s to %s ...", c, c>1 ? "s" : "", name);
		done = rundone ("/bin/mv", "mv");
		if (done) {
			endmesg ();
			message (" Move ", "Done");
		} else
			error ("Error while moving %d file%s to %s",
				c, c>1 ? "s" : "", name);
		endmesg ();
		name = 0;
	} else if ((c = (p = &cur->d.cat[cur->d.curfile])->mode & S_IFMT) == S_IFDIR) {
		/* rename directory */

		sprintf (buf, "Rename \"%s\" to", strtail (p->name, '/', 60));
		if (! (name = getstring (60, (cur==left?right:left)->cwd, " Rename ", buf)))
			return;
		exd = exist (name);
		sprintf (buf, "%s/%s", name, p->name);
		exf = exd=='d' ? exist (buf) : 0;
		if (exd=='f' || exf=='f') {
			if (getchoice (1, " Rename ", "File exists",
			    exf=='f' ? buf : name,
			    " Overwrite ", " Cancel ", NULL))
				return;
		} else if (exf == 'd') {
			error ("Directory %s exists", buf);
			return;
		}
		if (runl(1, "/bin/mv", "mv", p->name, name, NULL)) {
			error ("Cannot move %s to %s", p->name, name);
			return;
		}
	} else if (c == S_IFREG) {
		/* move regular file */

		sprintf (buf, "Rename or move \"%s\" to", p->name);
		if (! (name = getstring (60, (cur==left?right:left)->cwd, " Rename ", buf)))
			return;
		exd = exist (name);
		sprintf (buf, "%s/%s", name, p->name);
		exf = exd=='d' ? exist (buf) : 0;
		if (exd=='f' || exf=='f') {
			if (getchoice (1, " Rename ", "File exists",
			    exf=='f' ? buf : name,
			    " Overwrite ", " Cancel ", NULL))
				return;
		} else if (exf == 'd') {
			error ("%s is a directory", buf);
			return;
		}
		if (runl(1, "/bin/mv", "mv", p->name, name, NULL)) {
			error ("Cannot move %s to %s", p->name, name);
			return;
		}
	}
	setdir (cur == left ? right : left, NULL);
	setdir (cur, NULL);
	if (name)
		findfile (cur, name);
}
#endif

#if 0
/* main, menu */
void makedir (void) {
	register char *p;

	if (! (p = getstring (60, NULL, " Make directory ", "Create the directory")))
		return;
	switch (exist (p)) {
	default:
	case 'f':
		error ("File %s exists", p);
		break;
	case 'd':
		error ("Directory %s already exists", p);
		break;
	case 0:
		if (runl(1, "/bin/mkdir", "mkdir", p, NULL)) {
			error ("Cannot create directory %s", p);
			break;
		}
		setdir (cur == left ? right : left, NULL);
		setdir (cur, NULL);
		findfile (cur, p);
		break;
	}
}
#endif

#if 0
/* main, menu */
void mydelete (void) {
	char buf [80];
	register struct file *p;
	register c;

	if (c = tagged ()) {
		/* delete group of files */

		sprintf (buf, "You have selected %d file%s.", c, c>1 ? "s" : "");
		if (getchoice (0, " Delete ", buf, NULL, " Delete ", " Cancel ", NULL))
			return;
		sprintf (buf, "You are DELETING %d selected file%s from", c, c>1 ? "s" : "");
		if (getchoice (1, " Delete ", buf, cur->d.cwd, " Ok ", " Cancel ", NULL))
			return;
		for (p=cur->d.cat; p<cur->d.cat+cur->d.num; ++p)
			if (p->tag && unlink (p->name) < 0)
				error ("Cannot delete %s", p->name);
	} else if ((c = (p = &cur->d.cat[cur->d.curfile])->mode & S_IFMT) == S_IFDIR) {
		/* delete directory */

		if (getchoice (0, " Delete ", "Do you wish to delete directory",
		    p->name, " Delete ", " Cancel ", NULL))
			return;
		if (runl(1, "/bin/rmdir", "rmdir", p->name, NULL)) {
			error ("Cannot delete directory %s", p->name);
			return;
		}
	} else if (c == S_IFREG) {
		/* delete regular file */

		if (getchoice (0, " Delete ", "Do you wish to delete",
		    p->name, " Delete ", " Cancel ", NULL))
			return;
		if (unlink (p->name) < 0) {
			error ("Cannot delete %s", p->name);
			return;
		}
	}
	setdir (cur == left ? right : left, NULL);
	setdir (cur, NULL);
}
#endif

/* main, menu */
void findname (void) {
	register char *s, *q;
	register char *p;
	register struct file *f;

	if (! (p = getwstring (20, "", " Find file ", "Enter file name")) || ! *p)
		return;
	for (f=cur->d.cat; f<cur->d.cat+cur->d.num; ++f) {
		s=p;
		q=f->name;
		for (;;) {
			if (! *s) {
				cur->d.curfile = f - cur->d.cat;
				if (cur->d.topfile > cur->d.curfile || cur->d.topfile + PAGELEN (cur) <= cur->d.curfile)
					cur->d.topfile = cur->d.curfile - PAGELEN (cur) + 1;
				while (cur->d.topfile + PAGELEN (cur) - H > cur->d.curfile)
					cur->d.topfile -= H;
				if (cur->d.topfile < 0)
					cur->d.topfile = 0;
				return;
			} else if (*s != *q)
				break;
			++s;
			++q;
		}
	}
	error ("File not found");
}

/* main, menu */
void tagall (void) {
	register char *p;
	register struct file *f;
	static char pat [20];

	if (! pat[0])
		strcpy (pat, "*");
	if (! (p = getwstring (20, pat, " Select ", "Select the files")))
		return;
	strncpy (pat, p, 20);
	pat [19] = 0;
	for (f=cur->d.cat; f<cur->d.cat+cur->d.num; ++f)
		if ((f->mode & S_IFMT) == (unsigned) S_IFREG &&
		    (! pat[0] || match (f->name, pat)))
			f->tag = 1;
	counttag (cur);
}

/* main, menu */
void untagall (void) {
	register char *p;
	register struct file *f;
	static char pat [20];

	if (! pat[0])
		strcpy (pat, "*");
	if (! (p = getwstring (20, pat, " Unselect ", "Unselect the files")))
		return;
	strncpy (pat, p, 20);
	pat [19] = 0;
	for (f=cur->d.cat; f<cur->d.cat+cur->d.num; ++f)
		if ((f->mode & S_IFMT) == (unsigned) S_IFREG &&
		    (! pat[0] || match (f->name, pat)))
			f->tag = 0;
	counttag (cur);
}

/* menu */
void setpattern (int k) {
	dir *d = k=='l' ? left : right;
	register char *p;

	if (! (p = getwstring (20, d->d.pattern, " Set pattern ", "Set pattern for files")))
		return;
	strncpy (d->d.pattern, p, PATSZ);
	d->d.pattern [PATSZ-1] = 0;
	setdir (d, NULL);
	cur->chdir(cur->d.cwd);
}

/* local */
int cmpfil (char *d1, char *f1, char *d2, char *f2) {
	register i;
	int fd1, fd2;
	char buf1 [512], buf2 [512];
	int n1, n2, rez;

	sprintf (buf1, "%s/%s", d1, f1);
	sprintf (buf2, "%s/%s", d2, f2);
	if ((fd1 = open (buf1, 0)) < 0)
		return (0);
	if ((fd2 = open (buf2, 0)) < 0) {
		close (fd1);
		return (0);
	}
	rez = 0;
	for (;;) {
		n1 = read (fd1, buf1, sizeof (buf1));
		n2 = read (fd2, buf2, sizeof (buf2));
		if (n1 < 0 || n2 < 0 || n1 != n2)
			break;
		if (n1 == 0) {
			rez = 1;
			goto breakloop;
		}
		for (i=0; i<n1; ++i)
			if (buf1 [i] != buf2 [i])
				goto breakloop;
	}
breakloop:
	close (fd1);
	close (fd2);
	return (rez);
}

/* menu */
void cmpdir (void) {
	register struct file *f, *of;
	register dir *d, *od;
	struct file *end, *oend;
	int cmp;

	d = left;
	od = right;
	end = d->d.cat + d->d.num;
	oend = od->d.cat + od->d.num;
	for (of=od->d.cat; of<oend; ++of)
		if ((of->mode & S_IFMT) == (unsigned) S_IFREG)
			of->tag = 1;
	for (f=d->d.cat; f<end; ++f)
		if ((f->mode & S_IFMT) == (unsigned) S_IFREG)
			f->tag = 1;
	for (f=d->d.cat, of=od->d.cat; of<oend; ++of) {
		while ((cmp = compfile (f, of)) < 0)
			if (++f >= end)
				goto breakloop;
		if (! of->tag || ! f->tag)
			continue;
		if (cmp || f->size != of->size)
			continue;
		if (cmpfil (d->d.cwd, f->name, od->d.cwd, of->name))
			f->tag = of->tag = 0;
	}
breakloop:
	counttag (d);
	counttag (od);
}

/* menu */
#include "tape.h"
void Ident() {
int	rc;
char	name[9];
char	buf[256];
TPB	tpb;

	getchoice(0, " Ident ", "Identification tape", NULL, NULL, NULL, NULL);
	TapeStill ts("/dev/ravt0");
	rc = ts.Ident();
	endmesg();
	if (rc != TSOK) {
		getchoice(0, " Ident ", "Error identification.", NULL, NULL, NULL, NULL);
		getchar();
		endmesg();
		return;
	}
	ts.GetTape(&tpb);
	bzero(name, sizeof(name));
	switch(tpb.format) {
	case	FORMAT_K4:
	case	FORMAT_K7:
	case	FORMAT_K7CRC:
	case	FORMAT_RS:
		strncpy(name, tpb.name, 8);
	}
	getchoice(0, " Ident ", name, NULL, NULL, NULL, NULL);
//	printf("Format:\t%02X\n", tpb.format);
	getchar();
	endmesg();
};
