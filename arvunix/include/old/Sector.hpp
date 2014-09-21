/*
 *
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct Sector {
	size_t	slogic;	/* логический сектор в файле */
	u_int	start;	/* стартовый сектор */
	u_int	end;	/* последний сектор включительно */
	char	*Name;	/* имя файла */
	char	*To;	/* имя записываемого файла */
	size_t	sizeb;	/* размер в байтах */
	int	s1, s2, s3, verf;
	Sector	*next;	/* следуюший блок */
	Sector	*prev;	/* предыдущий блок */
} SECTOR;

class SectorList {
 public:
	Sector	*beg;		// указатель на начало
	Sector	*end;		// указатель на последний
	Sector	*cur;		// указатель на текущий

	SectorList();
	~SectorList();
	// добавить в очередь
 int	Add(size_t l, u_int s, u_int e, char *n, char *t, size_t b);
	// поиск вхождения по сектору
 int	Find(u_int s);
	// удаление вхождения по сектору
 int	Del(u_int s);
 void	PrintAll();
};

SectorList::SectorList() {
	beg = end = cur = NULL;
};

SectorList::~SectorList() {
	if (beg) {
		for (cur = beg; cur; cur = beg) {
			beg = cur->next;
			if (cur->Name)
				free(cur->Name);
			if (cur->To)
				free(cur->To);
			free(cur);
			};
		};
	beg = end = cur = NULL;
};

/*
 * cur - найденное вхождение или NULL
 * 0 - OK
 */
int SectorList::Find(u_int sect) {
int	find;

	/* проверим на наличие */
	if (cur == NULL)
		cur = beg;

	find = 1;
	while (cur && find) {
		if ((cur->start <= sect) && (cur->end >= sect))
			find = 0;
		else	{
			if (cur->end < sect) {
			/* идем в конец */
				cur = cur->next;
				if (cur)
				/* проверим на дырку */
					if (cur->start > sect)
						cur = NULL;
				}
			else	{
			/* идем в начало */
				cur = cur->prev;
				if (cur)
				/* проверим на дырку */
					if (cur->end < sect)
						cur = NULL;
				};
		//	cur = NULL;
			};
		};
	return (find);
};

/*
 * cur - новое вхождение
 * ls - logic sector
 * sts - start sector
 * ends - end sector
 * *name - file name
 * *to - file to
 * sizeb - file size of byte
 * 0 - OK
 */
int SectorList::Add(size_t ls, u_int sts,
		u_int ends, char *name,
		char *to, size_t sizeb) {
Sector	*p;
char	*tmpc;
int	len, find;

	/* защита от идиотов */
	if (sts > ends)
		return (1);
	if (sizeb == 0)
		return (1);

	/* ищем уже имеющееся вхождение */
	if (Find(sts) == 0)
		return (1);
	if (Find(ends) == 0)
		return (1);

	/* вставляем по порядку */
	cur = beg;
	find = 1;
	while (cur && find) {
		if (cur->start > sts) {
			find = 0;
			}
		else
			cur = cur->next;
		}; /* while */
	/* здесь *cur может равняться NULL */
	if ((p = (Sector *)malloc(sizeof(SECTOR))) == NULL) {
		printf("ERROR: Out of memory\n");
		return (1);
		};
	p->slogic = ls;
	p->start = sts;
	p->end = ends;
	p->sizeb = sizeb;

	/* уберем превод строки в конце */
	if (name) {
		tmpc = name;
		while ((*tmpc != '\n') && (*tmpc != '\0')) tmpc++;
		*tmpc = '\0';
		len = strlen (name);
		(char *)p->Name = (char *)malloc(len+1);
		// ???
		strncat ((char *)p->Name, name, len);
		}
	else
		p->Name = NULL;

	if (to) {
		tmpc = to;
		while ((*tmpc != '\n') && (*tmpc != '\0')) tmpc++;
		*tmpc = '\0';
		len = strlen (to);
		(char *)p->To = (char *)malloc(len+1);
		// ???
		strncat ((char *)p->To, to, len);
		}
	else
		p->To = NULL;

	/* вставляем в очередь */
	if (beg) {
	/* в очереди уже есть записи */
		if (cur) {
		/* *cur не последний */
			p->next = cur;
			p->prev = cur->prev;
			cur->prev = p;
			/* *cur перед первым */
			if (cur == beg)
				beg = p;
			else
				p->prev->next = p;
			}
		else	{
		/* *cur после последнего */
			p->prev = end;
			end->next = p;
			end = p;
			};
		}
	else	{
	/* это первая запись в очереди */
		beg = p;
		end = p;
		};
	cur = p;
	return (0);
};

/*
 * Удаление вхождения по сектору
 * 0 - OK
 */
int SectorList::Del(u_int sect) {
	if (Find(sect)) {
		return (1);
		}
	else
		return (1);
};

void SectorList::PrintAll() {
Sector	*tcur;

	printf("slogic\t start\t-\tend sizes sizeb name to\n");
	for (tcur = beg; tcur; tcur = tcur->next) {
		printf("%8lu %8lu-%8lu %u %u %s %s\n",
			tcur->slogic, tcur->start, tcur->end,
			tcur->end - tcur->start + 1,
			tcur->sizeb, tcur->Name, tcur->To);
		};
};
