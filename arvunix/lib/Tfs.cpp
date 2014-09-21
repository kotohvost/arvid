#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "dirent.h"
#include "Tfs.h"
#include "msdos_conv.h"

TDRFS::TDRFS(char *fn) {
int		len, fdin;
TDR_HEAD	head;
TDR_ADD_HEAD	addhead;

	len = strlen(fn);
	filename = new char[len+1];
	strncpy(filename, fn, len+1);
	files = dirs = descs = NULL;
	sizedesc = 0;
	tdrfmt = 0;
	cdir_name = NULL;
	cdir = NULL;
//	dir_name = NULL;
//	dir_ptr = NULL;
//	dir_ent = NULL;

	fdin = ::open(filename, O_RDONLY, 0644);
	if (fdin < 0) {
		fprintf(stderr, "Error open '%s'.\n", fn);
		return;
	}
	if (read(fdin, &head, sizeof(TDR_HEAD)) != sizeof(TDR_HEAD)) {
		fprintf(stderr, "Error read '%s'.\n", fn);
		close(fdin);
		return;
	}
	sizef = head.sizef;
	sized = head.sized;
	format = head.format;
	ID = head.ID;
	length = head.length;
	savelen = head.savelen;
	lastsect = nextsect = head.nextsect;
#if 0
	fprintf(stdout, "\tfiles\tdirs\tPT\n");
	fprintf(stdout, "start\t%08X\t%08X\t%08x\n",
		head.startf, head.startd, head.startpt);
	fprintf(stdout, "size\t%u\t%u\t%u\n",
		sizef, sized, head.sizept);
	fprintf(stdout, "format\t%04X\nID\t%04X\nlength\t%u\nsavelen\t%u\n",
		format, ID, length, savelen);
	fprintf(stdout, "nextsect\t%u\n", nextsect);
#endif
	if (head.startf == sizeof(TDR_HEAD) ||
		head.startd == sizeof(TDR_HEAD) ||
		head.startpt == sizeof(TDR_HEAD)) {
		fprintf(stdout, "This is OLD TDR\n");
		tdrfmt = OLDTDR;
	} else if (read(fdin, &addhead, sizeof(TDR_ADD_HEAD)) != sizeof(TDR_ADD_HEAD)) {
		fprintf(stderr, "Error read '%s'.\n", fn);
		close(fdin);
		return;
	}

	if (lseek(fdin, head.startf, SEEK_SET) != head.startf) {
		fprintf(stderr, "Error seek file '%s'.\n", fn);
		close(fdin);
		return;
	}
	files = malloc(sizef);
	if (read(fdin, files, sizef) != sizef) {
		fprintf(stderr, "Error read '%s'.\n", fn);
		free(files);
		close(fdin);
		return;
	}

	if (lseek(fdin, head.startd, SEEK_SET) != head.startd) {
		fprintf(stderr, "Error seek file '%s'.\n", fn);
		free(files);
		close(fdin);
		return;
	}
	dirs = malloc(sized);
	if (read(fdin, dirs, sized) != sized) {
		fprintf(stderr, "Error read '%s'.\n", fn);
		free(dirs);
		free(files);
		close(fdin);
		return;
	}

	/* load PT */

	if (tdrfmt == 0) {
		sizedesc = addhead.sizedes;
		lastsect = addhead.lastsect;
//		fprintf(stdout, "lastsect\t%d\n", lastsect);
		if (lseek(fdin, addhead.startdes, SEEK_SET) != addhead.startdes) {
			fprintf(stderr, "Error seek file '%s'.\n", fn);
			free(dirs);
			free(files);
			close(fdin);
			return;
		}
		descs = malloc(sizedesc);
		if (read(fdin, descs, sizedesc) != sizedesc) {
			fprintf(stderr, "Error read '%s'.\n", fn);
			free(descs);
			free(dirs);
			free(files);
			close(fdin);
			return;
		}
	}
	close(fdin);
	cdir = dirs;
	cdir_name = "/";
};

TDRFS::~TDRFS() {
	delete filename;
	if (files)
		free(files);
	if (dirs)
		free(dirs);
	if (descs)
		free(descs);
//	if (dir_name)
//		delete dir_name;
//	if (dir_ptr)
//		delete dir_ptr;
//	if (dir_ent)
//		delete dir_ent;
};

