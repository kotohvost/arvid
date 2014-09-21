/*
 * tdr.hpp
 */
#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"

/* Заголовок файла старого TDR */
struct	TDR_HEAD {
	u_int	startf;		/* смещение таблицы файлов */
	u_int	startd;		/* смещение таблицы каталогов */
	u_int	startpt;	/* смещение таблицы позиционирования */
	u_int	sizef;		/* размер таблицы файлов */
	u_int	sized;		/* размер таблицы каталогов */
	u_int	sizept;		/* размер таблицы позиционирования */
	u_short	format;		/* формат ленты */
	u_short	ID;		/* ID ленты */
	u_short	length;		/* длина ленты в секундах */
	u_short	savelen;	/* длина записанного участка ленты */
	u_int	nextsect;	/* следующий для записи сектор */
};

/* Описание добавочных полей TDR фомата */
struct	TDR_ADD_HEAD {
	u_int	startdes;	/* смещение таблицы description's */
	u_int	reserv1a;	/*  */
	u_int	reserv2a;	/*  */
	u_int	reserv3a;
	u_int	reserv4a;
	u_int	sizedes;	/* размер таблицы description */
	u_int	reserv1b;	/*  */
	u_int	reserv2b;	/*  */
	u_int	reserv3b;
	u_int	reserv4b;
	u_int	lastsect;	/* предыдущий сектор для отката */
	u_int	reserv[9];
};

/* Описание полей таблицы файлов */
struct	TDR_FILES {
	char	name[8];	/* имя файла */
	char	ext[3];		/* расширение файла */
	char	attr;		/* атрибут файла */
	u_int	startsect;	/* начальный сектор */
	u_int	startdesc;	/* смещение описания */
	u_short	reserv1;
	u_short	time;		/* время файла в формате DOS */
	u_short	date;		/* дата файла в формате DOS */
	u_short	clastr;		/* не используется */
	u_int	size;		/* размер в байтах */
};

/* Описание полей таблицы каталогов */
struct	TDR_DIR {
	u_short		a;	// бардак
	u_short		level;		/* уровень каталога */
	TDR_FILES	f;		/* описание каталога */
	u_short		startf;		/* номер первого файла */
	u_short		lastf;		/* номер последнего файла (рудимент) */
	u_short		numf;		/* число файлов */
	u_short		b;	// бардак
};
#define	SIZETDRDIR	40

struct TDR_DIRn;
/* переходная структура для работы с файлами */
struct	TDR_FILESn {
	TDR_FILES	f;	/* описание файла */
	TDR_FILESn	*next;	/* сосед */
	TDR_FILESn	*prior;	/* предыдущий */
	TDR_DIRn	*root;	/* каталог */
};

/* перходная структура для работы с каталогами */
struct	TDR_DIRn {
	u_short		a;	// бардак
	u_short		level;		/* уровень каталога */
	TDR_FILES	f;		/* описание каталога */
	u_short		startf;		/* номер первого файла */
	u_short		lastf;		/* номер последнего файла (рудимент) */
	u_short		numf;		/* число файлов */
	u_short		b;	// бардак
	TDR_DIRn	*next;		/* следующий сосед */
	TDR_DIRn	*prior;		/* предыдущий */
	TDR_DIRn	*nextd;		/* директории */
	TDR_FILESn	*nextf;		/* файлы */
	TDR_DIRn	*root;		/* каталог */
};

struct	TDR_PT {
	u_short		size;
	u_short		h[0x8d];
	u_char		l[0x1114];
};

class	TDR {
	u_int		cFormat;	// формат записи (PT)
	TDR_FILESn	*Sfiles;	// указатель на список файлов
	TDR_FILESn	*Efiles;	// указатель на последний
	TDR_FILESn	*nSf;		// указатель на текущий фаил
	u_int		Nfiles;		// количество файлов

	TDR_DIRn	*Sdir;		// указатель на список каталогов
	TDR_DIRn	*Edir;		// указатель на последний
	TDR_DIRn	*nSd;		// указатель на текущий каталог
	u_int		Ndir;		// количество каталогов

	TDR_PT		*Spt;		// таблица позиционирования
	u_char		*Sdesc;		// описатели файлов
 public:
	TDR();
	~TDR();
 int	LoadTDR(char *name);	// загрузка TDR из файла
 int	Cd(char *dir);		// перейти в каталог
 int	Dir(FILE *fdout);	// распечатка текущего каталога
 int	SectToSec(u_int);	// сектор -> секунды

