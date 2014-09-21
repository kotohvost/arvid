/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "sys/types.h"

#    include <dirent.h>
#    define direct dirent
  
# include "dir.h"
# include "file.h"
# include "deco.h"
# include "mem.h"
# include "main.h"
# include "match.h"
#include "choice.h"
#include "ut.h"

# define QUANT 128

# ifndef MAXNAMLEN
#    define MAXNAMLEN   255
# endif

extern uid, gid;

my_dir *compdir;

/* local */
void chdirup(dir *d, register char *name) {
	register char *s, *p;

	for (;;) {
		if (d->chdir(name) < 0)
			error ("Cannot chdir to %s", name);
		else if (d->access(".", 4) < 0)
			error ("Cannot read %s", name);
		else
			break;
		if (name[0]=='/' && name[1]==0) {       /* root directory */
			error ("DECO cannot work on this system -- goodbye!");
			quitdeco ();
		}
		/* cut last directory name */
		for (p=s=name; *s; ++s)
			if (*s == '/')
				p = s;
		*p = 0;
		if (! *name) {
			name [0] = '/';
			name [1] = 0;
		}
	}
}

/* local */
char *prepend(char *p, char *str) {
	int len = strlen (str);
	MemCopy (p-len, str, len);
	return (p-len);
}

/*
 * Get current directory name.  Pathname should be at least
 * MAXPATHLEN long.  Returns 0 on error, and puts message into pathname.
 * Compatible with BSD getwd().
 */

/* local */
char *getcwdname(dir *d, char *pathname) {
	char upname [MAXPATHLEN+MAXNAMLEN];
	char myname [MAXPATHLEN];
	char *upptr, *myptr;
	struct stat root, up, cur, f;
	register struct direct *rec;
	DIR *dir;
	int len;

	myptr = &myname[MAXPATHLEN-1];
	upptr = &upname[MAXPATHLEN-1];
	*myptr = *upptr = 0;
	upptr = prepend (upptr, "./");
	d->stat("/", &root);
	d->stat(upptr, &cur);
	while (cur.st_dev != root.st_dev || cur.st_ino != root.st_ino) {
		upptr = prepend (upptr, "../");
		dir = d->opendir(upptr);
		if (! dir) {
			strcpy (pathname, "Cannot read `..'");
			return (0);
		}
//		d->fstat(dirfd (dir), &up);
		d->stat(upptr, &up);
		for (;;) {
			if (! (rec = d->readdir(dir))) {
				strcpy (pathname, "Cannot find path");
				return (0);
			}
			if (cur.st_dev == up.st_dev) {
				if (cur.st_ino == rec->d_ino)
					break;
				continue;
			}
			if (rec->d_name[0]=='.' && (!rec->d_name[1] ||
			    rec->d_name[1]=='.' && rec->d_name[2]==0))
				continue;
			strcpy (&upname[MAXPATHLEN-1], rec->d_name);
			d->lstat(upptr, &f);
			upname[MAXPATHLEN-1] = 0;
			if (cur.st_ino == f.st_ino && cur.st_dev == f.st_dev)
				break;
		}
		myptr = prepend (myptr, rec->d_name);
		*--myptr = '/';
		d->closedir(dir);
		cur = up;
	}
	strcpy (pathname, *myptr ? myptr : "/");
	return (pathname);
}

/* local */
void retag(my_dir *d, my_dir *od, char pagelen) {
	register struct file *f, *of;
	struct file *end, *oend, *ocur, *otop;
	int cmp;

	end = d->cat + d->num;
	oend = od->cat + od->num;
	ocur = od->cat + od->curfile;
	otop = od->cat + od->topfile;
	d->curfile = -1;
	compdir = d;
	for (f=d->cat, of=od->cat; of<oend; ++of) {
		while ((cmp = compfile (f, of)) < 0)
			if (++f >= end)
				goto breakloop;
		if (of == ocur)
			d->curfile = f - d->cat;
		if (of == otop)
			d->topfile = f - d->cat;
		if (cmp == 0)
			f->tag = of->tag;
	}
breakloop:
	if (d->curfile < 0) {
		d->curfile = d->num - 1;
		d->topfile = d->curfile - pagelen + 1;
	} else if (d->topfile > d->curfile || d->topfile + pagelen <= d->curfile)
		d->topfile = d->curfile - pagelen + 1;
	while (d->topfile + pagelen  - H > d->curfile)
		d->topfile -= H;
	if (d->topfile < 0)
		d->topfile = 0;
}