DIR *TDRFS::opendir(char *fn) {
int	len;
void	*d;		// pointer of *dirs
DIR	*dir_ptr;

//	if (dir_name)
//		return NULL;
//	if (dir_ptr)
//		return NULL;
	if (!fn)
		return NULL;
//	fprintf(stdout, "opendir: %s\n", fn);

	if (*fn == '/')
		d = finddir(dirs, fn);
	else
		d = finddir(cdir, fn);
	if (d == NULL)
		return NULL;

	len = strlen(fn);
//	dir_name = new char[len+1];
//	strncpy(dir_name, fn, len+1);
	dir_ptr = new DIR;
	dir_ptr->dd_fd = 0;
	dir_ptr->dd_loc = 0;
	dir_ptr->dd_size = sizeof(dirent);
	dir_ptr->dd_len = (getdirentries(d) + \
			getfilentries(d)) * sizeof(dirent);
//	fprintf(stdout, "%d, %d\n",
//		dir_ptr->dd_len, dir_ptr->dd_len / sizeof(dirent));
	if (dir_ptr->dd_len)
		dir_ptr->dd_buf = (char *)malloc(dir_ptr->dd_len);
	else
		dir_ptr->dd_buf = NULL;
	if (dir_ptr->dd_len && dir_ptr->dd_buf)
		getentries(d, dir_ptr->dd_buf, dir_ptr->dd_len);

	return dir_ptr;
};

dirent *TDRFS::readdir(DIR *dirp) {
dirent	*pent;

//	if (dir_ptr != dirp)
//		return NULL;
	if (dirp->dd_loc >= dirp->dd_len)
		return NULL;
	pent = (dirent *)(dirp->dd_buf + dirp->dd_loc);
//	pent += dir_ptr->dd_loc;
	dirp->dd_loc += sizeof(dirent);
	return pent;
};

long TDRFS::telldir(DIR *dirp) {
	return NULL;
};

void TDRFS::seekdir(DIR *dirp, long loc) {
};

void TDRFS::rewinddir(DIR *dirp) {
};

int TDRFS::closedir(DIR *dirp) {
//	if (dir_ptr != dirp)
//		return -1;
//	if (dir_name) {
//		delete dir_name;
//		dir_name = NULL;
//	}
	if (dirp) {
		if (dirp->dd_buf)
			free(dirp->dd_buf);
		delete dirp;
	}
//	dir_ptr = NULL;

	return NULL;
};
//int	TDRFS::dirfd(DIR *dirp) {};

/*
 * Convert name to TDR name, ext.
 */
void TDRFS::nametotdr(char *fn, char *dn, char *de) {
int	len;
int	name, ext;

//	fprintf(stdout, "nametotdr: %s\n", fn);

	/* clear */
	memset(dn, ' ', 8);
	dn[8] = '\0';
	memset(de, ' ', 3);
	de[3] = '\0';
	for(len = 0; fn[len] != '/' && fn[len] != '\0'; len++);
	if (len) {
		for(name = 0; name < len && fn[name] != '.' && name < 8; name++)
			dn[name] = fn[name];
		if (fn[name] == '.')
			name++;
		for(ext = 0; name+ext < len && ext < 3; ext++)
			de[ext] = fn[name+ext];

//		fprintf(stdout, "%s.%s\n", dn, de);
	}
}

/*
 * recursive function.
 */
void *TDRFS::finddir(void *d, char *fn) {
TDR_DIR		*pdir;
int		len, notfound;
unsigned short	level;
char		dn[8+1], de[3+1], name, ext;

//	fprintf(stdout, "finddir: %s\n", fn);

	pdir = (TDR_DIR *)d;

	if (!*fn)
		return pdir;

	while (*fn == '/')
		fn++;
	if (!*fn)
		return pdir;
	/* . */
	if (fn[0] == '.' && (fn[1] == '\0' || fn[1] == '/'))
		return (finddir(pdir, fn + 1));
	/* .. */
	if (fn[0] == '.' && fn[1] == '.' && (fn[2]=='\0' || fn[2]=='/')) {
		level = pdir->level;
//		pdir--;
		while (level <= pdir->level && pdir > dirs)
			pdir--;
		return (finddir(pdir, fn + 2));
	}

	for(len = 0; fn[len] != '/' && fn[len] != '\0'; len++);

//	fprintf(stdout, "%s\n", fn);

	/* find */
	if (len) {
		memset(dn, ' ', sizeof(dn));
		memset(de, ' ', sizeof(de));
		dn[8] = '\0';
		de[3] = '\0';
		for(name = 0; name < len && fn[name] != '.' && name < 8; name++)
			dn[name] = fn[name];
		if (fn[name] == '.')
			name++;
		for(ext = 0; name+ext < len && ext < 3; ext++)
			de[ext] = fn[name+ext];

//		fprintf(stdout, "%s.%s\n", dn, de);

		level = pdir->level;
		pdir++;
		notfound = 1;
		while(level < pdir->level && notfound) {
			if (level+1 == pdir->level &&
			!strncasecmp(pdir->name, dn, 8) &&
			!strncasecmp(pdir->ext, de, 3))
				notfound = 0;
			else
				pdir++;
		}
		if (notfound)
			return NULL;

		/* recursive */
		if (fn[len] != '\0')
			pdir = (TDR_DIR *)finddir(pdir, fn + len);
	}
	return pdir;
};