 int	DelAllF(TDR_DIRn *cSd);	// удаление всех файлов из директории
 int	DelAll(TDR_DIRn *cSd);	// удаление всего из текущей директории
};

/*
 * 0 - OK!
 */
int TDR::LoadTDR(char *name) {
int		fdin;
int		count;
int		level;
TDR_HEAD	thead;	// ???
TDR_DIRn	*rSd;	// предыдущий уровень каталога
TDR_DIRn	*pnSd;	// предыдущий каталог
TDR_FILESn	*tSf, *pSf;

	fdin = open(name, O_RDONLY, 0644);
	if (fdin < 0) {
		printf("Error open %s\n", name);
		return (1);
		};
	if (read(fdin, &thead, sizeof(TDR_HEAD)) != sizeof(TDR_HEAD)) {
		close(fdin);
		printf("Error read %s\n", name);
		return (1);
		};
	printf("Start Files ........ %08lx\n", thead.startf);
	printf("Start Directory .... %08lx\n", thead.startd);
	printf("Start PosTable ..... %08lx\n", thead.startpt);
	printf("Files size ......... %08lx/%lu/%d\n", thead.sizef, thead.sizef, thead.sizef/sizeof(TDR_FILES));
	printf("Directory size ..... %08lx/%lu/%d\n", thead.sized, thead.sized, thead.sized/SIZETDRDIR);
	printf("PosTable size ...... %08lx/%lu\n", thead.sizept, thead.sizept);
	printf("Tape format ............ %04x\n", thead.format);
	cFormat = thead.format;
	printf("Tape ID ................ %04x\n", thead.ID);
	printf("Tape length ............ %04x/%us\n", thead.length, thead.length);
	printf("Tape save length ....... %04x/%us\n", thead.savelen, thead.savelen);
	printf("Next sector for write .. %08lx/%lu\n", thead.nextsect, thead.nextsect);

	/* ========================
	 * загружаем таблицу файлов
	 * ======================== */
	printf("\nLoading files: ");
	if (lseek(fdin, thead.startf, SEEK_SET) != thead.startf) {
		close(fdin);
		printf("Error lseek to startf\n");
		return (1);
		};
	for(Nfiles = 0; Nfiles < (thead.sizef / sizeof(TDR_FILES)); Nfiles++) {
		if (!Sfiles) {
			Sfiles = Efiles = nSf = (TDR_FILESn *)malloc(sizeof(TDR_FILESn));
			nSf->prior = NULL;
			}
		else	{
			nSf = Efiles->next = (TDR_FILESn *)malloc(sizeof(TDR_FILESn));
			nSf->prior = Efiles;
			Efiles = nSf;
			};
		nSf->next = NULL;
		nSf->root = NULL;
		if (read(fdin, nSf, sizeof(TDR_FILES)) != sizeof(TDR_FILES)) {
			close(fdin);
			printf("Error reading %s\n", name);
			return (1);
			};
//		printf(".");
		};
	printf("Loading %d files\n", Nfiles);
	/* ===========================
	 * загружаем таблицу каталогов
	 * =========================== */
	printf("\nLoading directories: ");
	if (lseek(fdin, thead.startd, SEEK_SET) != thead.startd) {
		close(fdin);
		printf("Error lseek to startd\n");
		return (1);
		};
	for(Ndir = 0; Ndir < (thead.sized / SIZETDRDIR); Ndir++) {
		if (!Sdir)
			Sdir = Edir = nSd = (TDR_DIRn *)malloc(sizeof(TDR_DIRn));
		else	{
			nSd = Edir->next = (TDR_DIRn *)malloc(sizeof(TDR_DIRn));
			nSd->prior = Edir;
			Edir = nSd;
			};
		nSd->next = NULL;
		nSd->nextd = NULL;
		nSd->nextf = NULL;
		nSd->root = NULL;
		if (read(fdin, &nSd->level, SIZETDRDIR) != SIZETDRDIR) {
			close(fdin);
			printf("Error reading %s\n", name);
			return (1);
			};
//		printf(".");
		};
	printf("Loading %d directories\n", Ndir);

	/***************
	 * Загружаем PT
	 ***************/
//	if (thead.sizept > sizeof(TDR_PT))
		thead.sizept = sizeof(TDR_PT);
	printf("\nLoading PT: %u", thead.sizept);
	Spt = (TDR_PT *)malloc(thead.sizept);
	if (lseek(fdin, thead.startpt, SEEK_SET) != thead.startpt) {
		close(fdin);
		printf("Error lseek to startpt\n");
		return (1);
		};
	if (read(fdin, Spt, thead.sizept) != thead.sizept) {
		close(fdin);
		printf("Error reading '%s' %x\n", name, thead.sizept);
		return(1);
		};
	printf(" Ok\n");
	printf("Pt size: %u sec\n", Spt->size << 3);

	/* закроем фаил */
	close(fdin);
	/*
	 * раставляем файлы по каталогам
	 * 1 - раставим root
	 * 2 - nextf
	 * 3 - nextd
	 * потерянные файлы в root ???
	 * (поимка кросс-линков ???)
	 */
	count = 0;	// текущий номер файла
	level = 0;	// текущий номер уровня каталога
	nSf = Sfiles;
	rSd = NULL;	// предыдущий уровень каталга
	pnSd = Sdir;
	for (nSd=Sdir; nSd; nSd=nSd->next) {
	// если есть файлы
		if (nSd->startf <= Nfiles) {
		// найдем первый фаил
			if (count > nSd->startf) {
			// найдем с первого
				nSf = Sfiles;
				count=0;
				while ((count <= Nfiles)
					 && (count < nSd->startf)
					 && nSf->next) {
					count++;
					nSf = nSf->next;
					}; /* while */
				}
			else	{
			// найдем с текущего
				while ((count < Nfiles)
					 && (count < nSd->startf)
					 && nSf->next) {
					count++;
					nSf = nSf->next;
					}; /* while */
				}; /* count > nSd->startf */
		// nSf - первый фаил в каталоге
		// раставим root
			nSd->nextf = nSf;
			do {
				nSf->root = nSd;
				nSf = nSf->next;
				count++;
				} while ((count <= nSd->lastf)
				 && (count <= Nfiles)
				 && nSf);
			}; /* nSd->startf <= Nfiles */
		// здесь nSf может быть равен NULL !!!
		/*
		 * разбираем каталоги
		 * потерянные каталоги в root ???
		 */
		/* текущий уровень 0 */
		if (nSd->level == 0) {
			if (level)
				pnSd->next = NULL;
			rSd = NULL;
			level = 0;
			};
		/* предыдущий уровень */
		if (level > nSd->level) {
		// обнуляем предыдущий next
			pnSd->next = NULL;
		// ищем root
			do {
				rSd = rSd->root;
				level--;
				} while (level
				 && (level > nSd->level)
				 && rSd->root);
		// ищем next
			for (pnSd=rSd->nextd; pnSd->next; pnSd=pnSd->next);
			pnSd->next = nSd;
			};
		/* уровни равны */
		/* следующий уровень */
		if ((level+1) == nSd->level) {
		// обнуляем предыдущий next
			rSd = pnSd;
			rSd->next = NULL;
			rSd->nextd = nSd;
	//		level++;
			};
		/* бяка */
		if ((level+1) > nSd->level) {
	//		nSd->level = 0;
	//		level = 0;
	//		pSd = NULL;
			};
	// rSd - root
		nSd->root = rSd;
		level = nSd->level;
		pnSd = nSd;
		}; /* for */
	nSd = Sdir;
	if (Sfiles) {
		tSf = nSf = pSf = Sfiles;
		Sfiles = NULL;
		while (nSf) {
			if (nSf->root == NULL) {
				if (Sfiles) {
					tSf->next = nSf;
					tSf = nSf;
					}
				else
					Sfiles = tSf = nSf;
				}
			else	{
				if (pSf->root != nSf->root)
					pSf->next = NULL;
				};
			pSf = nSf;
			nSf = nSf->next;
			}; /* while */
		};
	return (0);
};

