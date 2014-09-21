/*
 * Id		- File/Dir Name
 * Dtime	- File/Dir Date
 * Attr		- File/Dir Attributes
 * Size/Root	- File Size/Dir root
 * Sect/Tree	- File Sector/Dir entry tree
 * Desc		- File/Dir Description
 * *prev/NULL	- prevision item
 * *next/NULL	- next item
 *
 * root, entrdir
 */

#include "string.h"
#include "time.h"
#include "errno.h"
#include "Tape.h"		/* константы */
#include "tdr.h"		/* формат TDR */
#include "avt.h"		/* формат AVT */
#include "ReadWrite.h"		/* структура для чтения/записи */

extern "C" long dos2unixtime(u_short dd, u_short dt);
extern "C" u_int dos2tapeattr(u_char at);

struct	NewEntry {
	u_int	attr;		//
	u_int	uid, gid;
	u_int	size;
	u_long	dtime;
	u_char	*name;
	u_char	*desc;
};

struct	Entry {
	u_long	Id;		// номер имени в списке имен
	u_long	Dtime;		// дата в формате UNIX
	u_long	Attr;		// аттрибут в формате UNIX
	union	{
/* File */	u_long	Size;	// размер файла в байтах
/* Dir */	Entry	*root;	// указатель на родителя
		};
	union	{
/* File */	u_long	Sect;	// певый сектор на ленте
/* Dir */	Entry	*tree;	// указатель на первый элемент
		};
	u_long	Desc;		// номер описания в списке описаний
	Entry	*prev;		// предыдущий элемент
	Entry	*next;		// следующий элемент
	Entry	*mr;		// root
};

/*
 * Основной класс работы с файлами и каталогами на ленте.
 * формируются отдельные входы для загрузки и выгрузки
 * оглавлений форматов лент.
 */
class	TapeDir {
	Entry	*root;		// первый
	Entry	*curd;		// фактически curd->tree
	Entry	*cur;		// текущий
	u_int	r_used;
	u_int	r_alloc;
	u_char	**name;		// список имен
	u_int	nameid;		// последний id
	u_int	namealloc;	// размер
 u_long	AddName(char *);	// вставка имени и выдача ID
	u_char	**desc;		// список описаний
	u_int	descid;
	u_int	descalloc;
 u_long	AddDesc(char *);	// вставка описания и выдача ID
	union	{
		TDR_PT	*tdrpt;	// таблица позиционирования
		AVT_PT	*avtpt;
		};
	u_int	ptsize;
// TDR
	u_short	format;		// формат ленты
	u_int	id;		// идентификатор
	u_short	length;		// длина ленты
	u_short	savelen;	// длина записанного участка
	u_int	nextsect;	// следующий для записи сектор
 public:
 ReadWrite	RW;
	TapeDir();
	~TapeDir();
 int	LoadTDR(char *);		// загрузить из TDR
 int	SectToSec(u_int);
 int	InsEntry(NewEntry *, u_int);	// создать вход
 int	ChDir(u_char *);		// войти
 ReadWrite	readdir();		// читать вход
 void	PrintAttr(u_int);		// напечатать атрибуты
 void	PrintDir(Entry *);		// распечать каталог
};

TapeDir::TapeDir() {
	root = curd = cur = NULL;
	name = desc = NULL;
	r_used = r_alloc = 0;
	nameid = descid = 0;
	namealloc = descalloc = 0;
//
	tdrpt = NULL;
	ptsize = 0;
//	bzero(&RW, sizeof(ReadWrite));
};

#define	FNAME	"D.name"
#define	FDESC	"D.desc"

TapeDir::~TapeDir() {
#if 1
	if (root) {
		cur = root;
		PrintDir(cur);
//		PrintDir(cur->tree);
		free(root);
		};
#endif
	if (name) {
		int	d;

		d = open(FNAME, O_WRONLY|O_CREAT, 0666);
		if (d >= 0) {
		int	c;
			printf("Write '%s'\n", FNAME);
			for (c=0; c<nameid; c++) {
				write(d, name[c], strlen((char *)name[c])+1);
				free(name[c]);
				};
			close(d);
			}
		else
			printf("Error open '%s' %d\n", FNAME, d);
		free(name);
		};
	if (desc) {
		int	d;

		d = open(FDESC, O_WRONLY|O_CREAT, 0666);
		if (d >= 0) {
		int	c;
			printf("Write '%s'\n", FDESC);
			for (c=0; c<descid; c++) {
				write(d, desc[c], strlen((char *)desc[c])+1);
				free(desc[c]);
				};
			close(d);
			}
		else
			printf("Error open '%s' %d\n", FDESC, d);
		free(desc);
		};
//
	if (tdrpt)
		free(tdrpt);
};