/*
 * TODO:
 *	check region dirs.
 */
int TDRFS::getdirentries(void *ptr) {
TDR_DIR		*pdir;
int		rc;
unsigned short	level;

	rc = 2;
//	fprintf(stdout, "%d\n", sizeof(TDR_DIR));
	pdir = (TDR_DIR *)ptr;
	level = pdir->level;
//	fprintf(stdout, "level\tstartf\tlastf\tnumf\n");
//	fprintf(stdout, "%d\t%d\t%d\t%d\n",
//		pdir->level, pdir->startf, pdir->lastf, pdir->numf);
	pdir++;
	while(level < pdir->level) {
		if (level+1 == pdir->level) {
//		fprintf(stdout, "%d\t%d\t%d\t%d\n",
//			pdir->level, pdir->startf, pdir->lastf, pdir->numf);
			rc++;
		}
		pdir++;
	}
	return rc;
};

/*
 * fill dirent.
 * TODO:
 *	check region dirs and files.
 */
void TDRFS::getentries(void *d, char *ptr, int len) {
dirent		*pent;
TDR_DIR		*pdir;
TDR_FILE	*pfile;
int		num, fileno;
char		*p;
unsigned short	level;

	num = 0;
	pent = (dirent *)ptr;
	fileno = 0;

	/* fill dirs */
	pdir = (TDR_DIR *)d;
	level = pdir->level;
	/* . */
		pent->d_fileno = (u_long *)pdir->name;
		pent->d_reclen = sizeof(dirent);
		pent->d_type = DT_DIR;
		strcpy(pent->d_name, ".");
		pent->d_namlen = 1;
		pent++;
		num += sizeof(dirent);
	/* .. */
		pent->d_fileno = (u_long *)findfile(pdir, "..");
		pent->d_reclen = sizeof(dirent);
		pent->d_type = DT_DIR;
		strcpy(pent->d_name, "..");
		pent->d_namlen = 2;
		pent++;
		num += sizeof(dirent);
	pdir++;
	while(level < pdir->level && num < len) {
		if (level+1 == pdir->level) {
			pent->d_fileno = (u_long *)pdir->name;
			pent->d_reclen = sizeof(dirent);
			pent->d_type = DT_DIR;
			bzero(pent->d_name, 256);
			strncpy(pent->d_name, pdir->name, 8);
			p = pent->d_name + 8;
			while(p > pent->d_name && (*p == ' ' || *p == '\0'))
				*p-- = '\0';
			*++p = '.';
			*++p = '\0';
			strncat(pent->d_name, pdir->ext, 3);
			p = pent->d_name + strlen(pent->d_name);
			while(p > pent->d_name && (*p == ' ' || *p == '\0' || *p == '.'))
				*p-- = '\0';
			pent->d_namlen = strlen(pent->d_name);
			pent++;
			num += sizeof(dirent);
		}
		pdir++;
	}

	/* fill files */
	pfile = (TDR_FILE *)files;
	pdir = (TDR_DIR *)d;
	if (pdir->numf) {
		level = pdir->startf;
		pfile += pdir->startf;
		while (num < len && level <= pdir->lastf) {
			pent->d_fileno = pfile;
			pent->d_reclen = sizeof(dirent);
			pent->d_type = DT_REG;
			bzero(pent->d_name, 256);
			strncpy(pent->d_name, pfile->name, 8);
			p = pent->d_name + 8;
			while(p > pent->d_name && (*p == ' ' || *p == '\0'))
				*p-- = '\0';
			*++p = '.';
			*++p = '\0';
			strncat(pent->d_name, pfile->ext, 3);
			p = pent->d_name + strlen(pent->d_name);
			while(p > pent->d_name && (*p == ' ' || *p == '\0' || *p == '.'))
				*p-- = '\0';
			pent->d_namlen = strlen(pent->d_name);
			pent++;
			num += sizeof(dirent);
			pfile++;
			level++;
		}
	}
};