TDR::TDR() {
	Sfiles = nSf = NULL;
	Sdir = nSd = NULL;
	Spt = NULL;
	cFormat = Nfiles = Ndir = 0;
};

TDR::~TDR() {
int	i, count;

	printf("\nDestructor\n");
#if 0
	count = 0;
	if (Sfiles)
		for (nSf=Sfiles; nSf; nSf=Sfiles) {
			for(i=0; i<8; i++)
				printf("%c", nSf->f.name[i]);
			printf(" ");
			for(i=0; i<3; i++)
				printf("%c", nSf->f.ext[i]);
			printf(" ");
			printf("%11u", nSf->f.size);
			printf(" ");
			printf("%11u", nSf->f.startsect);
	// имя каталога
			printf(" ");
			if (nSf->root) {
				for(i=0; i<8; i++)
					printf("%c", nSf->root->f.name[i]);
				printf(" ");
				for(i=0; i<3; i++)
					printf("%c", nSf->root->f.ext[i]);
				}
			else	{
				printf("Error: not root directory!");
				};
			printf("\t");
			printf("%5d", count);
			printf("\n");
			Sfiles = Sfiles->next;
			count++;
			free(nSf);
			};
	if (Sdir)
		for (nSd=Sdir; nSd; nSd=Sdir) {
			for(i=0; i<8; i++)
				printf("%c", nSd->f.name[i]);
			printf(" ");
			for(i=0; i<3; i++)
				printf("%c", nSd->f.ext[i]);
			printf(" ");
			printf("%5u", nSd->level);
			printf(" ");
			printf("%5u", nSd->startf);
			printf(" ");
			printf("%5u", nSd->lastf);
			printf(" ");
			printf("%5u", nSd->numf);
		// имя каталога
			if (nSd->root) {
				printf(" ");
				for(i=0; i<8; i++)
					printf("%c", nSd->root->f.name[i]);
				printf(" ");
				for(i=0; i<3; i++)
					printf("%c", nSd->root->f.ext[i]);
				};
		// имя первого файла
			if (nSd->nextf) {
				printf(" ");
				for(i=0; i<8; i++)
					printf("%c", nSd->nextf->f.name[i]);
				printf(" ");
				for(i=0; i<3; i++)
					printf("%c", nSd->nextf->f.ext[i]);
				}
			printf("\n");
			Sdir = Sdir->next;
			free(nSd);
			};
#else
	if (Sdir) {
		DelAll(Sdir);
		free(Sdir);
		}
#endif
	if (Spt)
		free(Spt);
};