/* cmd, main */
void counttag(register dir *d) {
	register struct file *f;

	/* compute number of files and bytes */
	d->d.tfiles = 0;
	d->d.tbytes = 0;
	for (f=d->d.cat; f<d->d.cat+d->d.num; ++f)
		if ((f->mode & S_IFMT) == (unsigned) S_IFREG && f->tag) {
			++d->d.tfiles;
			d->d.tbytes += f->size;
		}
}

/*
 *      Set up new directory.
 *      struct dir *d   - directory structure
 *      char *dirname   - name of directory.
 *                      If 0 then use d->d.cwd.
 */

/* cmd, com, main */
int setdir(register dir *d, char *dirname) {
	DIR *f;
	struct direct *recp;
	register struct file *p;
	my_dir olddir;
	struct stat st;

	/* change directory */
	if (dirname)
		if (d->chdir(dirname) < 0)
			error ("Cannot chdir to %s", dirname);
		else
			getcwdname (d, d->d.cwd);
	chdirup(d, d->d.cwd);
	d->d.shortcwd = strtail (d->d.cwd, '/', 35);
	d->stat(d->d.cwd, &st);
	d->d.dev = st.st_dev;
	d->d.ino = st.st_ino;

	f = d->opendir(".");
	if (! f) {
		error ("Cannot read '.' -- good bye !");
		quitdeco ();
	}

	if (! dirname) {
		/* save directory structure, then use it for retagging */
		olddir = d->d;
		d->d.catsz = 0;
		d->d.cat = 0;
	}

	if (! d->d.cat) {
		d->d.catsz = QUANT;
		d->d.cat = (struct file *) MemAlloc ((int) (d->d.catsz * sizeof (struct file)));
	}

	d->d.num = 0;
	d->d.nfiles = 0;
	d->d.nbytes = 0;
	d->d.mfiles = 0;
	d->d.mbytes = 0;
	while (recp = d->readdir(f)) {
		if (! recp->d_ino)
			continue;
		MemCheckIndex (d->d.cat, struct file *, d->d.catsz, QUANT, d->d.num);
		p = &d->d.cat[d->d.num];
		d->lstat(recp->d_name, &st);
# ifdef S_IFLNK
		p->link = ((st.st_mode & S_IFMT) == (unsigned) S_IFLNK);
		if (p->link)
			d->stat(recp->d_name, &st);
# else
		p->link = 0;
# endif
		/* skip .. in root directory */
		if (recp->d_name[0] == '.' && recp->d_name[1] == '.' &&
		    recp->d_name[2] == 0 && st.st_ino == d->d.ino &&
		    st.st_dev == d->d.dev)
			continue;

		/* skip all hidden files except parent dir entry */
		if (! showhidden && recp->d_name[0] == '.' &&
		    (recp->d_name[1] != '.' || recp->d_name[2]))
			continue;

		/* skip names not matched with pattern */
		if ((st.st_mode & S_IFMT) == (unsigned) S_IFREG) {
			if (! showhidden && recp->d_name[0] == '.')
				continue;
			++d->d.nfiles;
			d->d.nbytes += st.st_size;
			if (d->d.pattern [0] && ! match (recp->d_name, d->d.pattern))
				continue;
			++d->d.mfiles;
			d->d.mbytes += st.st_size;
		}
		p->mode  = st.st_mode;
		p->mtime = st.st_mtime;
		p->atime = st.st_atime;
		p->ctime = st.st_ctime;
		p->size  = st.st_size;
		p->nlink = st.st_nlink;
		p->dev   = st.st_dev;
		p->rdev  = st.st_rdev;
		p->ino   = st.st_ino;
		p->uid   = st.st_uid;
		p->gid   = st.st_gid;
		strncpy (p->name, recp->d_name, sizeof(p->name)-1);
		p->name [sizeof(p->name)-1] = 0;
		if (uid == 0)
			p->execable = ((p->mode & 0111) != 0);
		else if (uid == p->uid)
			p->execable = p->mode >> 6 & 1;
		else if (gid == p->gid)
			p->execable = p->mode >> 3 & 1;
		else
			p->execable = p->mode & 1;
		p->tag = 0;
		++d->d.num;
	}

	d->d.topfile = d->d.curfile = 0;

	compdir = &d->d;
	qsort ((char *) d->d.cat, (unsigned) d->d.num, sizeof (d->d.cat[0]), compfile);

	if (! dirname) {
		/* retag files, restore curfile */
		retag (&d->d, &olddir, PAGELEN(d));
		MemFree ((mem *) olddir.cat);
		counttag (d);
	}
	d->closedir(f);
	return (1);
}

/* unused */
#if 0
long modtime (name)
char *name;
{
	struct stat st;

	if (stat (name, &st) < 0)
		return (0);
	return (st.st_mtime);
}
#endif