int TDRFS::chdir(char *name) {
TDR_DIR	*pdir;

	if (!*name)
		return -1;
	if (*name == '/')
		pdir = (TDR_DIR *)finddir(dirs, name);
	else
		pdir = (TDR_DIR *)finddir(cdir, name);
	if (pdir == NULL)
		return -1;
	cdir = pdir;
	return 0;
};

/*
 * '/vvv/bbb/bbb/'
 * 'nnn/mmm'
 * '/'
 * '.' './'
 */
void *TDRFS::findfile(void *d, char *cfn) {
TDR_DIR		*pdir, *apdir;
TDR_FILE	*pfile;
char		*nfn, *dfn, *fn;
int		sfn, entr, notfound;
char		name[8+1], ext[3+1];

	/* no name - no found */
	if (!*cfn)
		return NULL;

	pdir = (TDR_DIR *)d;

	/* find dirs in path */
	sfn = strlen(cfn);
	fn = new char[sfn+1];
	strcpy(fn, cfn);
	nfn = fn + sfn;

	/* delete right slash */
	while((*nfn=='\0' || *nfn=='/') && nfn != fn && sfn != 0) {
		*nfn = '\0'; nfn--; sfn--;
	}
	/* '//////' -> '' || '/' */
	if (*nfn == '/')
		return (TDR_FILE *)pdir->name;

	/* find dir, file */
	while(*nfn != '/' && nfn != fn && sfn != 0) {
		nfn--; sfn--;
	}
	/* fn = '/ccc/ggg/', nfn = 'ddd' */
	/* fn = '/vvv', nfn = '/vvv' */

	/* find in dirs */
	if (sfn) {
		dfn = new char[sfn+1];
		strncpy(dfn, fn, sfn);
		dfn[sfn] = 0;
		pdir = (TDR_DIR *)finddir(pdir, dfn);
		delete dfn;
		if (pdir == NULL) {
			delete fn;
			return NULL;
		}
	}

	/* find in dirs */
	apdir = (TDR_DIR *)finddir(pdir, nfn);
	if (apdir) {
		delete fn;
		return ((TDR_FILE *)apdir->name);
	} else {
	/* find in files */
		pfile = (TDR_FILE *)files;
		nametotdr(nfn, name, ext);
		pfile += pdir->startf;
		entr = getfilentries(pdir);
		notfound = 1;
		while (entr-- > 0 && notfound) {
			if (!strncasecmp(pfile->name, name, 8) &&
			!strncasecmp(pfile->ext, ext, 3))
				notfound = 0;
			else
				pfile++;
		}
	}
	delete fn;
	if (!notfound)
		return pfile;
	return NULL;
};

int TDRFS::stat(char *path, struct stat *st) {
TDR_FILE	*pfile;

	if (*path == '/')
		pfile = (TDR_FILE *)findfile(dirs, path);
	else
		pfile = (TDR_FILE *)findfile(cdir, path);

	if (pfile == NULL)
		return -1;

#define S_DOS_READ	0x01
#define S_DOS_HYD	0x02
#define S_DOS_SYS	0x04
#define S_DOS_VOL	0x08
#define S_DOS_DIR	0x10
#define S_DOS_ARC	0x20
	st->st_dev = 0;
	st->st_ino = pfile;
	st->st_mode = (pfile->attr & S_DOS_DIR) ? (S_IFDIR | S_IEXEC | S_IEXEC>>3 | S_IEXEC>>6) : S_IFREG;
	st->st_mode |= (pfile->attr & S_DOS_READ) ? (S_IREAD | S_IREAD>>3) : (S_IREAD | S_IREAD>>3 | S_IREAD>>6);
//	st->st_mode |= (pfile->attr & S_DOS_HYD) ? S_IREAD : ((S_IREAD | S_IWRITE)<< 3);
//	st->st_mode |= (pfile->attr & S_DOS_SYS) ? 0 : (S_IREAD << 6);
	st->st_nlink = 1;
	st->st_uid = 0;
	st->st_gid = 0;
	st->st_rdev = 0;
	st->st_atime = st->st_mtime = st->st_ctime = dos2unixtime(pfile->dd, pfile->dt);
	st->st_size = pfile->size;
	return 0;
};

int TDRFS::lstat(char *path, struct stat *sb) {
	return stat(path, sb);
};

int TDRFS::fstat(int fd, struct stat *sb) {
	return -1;
};

int TDRFS::access(char *path, int mode) {
int	rc;

	rc = 0;
	if (*path != '/') {
		if (findfile(cdir, path) == NULL)
			rc = -1;
	} else {
		if (findfile(dirs, path) == NULL)
			rc = -1;
	}
	return rc;
};