/*
 * перейти в каталог
 * 0 - OK
 */
int TDR::Cd(char *dir) {
	return(1);
};

/*
 * распечатка текущего каталога
 * 0 - OK
 */
int TDR::Dir(FILE *fdout) {
int		i;
TDR_DIRn	*tSd;
TDR_FILESn	*tSf;

	if (!nSd)
		return(1);
	// директории
	for (tSd=nSd->nextd; tSd; tSd=tSd->next) {
		for(i=0; i<8; i++)
			fprintf(fdout, "%c", tSd->f.name[i]);
		fprintf(fdout, " ");
		for(i=0; i<3; i++)
			fprintf(fdout, "%c", tSd->f.ext[i]);
		fprintf(fdout, " ");
		fprintf(fdout, "%5u", tSd->level);
		fprintf(fdout, " ");
		fprintf(fdout, "%5u", tSd->startf);
		fprintf(fdout, " ");
		fprintf(fdout, "%5u", tSd->lastf);
		fprintf(fdout, " ");
		fprintf(fdout, "%5u", tSd->numf);
		fprintf(fdout, "\n");
		};
	// файлы
	for (tSf=nSd->nextf; tSf; tSf=tSf->next) {
		for(i=0; i<8; i++)
			fprintf(fdout, "%c", tSf->f.name[i]);
		fprintf(fdout, " ");
		for(i=0; i<3; i++)
			fprintf(fdout, "%c", tSf->f.ext[i]);
		fprintf(fdout, " ");
		fprintf(fdout, "%11u", tSf->f.size);
		fprintf(fdout, " ");
		fprintf(fdout, "%11u", tSf->f.startsect);
		fprintf(fdout, "\n");
		};
};

/*
 * удаление файлов из директории
 */
int TDR::DelAllF(TDR_DIRn *cSd) {
TDR_FILESn	*tSf, *ntSf;
int		i;

	if (cSd == NULL)
		return(1);
	while (cSd->nextf) {
	// найдем последний фаил
		tSf = ntSf = cSd->nextf;
		while (tSf->next) {
			ntSf = tSf;
			tSf = tSf->next;
			};
#if 1
		for(i=0; i<8; i++)
			printf("%c", tSf->f.name[i]);
		printf(" ");
		for(i=0; i<3; i++)
			printf("%c", tSf->f.ext[i]);
		printf(" ");
		printf("%11u", tSf->f.size);
		printf(" ");
		printf("%11u", tSf->f.startsect);
		printf("\n");
#endif
	// удалим ссылку на последний фаил
		if (ntSf == tSf)	cSd->nextf = NULL;
		else			ntSf->next = NULL;
	// удалим последний фаил
		free(tSf);
		};
};

