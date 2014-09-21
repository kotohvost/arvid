/*
 * Виртуальный класс для работы с файлами и директориями.
 */
#include <sys/stat.h>
#include "tdr.h"

class FileSystem {
public:
virtual int	access(char *path, int mode) = 0;
virtual DIR	*opendir(char *fn) = 0;
virtual dirent	*readdir(DIR *dirp) = 0;
virtual long	telldir(DIR *dirp) = 0;
virtual void	seekdir(DIR *dirp, long loc) = 0;
virtual void	rewinddir(DIR *dirp) = 0;
virtual int	closedir(DIR *dirp) = 0;
//virtual int	dirfd(DIR *dirp) = 0;
virtual int	chdir(char *name) = 0;

virtual int	stat(char *path, struct stat *sb) = 0;
virtual int	lstat(char *path, struct stat *sb) = 0;
virtual int	fstat(int fd, struct stat *sb) = 0;
};

/*
 * UnixFileSystem
 */
class UFS : public FileSystem {
public:
int	access(char *path, int mode) {
		return ::access(path, mode); }
DIR	*opendir(char *fn) {
		return ::opendir(fn); };
dirent	*readdir(DIR *dirp) {
		return ::readdir(dirp); };
long	telldir(DIR *dirp) {
		return ::telldir(dirp); };
void	seekdir(DIR *dirp, long loc) {
		::seekdir(dirp, loc); };
void	rewinddir(DIR *dirp) {
		::rewinddir(dirp); };
int	closedir(DIR *dirp) {
		return ::closedir(dirp); };
//virtual int	dirfd(DIR *dirp) = 0;
int	chdir(char *name) {
		return ::chdir(name); };

int	stat(char *path, struct stat *sb) {
		return ::stat(path, sb); };
int	lstat(char *path, struct stat *sb) {
		return ::lstat(path, sb); };
int	fstat(int fd, struct stat *sb) {
		return ::fstat(fd, sb); }
};

/*
 * Класс для работы с лентами TDR.
 */
class TDRFS : public FileSystem {
	char	*filename;	// имя файла

	unsigned short	format;
	unsigned short	ID;
	unsigned short	length;
	unsigned short	savelen;
	unsigned int	nextsect;
	unsigned int	lastsect;

	int		tdrfmt;
#define	OLDTDR	1

	void	*files;		//
	int	sizef;
	void	*dirs;		//
	int	sized;
	void	*descs;		//
	int	sizedesc;

	char	*cdir_name;	// current dir name
	void	*cdir;		// current dir
//	char	*dir_name;	// opendir()
//	DIR	*dir_ptr;	// readdir()
//	dirent	*dir_ent;	// closedir()

void	nametotdr(char *fh, char *dn, char *de);
void	*finddir(void *dir, char *fn);	// get pointer of dir
void	*findfile(void *dir, char *fn);	// get pointer of file
int	getdirentries(void *dir_ptr);	// get num dirs in dir
int	getfilentries(void *dir_ptr) {	// get num files in dir
		return (((TDR_DIR *)dir_ptr)->numf);
		};
void	getentries(void *dirs, char *, int len);	// fill dirent struct

public:
	TDRFS(char *fn);
	~TDRFS();

int	access(char *path, int mode);
DIR	*opendir(char *fn);
dirent	*readdir(DIR *dirp);
long	telldir(DIR *dirp);
void	seekdir(DIR *dirp, long loc);
void	rewinddir(DIR *dirp);
int	closedir(DIR *dirp);
//int	dirfd(DIR *dirp);
int	chdir(char *name);

int	stat(char *path, struct stat *sb);
int	lstat(char *path, struct stat *sb);
int	fstat(int fd, struct stat *sb);
};
