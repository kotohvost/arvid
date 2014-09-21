/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "Tfs.h"

# define MAXPATHLEN     1024    /* max length of directory name */
# define PATSZ          20      /* max length of pattern */

# define SORTNAME       1       /* sort by name */
# define SORTEXT        2       /* sort by extension */
# define SORTTIME       3       /* sort by time */
# define SORTSIZE       4       /* sort by size */
# define SORTSKIP       5       /* unsorted */

# ifdef SHORTNAMES
#    define NAMESZ 15
# else
#    define NAMESZ 39
# endif

struct file {
	char name [NAMESZ];     /* file name */
	unsigned execable:1;    /* file is executable for me */
	unsigned tag:1;         /* file tagged */
	unsigned link:1;        /* symbolic link */
	unsigned long size;     /* file size */
	unsigned short mode;    /* access mode */
	unsigned short uid;     /* user ident */
	unsigned short gid;     /* group ident */
	unsigned short nlink;   /* number of file links */
	short dev;              /* device */
	short rdev;             /* special file parameters */
	unsigned long ino;      /* i-node */
	unsigned long mtime;    /* modification time */
	unsigned long atime;    /* access time */
	unsigned long ctime;    /* status change time */
};

struct my_dir {
	char cwd [MAXPATHLEN];  /* directory full name */
	char *shortcwd;         /* short printable directory name */
	short dev;              /* device */
	unsigned long ino;      /* i-node */
	struct file *cat;       /* catalog */
	int catsz;              /* length of catalog */
	int num;                /* number of files in cat */
	int topfile;            /* top file in the window */
	int curfile;            /* current file */
	char full;              /* full format */
	char status;            /* is it status window */
	char sort;              /* sort kind */
	char typesort;          /* sort by file types */
	char revsort;           /* reverse sorting */
	char alignext;          /* align extensions */
	int basecol;            /* base of window */
	int nfiles;             /* number of regular files */
	long nbytes;            /* number of bytes used */
	int mfiles;             /* number of matched files */
	long mbytes;            /* number of bytes matched */
	int tfiles;             /* number of tagged files */
	long tbytes;            /* number of bytes tagged */
	char pattern [PATSZ];   /* pattern for file names */
};

class dir {
	FileSystem	*fs;	/* current file system */
public:
	my_dir		d;

	dir() {
		fs = new UFS;
		d.sort = SORTEXT;
		d.typesort = 1;
		d.alignext = 1;
		d.catsz = 0;
		d.cat = 0;
		d.tfiles = 0;
		d.tbytes = 0;
		d.status = 0;
		*d.pattern = 0;
	}
	dir(char *fn) {
		fs = new TDRFS(fn);
		d.sort = SORTEXT;
		d.typesort = 1;
		d.alignext = 1;
		d.catsz = 0;
		d.cat = 0;
		d.tfiles = 0;
		d.tbytes = 0;
		d.status = 0;
		*d.pattern = 0;
	}
	~dir() {
		delete fs;
	}
int	chdir(char *p) {	/* chdir func */
		return fs->chdir(p); }
int	stat(char *path, struct stat *sb) {
		return fs->stat(path, sb); }
int	lstat(char *path, struct stat *sb) {
		return fs->lstat(path, sb); }
int	fstat(int fd, struct stat *sb) {
		return fs->fstat(fd, sb); }
DIR	*opendir(char *filename) {
		return fs->opendir(filename); }
dirent	*readdir(DIR *dirp) {
		return fs->readdir(dirp); }
int	closedir(DIR *dirp) {
		return fs->closedir(dirp); }
int	access(char *path, int mode) {
		return fs->access(path, mode); }
};

void counttag(register dir *d);
int setdir(register dir *d, char *dirname);