/*
 * загрузка из файла оглавлений формата TDR
 * 0 - OK
 */
int TapeDir::LoadTDR(char *file) {
TDR_HEAD	head;
TDR_ADD_HEAD	addhead;
int		fdin;
void		*pfiles, *pdirs, *pdescs;
int		errexit;
int	ds, fs, fsall, n;
NewEntry	ne;
int		lendesc, level, plevel, nlevel;
unsigned char	*cdup;

	cdup = "..\0";

	errexit = 0;
	pfiles = pdirs = pdescs = NULL;
	fdin = open(file, O_RDONLY, 0644);
	if (fdin < 0) {
		errexit = 1;
		goto errexit0;
		};

	if (read(fdin, &head, sizeof(TDR_HEAD)) != sizeof(TDR_HEAD)) {
		errexit = 2;
		goto errexit1;
		};

	format = head.format;
	id = head.ID;
	length = head.length;
	savelen = head.savelen;
	ptsize = head.sizept;
	nextsect = head.nextsect;
	printf("Start Files ........ %08lx\n", head.startf);
	printf("Start Directory .... %08lx\n", head.startd);
	printf("Start PosTable ..... %08lx\n", head.startpt);
	printf("Files size ......... %08lx/%lu b/%d\n",
		head.sizef, head.sizef, head.sizef/sizeof(TDR_FILES));
	printf("Directory size ..... %08lx/%lu b/%d\n",
		head.sized, head.sized, head.sized/SIZETDRDIR);
	printf("PosTable size ...... %08lx/%lu\n",
		head.sizept, head.sizept);
	printf("Tape format ............ %04x\n", format);
	printf("Tape ID ................ %04x\n", id);
	printf("Tape length ............ %04x/%us/%um\n",
		length, length, length/60);
	printf("Tape save length ....... %04x/%us/%um\n",
		savelen, savelen, savelen/60);
	printf("Next sector for write .. %08lx/%lu\n",
		nextsect, nextsect);
	printf("Density save ....... %dK/s -> %uK/s\n",
		(50 * 512 * (format==4?4:8)) / 1024,
		(nextsect * 512 / ((format==4?4:8) * 50)
		/ savelen * (format==4?4:8) * 50) / 1024);

	printf("\n");
	/*
	 * Description's
	 */
	if ((head.startf != sizeof(TDR_HEAD))
		&& (head.startd != sizeof(TDR_HEAD))
		&& (head.startpt != sizeof(TDR_HEAD))) {
		printf("Loading descriptions:");
		if (read(fdin, &addhead, sizeof(TDR_ADD_HEAD)) != sizeof(TDR_ADD_HEAD)) {
			errexit = 2;
			goto errexit1;
			};
		if (lseek(fdin, addhead.startdes, SEEK_SET) != addhead.startdes) {
			errexit = 3;
			goto errexit1;
			};
		pdescs = malloc(addhead.sizedes);
		if (read(fdin, pdescs, addhead.sizedes) != addhead.sizedes) {
			errexit = 2;
			goto errexit1;
			};
		printf(" %u Ok\n", addhead.sizedes);
		};

	/***************
	 * Загружаем PT
	 ***************/
//	if (thead.sizept > sizeof(TDR_PT))
//		thead.sizept = sizeof(TDR_PT);
	printf("Loading PT: %u", head.sizept);
	tdrpt = (TDR_PT *)malloc(head.sizept);
	if (lseek(fdin, head.startpt, SEEK_SET) != head.startpt) {
		errexit = 3;
		printf("Error lseek to startpt\n");
		goto errexit1;
		};
	if (read(fdin, tdrpt, head.sizept) != head.sizept) {
		errexit = 2;
		printf("Error reading '%s' %x\n", name, head.sizept);
		goto errexit1;
		};
	printf(" Ok\n");
	printf("Pt size: %u sec\n", tdrpt->size << 3);

	/* ========================
	 * загружаем таблицу файлов
	 * ======================== */
	printf("Loading files: ");
	if (lseek(fdin, head.startf, SEEK_SET) != head.startf) {
		errexit = 3;
		printf("Error lseek to startf\n");
		goto errexit1;
		};
	pfiles = malloc(head.sizef);
	if (read(fdin, pfiles, head.sizef) != head.sizef) {
		errexit = 2;
		goto errexit1;
		};
	printf("Ok\n");

	/* ===========================
	 * загружаем таблицу каталогов
	 * =========================== */
	printf("Loading directories: ");
	if (lseek(fdin, head.startd, SEEK_SET) != head.startd) {
		errexit = 3;
		printf("Error lseek to startd\n");
		goto errexit1;
		};
	pdirs = malloc(head.sized);
	if (read(fdin, pdirs, head.sized) != head.sized) {
		errexit = 2;
		goto errexit1;
		};
	printf("Ok\n");

	/*
	 *
	 */
	fsall = 0;
	ne.uid = ne.gid = 0;
	ne.name = (u_char *)malloc(8+1+3+1);
	lendesc = 256;
	ne.desc = (u_char *)malloc(lendesc);
	level = plevel = nlevel = 0;
	for (ds=0; ds<head.sized/SIZETDRDIR; ds++) {
	TDR_DIR		*pd=(TDR_DIR *)pdirs + ds;

		printf("Level: %d\n", pd->level);

	/* атрибуты */
		ne.size = 0;
		ne.attr = dos2tapeattr(pd->attr);
		ne.dtime = dos2unixtime(pd->dd, pd->dt);
	/* имя */
		{
		int	ni, ei;
		u_char	c, *un=ne.name;

		for (ni = 7; ni >= 0; ni--)
			if (pd->name[ni] != ' ')
				break;
		for (ei = 10; ei >= 8; ei--)
			if (pd->name[ei] != ' ')
				break;
		for (n = 0; n <= ni; n++) {
			c = pd->name[n];
			*un++ = (c >= 'A' && c <= 'Z') ? (c | ' ') : c;
			}
		if (ei >= 8) {
			*un++ = '.';
			for (n = 8; n <= ei; n++) {
				c = pd->name[n];
				*un++ = (c >= 'A' && c <= 'Z') ? (c | ' ') : c;
				}
			}
		*un++ = 0;
		}
	/* нулевой уровень - имя корневого каталога == имя файла :) */
		if ((ne.name[n] == '\0') && (pd->level == 0)) {
		int	tlen=strlen((char *)file);
			if (tlen > 8+1+3) {
				free(ne.name);
				ne.name = (u_char *)malloc(tlen+1);
				bzero(ne.name, tlen+1);
				};
			strncat((char *)ne.name, (char *)file, tlen);
			};
	/* описание */
		bzero(ne.desc, lendesc);
		n = pd->startdesc[0] + (pd->startdesc[1] << 16);
		if (n) {
		char	*ddesc = n + (char *)pdescs - 1;
			n = *ddesc++;
			if (n > lendesc)
				n = lendesc - 1;
			strncpy((char *)ne.desc, ++ddesc, n);
			};
	/* уровень */
//		if (pd->level > level)
		if (plevel && (plevel == pd->level))
			ChDir(cdup);
		if (plevel > pd->level) {
			ChDir(cdup);
			while (plevel > pd->level) {
				ChDir(cdup);
				plevel--;
				};
			};
		plevel = pd->level;
	/* создаем директорию */
		if (InsEntry(&ne, 0)) {
			printf("Error insert entry '%s'.\n", ne.name);
			}
		else if (ChDir(ne.name) == 0) {
			if (pd->numf) {
				printf("%d-%d/%d\n", pd->startf, pd->lastf, pd->numf);
				for (fs=pd->startf; fs <= pd->lastf; fs++) {
				TDR_FILES	*pf=(TDR_FILES *)pfiles + fs;

					ne.attr = dos2tapeattr(pf->attr);
					ne.size = pf->size;
					ne.dtime = dos2unixtime(pf->dd, pf->dt);
					{
					int	ni, ei;
					u_char	c, *un=ne.name;

					for (ni = 7; ni >= 0; ni--)
						if (pf->name[ni] != ' ')
							break;
					for (ei = 10; ei >= 8; ei--)
						if (pf->name[ei] != ' ')
							break;
					for (n = 0; n <= ni; n++) {
						c = pf->name[n];
						*un++ = (c >= 'A' && c <= 'Z') ? (c | ' ') : c;
						}
					if (ei >= 8) {
						*un++ = '.';
						for (n = 8; n <= ei; n++) {
							c = pf->name[n];
							*un++ = (c >= 'A' && c <= 'Z') ? (c | ' ') : c;
							}
						}
					*un++ = 0;
					}
				/* описание */
					bzero(ne.desc, lendesc);
					n = pf->startdesc;
					if (n) {
					char	*fdesc = n + (char *)pdescs - 1;
						n = *fdesc++;
						if (n > lendesc)
							n = lendesc - 1;
						strncpy((char *)ne.desc, ++fdesc, n);
						};
					if (InsEntry(&ne, pf->startsect)) {
						printf("Error insert entry '%s'.\n", ne.name);
						};
					}; /* for (fs) */
				printf("files: %d\n", pd->numf);
				fsall += pd->numf;
				}; /* pd->numf */
			}
		else	{
			printf("Cannot chdir to entry '%s'.\n", ne.name);
			}; /* Cd(ne) */
		}; /* for (ds) */
	free(ne.desc);
	free(ne.name);
	printf("directories: %d", ds);
	printf(", files: %d\n", fsall);
errexit1:
	if (pdirs)
		free(pdirs);
	if (pfiles)
		free(pfiles);
	if (pdescs)
		free(pdescs);
	close(fdin);
errexit0:
	return(errexit);
};
#if 0
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
#endif

