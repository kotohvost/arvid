/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

/* width of directory window */

# define PAGEWID        (fullwin ? 77 : 39)

/* number of displayable files in directory window */

# define PAGELEN(c)     pagelen ((c)->d.full)

# define BASECOL(c)     (fullwin ? 0 : (c)->d.basecol)

# define CMDLEN 512                     /* length of command string */

extern dir *left, *right;	/* left and right directories */
extern dir *cur;		/* current directory */
extern fullwin;                         /* full window mode */
extern H;                               /* file window height */
extern cmdreg;                          /* command line mode */
extern char command [];                 /* command line */
extern cpos;                            /* command line cursor */
extern char *user;                      /* user name */
extern char *group;                     /* user group name */
extern char *tty;                       /* tty name */
extern char *machine;                   /* machine name */
extern visualwin;                       /* cursor in command line */
extern showhidden;                      /* show hidden files ".*" */
extern gid;                             /* real group id */
extern uid;                             /* real user id */
extern ppid;                            /* real parent id */

struct palette {
	int fg, bg;
	int revfg, revbg;
	int boldfg, boldbg;
	int boldrevfg, boldrevbg;
	int dimfg, dimbg;
	int dimrevfg, dimrevbg;
};
