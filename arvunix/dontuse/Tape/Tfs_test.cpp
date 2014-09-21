#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "dirent.h"
#include "Tfs.h"

char	*defdir = "/";

char *attrtochar(mode_t m) {
static char	attr[11];
char		*p = attr;

	*p++ = m & S_IFDIR ? 'd' : '-';
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
	return attr;
};

int main(int argc, char *argv[]) {
DIR		*dirp;
dirent		*dp;
FileSystem	*fs;
char		*pd;
struct stat	st;

	if (argc < 2)
		return 0;
	fs = new TDRFileSystem(argv[1]);

	if (argc > 2)
		pd = argv[2];
	else
		pd = defdir;

	if (fs->chdir(pd))
		fprintf(stderr, "Error chdir '%s'.\n", pd);

	dirp = fs->opendir(".");

	if (dirp == NULL) {
		fprintf(stderr, "Error open '%s'.\n", pd);
		delete fs;
		return 1;
	}

	while((dp = fs->readdir(dirp)) != NULL) {
		if (!fs->stat(dp->d_name, &st)) {
			fprintf(stdout, "%s\t", attrtochar(st.st_mode));
			fprintf(stdout, "%8u\t", st.st_size);
		}
		fprintf(stdout, "%s\n", dp->d_name);
	}
	fs->closedir(dirp);
	delete fs;

	return 0;
};
