/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include "stdio.h"
# include "string.h"
# include <sys/types.h>
# include <sys/stat.h>

# include "dir.h"
# include "file.h"
# include "ut.h"

extern struct my_dir *compdir;

/*
 *      Return integer number, used for comparing file types.
 */

/* local */
int comptype(register struct file *f) {
	switch (f->mode & S_IFMT) {
	case S_IFDIR:
		if (f->name[0] == '.') {
			if (! f->name[1])
				return (-1);
			if (f->name[1] == '.' && ! f->name[2])
				return (-2);
		}
		if (! f->execable)
			return (1);
		return (0);
	case S_IFCHR:
		return (100);
	case S_IFBLK:
		return (110);
# ifdef S_IFIFO
	case S_IFIFO:
		return (120);
# endif
# ifdef S_IFNAM
	case S_IFNAM:
		return (130);
# endif
# ifdef S_IFSOCK
	case S_IFSOCK:
		return (140);
# endif
# ifdef S_IFLNK
	case S_IFLNK:
		return (150);
# endif
	case S_IFREG:
		return (500);
	default:
		return (900);
	}
}

/*
 *      Compare files. Used in call to qsort.
 *      Return -1, 0, 1 iff a is less, equal or greater than b.
 */

/* cmd, dir */
int compfile(register struct file *a, register struct file *b) {
	register atype, btype;
	int rez;

	if (compdir->revsort) {
		struct file *t;

		t = a;
		a = b;
		b = t;
	}
	if (compdir->typesort) {
		atype = comptype (a);
		btype = comptype (b);
		if (atype != btype)
			return (atype<btype ? -1 : 1);
	}
	switch (compdir->sort) {
	case SORTEXT:
		atype = a->name[0] != '.';
		btype = b->name[0] != '.';
		if (atype != btype)
			return (atype<btype ? -1 : 1);
		if (rez = strcmp (extension (a->name), extension (b->name)))
			return (rez);
	case SORTNAME:
		return (strcmp (a->name, b->name));
	case SORTTIME:
		return (a->mtime < b->mtime ? -1 :
			a->mtime > b->mtime ? 1 : 0);
	case SORTSIZE:
		return (a->size < b->size ? -1 :
			a->size > b->size ? 1 : 0);
	}
	return (0);
}

/* draw */
char *filesize(register struct file *f) {
static char buf [9];

	switch (f->mode & S_IFMT) {
	case S_IFDIR:
		return ("   <DIR>");
	case S_IFCHR:
		sprintf (buf, " %3d,%3d", f->rdev>>8&0377, f->rdev&0377);
		return (buf);
	case S_IFBLK:
		sprintf (buf, " %3d,%3d", f->rdev>>8&0377, f->rdev&0377);
		return (buf);
# ifdef S_IFIFO
	case S_IFIFO:
		return ("  <FIFO>");
# endif
# ifdef S_IFNAM
	case S_IFNAM:
		return ("  <NAME>");
# endif
# ifdef S_IFSOCK
	case S_IFSOCK:
		return ("  <SOCK>");
# endif
# ifdef S_IFLNK
	case S_IFLNK:
		return ("  <LINK>");
# endif
	case S_IFREG:
		sprintf (buf, "%8ld", (long) f->size);
		return (buf);
	default:
		return ("     ???");
	}
}

/* draw */
char *filemode(int m) {
static char buf [10];
register char *p = buf;

	*p++ = m & S_IREAD ? 'r' : '-';
	*p++ = m & S_IWRITE ? 'w' : '-';
	*p++ = m & S_IEXEC ?
		(m & S_ISUID ? 's' : 'x') :
		(m & S_ISUID ? 'S' : '-');
	*p++ = m & S_IREAD >> 3 ? 'r' : '-';
	*p++ = m & S_IWRITE >> 3 ? 'w' : '-';
	*p++ = m & S_IEXEC >> 3 ?
		(m & S_ISGID ? 's' : 'x') :
		(m & S_ISGID ? 'S' : '-');
	*p++ = m & S_IREAD >> 6 ? 'r' : '-';
	*p++ = m & S_IWRITE >> 6 ? 'w' : '-';
	*p++ = m & S_IEXEC >> 6 ?
		(m & S_ISVTX ? 't' : 'x') :
		(m & S_ISVTX ? 'T' : '-');
	*p = 0;
	return (buf);
}

/* draw */
int filetype(register struct file *f) {
	switch (f->mode & S_IFMT) {
	case S_IFDIR:
		if (! f->execable)
			return ('&');
		return ('/');
	case S_IFCHR:
		return ('$');
	case S_IFBLK:
		return ('#');
# ifdef S_IFIFO
	case S_IFIFO:
		return ('=');
# endif
# ifdef S_IFNAM
	case S_IFNAM:
		return ('@');
# endif
# ifdef S_IFSOCK
	case S_IFSOCK:
		return ('!');
# endif
# ifdef S_IFLNK
	case S_IFLNK:
		return ('~');
# endif
	case S_IFREG:
		if (f->execable)
			return ('*');
		return (' ');
	default:
		return ('?');
	}
}