/*
 * удаление файлов и каталогов из директории
 */
int TDR::DelAll(TDR_DIRn *cSd) {
TDR_DIRn	*tSd, *ntSd;
int		i;

	while (cSd->nextd) {
	// найдем последний каталог
		tSd = ntSd = cSd->nextd;
		while (tSd->nextd || tSd->next) {
			ntSd = tSd;
			if (tSd->nextd)	tSd = tSd->nextd;
			else		tSd = tSd->next;
			};
#if 1
		printf("Directory: ");
		for(i=0; i<8; i++)
			printf("%c", tSd->f.name[i]);
		printf(" ");
		for(i=0; i<3; i++)
			printf("%c", tSd->f.ext[i]);
		printf(" ");
		printf("%5u", tSd->level);
		printf(" ");
		printf("%5u", tSd->startf);
		printf(" ");
		printf("%5u", tSd->lastf);
		printf(" ");
		printf("%5u", tSd->numf);
		printf("\n");
#endif
	// удалим файлы из каталога
		DelAllF(tSd);
		if (ntSd->nextd == tSd)		ntSd->nextd = NULL;
		else if (ntSd->next == tSd)	ntSd->next = NULL;
		else if (ntSd == tSd)		cSd->nextd = NULL;
	// удалим последний вход
		free(tSd);
		};
	printf("\\\n");
	DelAllF(nSd);
};

#if 0
int TDR::SectToSec(u_int sector) {
int	h, l;
int	nfmt;
char	hour, min, sec;
u_int	osector, dsector, nsector, csector;

	osector = dsector = nsector = csector = 0;
	nfmt = 8;
	if (cFormat == 8)
		nfmt++;
	printf("Pt size: %u sec\n", Spt->size << 3);
	printf("\t");
	for (l=0; l<32; l++)
		printf(", %8d", l*8);
	printf("\n");
	for (h=0; h < 0x8d; h++) {
		csector = Spt->h[h] << nfmt;
		if ((csector == 0) && h)
			break;
		dsector = csector - osector;
		hour = (h*8*32) / 3600;
		min = ((h*8*32) - hour*3600) / 60;
		sec = h*8*32 - hour*3600 - min*60;
		printf("%02d:%02d:%02d", hour, min, sec);
		printf(": %8d", dsector);
		osector = nsector = csector;
		for (l=0; l < 31; l++) {
			csector = (Spt->l[l+h*31] << nfmt) + nsector;
			if (osector > csector)
				break;
			dsector = csector - osector;
			printf(", %8d", dsector);
			osector = csector;
			};
		printf("\n");
		};
	return(1);
};
#else
/*
 * возвращает положение сектора в секундах
 */
int TDR::SectToSec(u_int sector) {
int	h, l, time;
int	nfmt;
char	hour, min, sec;
u_int	osector, nsector, csector;

	if ((sector < 0) || (sector == 0))
		return(0);

	osector = nsector = csector = 0;
	nfmt = 8;
	if (cFormat == 8)
		nfmt++;

	/* ищем в старшей половине */
	for (h=0; h < 0x8d; h++) {
		time = h*8*32;
		csector = Spt->h[h] << nfmt;
		if ((csector == 0) && h)
			break;
		if (sector == csector)
			return(time);
		if (sector < csector) {
			if (h)
				h--;
			time = h*8*32;
			csector = Spt->h[h] << nfmt;
			/* ищем в младшей половине */
			hour = time / 3600;
			min = (time - hour*3600) / 60;
			sec = time - hour*3600 - min*60;
			printf("%02d:%02d:%02d", hour, min, sec);
			printf(": %8d", csector);
			osector = nsector = csector;
			for (l=0; l < 31; l++) {
				csector = (Spt->l[l+h*31] << nfmt) + nsector;
				if ((osector > csector) || (sector < csector))
					break;
				time += 8;
				printf(", %8d", csector);
				osector = csector;
				};
			printf("\n");
			return(time);
			};
		};
	return(0);
};
#endif
