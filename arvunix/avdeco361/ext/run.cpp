/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include <signal.h>
# include <errno.h>
# include <setjmp.h>
#include "unistd.h"
#include "sys/wait.h"
# include "mem.h"
# include "env.h"
#include "ut.h"

# ifndef NULL
#    define NULL ((char *) 0)
# endif

# ifdef VFORK
#    define FORK vfork
# else
#    define FORK fork
# endif

# ifndef SIGTYPE
#    define SIGTYPE int
# endif

# define QUANT 16

//typedef (*funcptr) ();

static char **arg;
static arglen, argp;

extern errno;
extern execve ();

# ifdef SIGTSTP
static jmp_buf beforewait;

void tstp(int sig) {
	longjmp (beforewait, -1);
}
# endif /* SIGTSTP */

/* cmd */
void runset(char *dest) {
	arg = (char **) MemAlloc (QUANT * sizeof (char *));
	arglen = QUANT;
	arg [0] = dest;
	argp = 1;
}

/* cmd */
void runarg(char *name) {
	MemCheckIndex (arg, char **, arglen, QUANT, argp);
	arg [argp++] = name;
}

/* local */
void sigdfl () {
	signal (SIGTERM, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
	signal (SIGINT, SIG_DFL);
}

/* local */
int run(char *name, char **a0, int (*exe)(char *, char **, char **), int silent) {
	register t;
	int status;

	if ((t = FORK ()) == -1)
		/* cannot fork */
		return (0x7e00);
	if (t == 0) {
		/* child */
		if (silent) {
			close (0);
			close (1);
			close (2);
		}
		sigdfl();
		(*exe) (name, a0, EnvVector);
		if (! silent)
			outerr("%s: Command not found.\n", name);
		_exit (0x7f);                   /* file not found */
	}
# ifdef SIGTSTP
	if (setjmp (beforewait)) {
		outerr ("\n[%d]", t);
		for (; *a0; ++a0) {
			write (2, " ", 1);
			write (2, *a0, strlen (*a0));
		}
		write (2, " &\n", 3);
		signal (SIGTSTP, SIG_IGN);
		return (0);
	}
	signal(SIGTSTP, tstp);
# endif
	/* parent */
	while (t != wait (&status));
# ifdef SIGTSTP
	signal (SIGTSTP, SIG_IGN);
# endif
	return (status);
}

/* local */
char *execat(register char *s1, register char *s2, char *si) {
	register char *s;

	s = si;
	while (*s1 && *s1!=':')
		*s++ = *s1++;
	if (si != s)
		*s++ = '/';
	while (*s2)
		*s++ = *s2++;
	*s = 0;
	return (*s1 ? ++s1 : 0);
}

/* local */
int execvpe(char *name, char **argv, char **envstr) {
	static char *pathstr;
	char fname [128];
	char *newargs [256];
	int i;
	register char *cp;
	register unsigned etxtbsy = 1;
	register eacces = 0;

	if (! pathstr && ! (pathstr = EnvGet ("PATH")))
		pathstr = ":/bin:/usr/bin";
	for (cp=name; ; ++cp) {         /* check if name contains '/' */
		if (! *cp) {
			cp = pathstr;   /* if no, set cp to pathstr */
			break;
		}
		if (*cp == '/') {
			cp = "";        /* else path is empty */
			break;
		}
	}
	do {
		cp = execat (cp, name, fname);
retry:          (void) execve (fname, argv, envstr);
		switch (errno) {
		case ENOEXEC:
			newargs [0] = "sh";
			newargs [1] = fname;
			for (i=1; newargs [i+1] = argv [i]; ++i) {
				if (i >= 254) {
					errno = E2BIG;
					return (-1);
				}
			}
			(void) execve ("/bin/sh", newargs, envstr);
			return (-1);
		case ETXTBSY:
			if (++etxtbsy > 5)
				return (-1);
			(void) sleep (etxtbsy);
			goto retry;
		case EACCES:
			++eacces;
			break;
		case ENOMEM:
		case E2BIG:
			return (-1);
		}
	} while (cp);
	if (eacces)
		errno = EACCES;
	return (-1);
}

/* cmd */
int runv(int silent, char *name, char **a0) {
	return (run (name, a0, execvpe, silent));
}

/* cmd */
int rundone(char *name, char *a0) {
	register char **p, **q, *a;
	int ret;

	runarg (a0);
	for (p=arg, q=arg+argp-1; p<q; ++p, --q) {
		a = *p;
		*p = *q;
		*q = a;
	}
	runarg (NULL);
	ret = runv (1, name, arg) == 0;
	MemFree ((mem *) arg);
	return (ret);
}

/* cmd */
void runcancel(void) {
	MemFree ((mem *) arg);
}

# ifdef VARARGS
# ifdef sparc
#    include <vfork.h>
# endif
# include <stdarg.h>
# define MAXARGS 256
/* cmd */
int runl(char *fmt, ...) {
	va_list ap;
	char *name;
	char *args [MAXARGS];
	int argno = 0;
	int silent;

	va_start (ap, fmt);
	silent = va_arg (ap, int);
	name = va_arg (ap, char *);
	while (args [argno++] = va_arg (ap, char *))
		continue;
	va_end (ap);
	return (run (name, args, execve, silent));
}

# else

/* VARARGS 2 */
/* cmd */
int runl(int silent, char *name, char *a0) {
	return (run(name, &a0, execve, silent));
}

# endif
