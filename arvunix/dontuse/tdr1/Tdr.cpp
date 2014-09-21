#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "sys/stat.h"
#include "Tdr.h"

void print_head(tdr_head head);
void print_headnew(tdr_head_new headnew);
void print_dirs(u_char *dir, u_int cd, u_char *file, u_int mf, u_char *des);
void store_dir(List *, u_char *file, u_int mf, u_int sf, u_int ef, u_int nf);
int LoadTDR(char *name);

main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file.tdr>\n", argv[0]);
		return(0);
	}
	LoadTDR(argv[1]);
};

int LoadTDR(char *name) {
int		fdin;		// file desk
struct stat	fdstat;		// stat file
tdr_head	head;		// head of TDR file
tdr_head_new	headnew;	// new head of TDR  file
u_char		*files, *dirs;	// alloc for data
u_char		*des;

	// пробуем открыть
	fdin = open(name, O_RDONLY, 0);
	if (fdin < 0) {
		fprintf(stderr, "Can't open file '%s'\n", name);
		return(0);
	}

	// статистика по файлу для проверок
	if (fstat(fdin, &fdstat) < 0) {
		fprintf(stderr, "Can't stat file '%s'\n", name);
		close(fdin);
		return(0);
	}

	// загрузим заголовок
	if (read(fdin, &head, sizeof(tdr_head)) != 36) {
		fprintf(stderr, "Error read HEAD from file '%s'\n", name);
		close(fdin);
		return(0);
	}
	print_head(head);

	// загрузим новый заголовок
	if (head.startf != sizeof(tdr_head) &&
		head.startd != sizeof(tdr_head)) {
		fprintf(stdout, "New format\n");
		if (read(fdin, &headnew, sizeof(tdr_head_new)) != 80) {
			fprintf(stderr, "Error read HEAD_NEW from file '%s'\n",
				name);
			close(fdin);
			return(0);
		}
		print_headnew(headnew);
	} else {
		bzero(&headnew, sizeof(tdr_head_new));
	}

	// проверим на соответсвие
	if ((head.startf + head.sizef > fdstat.st_size) ||
		(head.startd + head.sized > fdstat.st_size) ||
		(head.startpt + head.sizept > fdstat.st_size) ||
		(headnew.startdes + headnew.sizedes > fdstat.st_size)) {
		fprintf(stderr, "Error format TDR file '%s'!\n", name);
		close(fdin);
		return(0);
	}

//	fprintf(stdout, "Alloc."); getchar();
	// аллокируем память для разбора
	files = (u_char *)malloc(head.sizef);
	dirs = (u_char *)malloc(head.sized);
	des = (u_char *)malloc(headnew.sizedes);

	// читаем
	if ((lseek(fdin, head.startf, SEEK_SET) != head.startf) ||
		(read(fdin, files, head.sizef) != head.sizef) ||
		(lseek(fdin, head.startd, SEEK_SET) != head.startd) ||
		(read(fdin, dirs, head.sized) != head.sized)) {
		fprintf(stderr, "Error read from file '%s'!\n", name);
		free(des);
		free(dirs);
		free(files);
		close(fdin);
		return(0);
	}

	// читаем описания если есть
	if (headnew.startdes &&
		lseek(fdin, headnew.startdes, SEEK_SET) != headnew.startdes ||
		read(fdin, des, headnew.sizedes) != headnew.sizedes) {
		fprintf(stderr, "Error read DESC from file '%s'!\n", name);
		free(des);
		des = NULL;
		headnew.startdes = 0;
		headnew.sizedes = 0;
	}
//	fprintf(stdout, ".Done\n"); getchar();
	print_dirs(dirs, head.sized/40, files, head.sizef/32, des);
//	fprintf(stdout, "End."); getchar();

	// освобождаем память и выходим
	free(des);
	free(dirs);
	free(files);
//	fprintf(stdout, ".Done\n"); getchar();
	close(fdin);
	return(0);
};

void print_head(tdr_head head) {
	fprintf(stdout, "StartF:\t\t0x%x\n", head.startf);
	fprintf(stdout, "StartD:\t\t0x%x\n", head.startd);
	fprintf(stdout, "StartPT:\t0x%x\n", head.startpt);
	fprintf(stdout, "SizeF:\t\t%u\n", head.sizef);
	fprintf(stdout, "SizeD:\t\t%u\n", head.sized);
	fprintf(stdout, "SizePT:\t\t%u\n", head.sizept);
	fprintf(stdout, "Format:\t\t%x\n", head.format);
	fprintf(stdout, "ID:\t\t%x\n", head.id);
	fprintf(stdout, "Length:\t\t%u sec. (%u min.)\n",
		head.length, head.length/60);
	fprintf(stdout, "SaveLen:\t%u sec. (%u min.)\n",
		head.savelen, head.savelen/60);
	fprintf(stdout, "NextSect:\t%u\n", head.nextsect);
	fprintf(stdout, "\n");
};

void print_headnew(tdr_head_new headnew) {
	fprintf(stdout, "StartDesc:\t0x%x\n", headnew.startdes);
	fprintf(stdout, "SizeDesc:\t%u\n", headnew.sizedes);
	fprintf(stdout, "LastSect:\t%u\n", headnew.lastsect);
	fprintf(stdout, "\n");
};

void store_dir(List *Rd, u_char *file, u_int mf, u_int sf, u_int ef, u_int nf) {
int		i;

	if (ef <= mf)
		for (i = 0; i < nf; i++)
			Rd->store((tdr_file *)(file) + sf + i);
//	Rd->print(des);
};

void print_dirs(u_char *dir, u_int cd, u_char *file, u_int mf, u_char *des) {
u_short		level, plevel, sf, ef, nf;
tdr_file	*dfile;

	/* root dir */
	level = *(u_short *)dir;
	dir += 2;

	dfile = (tdr_file *)dir;
	List RootDir(dfile, level);
	dir += 32;

	/* start file */
	sf = *(u_short *)dir;
	dir += 2;

	/* end file */
	ef = *(u_short *)dir;
	dir += 2;

	/* num file - rudiment */
	nf = *(u_short *)dir;
	dir += 2;

	store_dir(&RootDir, file, mf, sf, ef, nf);

	cd--;
	plevel = level;

	while(cd--) {
		level = *(u_short *)dir;
		dir += 2;
		if (plevel >= level)
			while(plevel-- >= level)
				RootDir.chdir("..");

		dfile = (tdr_file *)dir;
		RootDir.store(dfile, level);
		{
			Info File(dfile);
			RootDir.chdir(File.getname());
		}
		dir += 32;

		/* start file */
		sf = *(u_short *)dir;
		dir += 2;

		/* end file */
		ef = *(u_short *)dir;
		dir += 2;

		/* num file - rudiment */
		nf = *(u_short *)dir;
		dir += 2;

		store_dir(&RootDir, file, mf, sf, ef, nf);
		plevel = level;
	}
//	RootDir.print(des);
	RootDir.chdir("/");
	RootDir.print(des, 1);
};