/*
 * возвращает положение сектора в секундах
 */
int TapeDir::SectToSec(u_int sector) {
int	h, l, time;
int	nfmt;
char	hour, min, sec;
u_int	osector, nsector, csector;

	if ((sector < 0) || (sector == 0))
		return(0);

	osector = nsector = csector = 0;
	nfmt = 8;
	if (format == 8)
		nfmt++;

	/* ищем в старшей половине */
	for (h=0; h < 0x8d; h++) {
		time = h*8*32;
		csector = tdrpt->h[h] << nfmt;
		if ((csector == 0) && h)
			break;
		if (sector == csector)
			return(time);
		if (sector < csector) {
			if (h)
				h--;
			time = h*8*32;
			csector = tdrpt->h[h] << nfmt;
			/* ищем в младшей половине */
			hour = time / 3600;
			min = (time - hour*3600) / 60;
			sec = time - hour*3600 - min*60;
			printf("%02d:%02d:%02d", hour, min, sec);
			printf(": %8d", csector);
			osector = nsector = csector;
			for (l=0; l < 31; l++) {
				csector = (tdrpt->l[l+h*31] << nfmt) + nsector;
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

/*
 * Вставка элемента в текущий каталог.
 * Если элемент первый, то создаем root.
 */
int TapeDir::InsEntry(NewEntry *in, u_int stsect) {
long		days;
struct tm	*ct;
Entry		*n;

	/* проверим на дурака */
	if (in == NULL)		return(EFAULT);
	if (in->name == NULL)	return(EINVAL);
	if (*in->name == '\0')	return(EINVAL);

	/* найдем вхождения */
#if 0
	if (FindEntry(in->name))	return(EEXIST);
#endif
	/* создание элемента */
	/* выделяем память */
	n = (Entry *)malloc(sizeof(Entry));
	n->Id = AddName((char *)in->name);
	n->Dtime = in->dtime;
	n->Attr = in->attr;
	if (n->Attr & TAPE_DIR) {
	/* каталог */
//		printf("Is dir\n");
//		n->root;
		n->tree = NULL;
		}
	else	{
	/* фаил */
//		printf("Is file\n");
		n->Size = in->size;
		n->Sect = stsect & 0x3fffffffl;
		};

	PrintAttr(n->Attr);

	ct=gmtime((long *)&n->Dtime);
	printf(" %02d/%02d/%02d %02d:%02d:%02d",
		ct->tm_mday, ct->tm_mon+1, ct->tm_year,
		ct->tm_hour, ct->tm_min, ct->tm_sec);

	if (!(n->Attr & TAPE_DIR)) {
		printf(" %10u", n->Size);
		printf(" %10u", n->Sect);
		};
	printf(" %d %s", n->Id, in->name);
	if (*in->desc) {
		n->Desc = AddDesc((char *)in->desc);
		printf(" %d '%s'", n->Desc, in->desc);
		};
	printf("\n");

	/* вставляем элемент */
	if (curd == NULL)
		curd = root;
	if (curd) {
	/* пляшем от curd->tree */
		if (curd->Attr & TAPE_DIR) {
		/* если curd всетаки каталог */
			if (curd->tree) {
				cur = curd->tree;
				while (cur->next)
					cur = cur->next;
				cur->next = n;
				n->prev = cur;
				n->next = NULL;
				n->mr = curd;
				}
			else	{
			/* первый элемент в каталоге */
				curd->tree = n;
				n->prev = NULL;
				n->next = NULL;
				n->mr = curd;
				};
			}
		else	{
		/* curd не каталог */
		/* найдем последний */
			for (cur=curd; cur->next; cur=cur->next);
		/* вставим */
			cur->next = n;
			n->prev = cur;
			n->next = NULL;
			n->mr = cur->mr;
			};
		}
	else	{
	/* первый элемент в дереве */
	//	printf("root = %u/%u\n", n->Id, n->tree);
		root = curd = n;
		n->next = n->prev = n->mr = NULL;
		};
//	free(n);

	return(0);
};

/*
 *
 */
u_long TapeDir::AddName(char *fn) {
int	len;
u_long	sizem;

	len = strlen(fn) + 1;
	if (namealloc == nameid) {
		namealloc += 100;
		sizem = namealloc * sizeof(u_char *);
		printf("*** Add namealloc: %d\n", sizem);
		if (nameid) {
			name = (u_char **)realloc(name, sizem);
			}
		else	{
			name = (u_char **)malloc(sizem);
			};
		};
	name[nameid] = (u_char *)malloc(len);
	strncpy((char *)name[nameid], fn, len);

	return(nameid++);
};

/*
 *
 */
u_long TapeDir::AddDesc(char *fn) {
int	len;
u_long	sizem;

	len = strlen(fn) + 1;
	if (descalloc == descid) {
		descalloc += 100;
		sizem = descalloc * sizeof(u_char *);
		printf("*** Add namealloc: %d\n", sizem);
		if (descid) {
			desc = (u_char **)realloc(desc, sizem);
			}
		else	{
			desc = (u_char **)malloc(sizem);
			};
		};
	desc[descid] = (u_char *)malloc(len);
	strncpy((char *)desc[descid], fn, len);

	return(descid++);
};

/*
 * смена текущей директории
 */
int TapeDir::ChDir(u_char *fn) {
Entry	*tcur;
int	find;

	if (curd == NULL)
		tcur = root;
	else	{
		if (curd->Attr & TAPE_DIR)
			tcur = curd->tree;
		else
			tcur = curd;
		};
	if (strncmp((char *)fn, ".\0", 2) == 0)
		return(0);
	if (strncmp((char *)fn, "..\0", 3) == 0) {
//		if (curd->mr != NULL)
			curd = curd->mr;
		return(0);
		};
	find = 1;
//	printf("%d\n", tcur->Id);
	for (; tcur && find; tcur=tcur->next) {
		if (tcur->Attr & TAPE_DIR) {
			printf("%s %s\n", name[tcur->Id], fn);
			if (strcmp((char *)name[tcur->Id], (char *)fn) == 0) {
				curd = tcur;
				find = 0;
				};
			};
		};

	return(find);
/*	return (0);
*/
};

void TapeDir::PrintAttr(u_int attr) {
	if (attr & TAPE_DIR)
		printf("d");
	else
		printf("-");
	if (attr & TAPE_RUSR)
		printf("r");
	else
		printf("-");
	if (attr & TAPE_WUSR)
		printf("w");
	else
		printf("-");
	if (attr & TAPE_XUSR)
		printf("x");
	else
		printf("-");
	if (attr & TAPE_RGRP)
		printf("r");
	else
		printf("-");
	if (attr & TAPE_WGRP)
		printf("w");
	else
		printf("-");
	if (attr & TAPE_XGRP)
		printf("x");
	else
		printf("-");
	if (attr & TAPE_ROTH)
		printf("r");
	else
		printf("-");
	if (attr & TAPE_WOTH)
		printf("w");
	else
		printf("-");
	if (attr & TAPE_XOTH)
		printf("x");
	else
		printf("-");
};

void TapeDir::PrintDir(Entry *tc) {
Entry	*ptc;
int	tdir;

	tdir = 0;
	while (tc) {
	/* печать каталога */
		if (tdir == 0) {
			ptc = tc;
			while (ptc) {
				PrintAttr(ptc->Attr);
				printf(" ");
				printf("%s", name[ptc->Id]);
				printf("\n");
				ptc = ptc->next;
				};
			printf("\n");
			tdir = 1;
			}; /* if (tdir) */
		if (tc->Attr & TAPE_DIR) {
			if (tc->tree) {
				printf("Entering: %s\n", name[tc->Id]);
				tc = tc->tree;
				tdir = 0;
				goto Next1;
				};
			}; /* if (Attr) */
		tc = tc->next;
Next1:;
		};
};

ReadWrite TapeDir::readdir() {

	if (cur == NULL)
		cur = root;

	if (RW.FName) {
		free(RW.FName);
		RW.FName = NULL;
		}
	if (RW.TName) {
		free(RW.TName);
		RW.TName = NULL;
		}

	return(TapeDir::RW);
};
