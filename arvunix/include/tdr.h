/*
 * tdr.h
 */

/*
 * Заголовок файла TDR
 */
struct	TDR_HEAD {
	unsigned int	startf;		/* смещение таблицы файлов */
	unsigned int	startd;		/* смещение таблицы каталогов */
	unsigned int	startpt;	/* смещение таблицы позиционирования */
	unsigned int	sizef;		/* размер таблицы файлов */
	unsigned int	sized;		/* размер таблицы каталогов */
	unsigned int	sizept;		/* размер таблицы позиционирования */
	unsigned short	format;		/* формат ленты */
	unsigned short	ID;		/* ID ленты */
	unsigned short	length;		/* длина ленты в секундах */
	unsigned short	savelen;	/* длина записанного участка ленты */
	unsigned int	nextsect;	/* следующий для записи сектор */
};

/*
 * Описание добавочных полей TDR фомата.
 * добавили в версии 3.05
 */
struct	TDR_ADD_HEAD {
	unsigned int	startdes;	/* смещение таблицы description's */
	unsigned int	reserv1a;	/*  */
	unsigned int	reserv2a;	/*  */
	unsigned int	reserv3a;
	unsigned int	reserv4a;
	unsigned int	sizedes;	/* размер таблицы description */
	unsigned int	reserv1b;	/*  */
	unsigned int	reserv2b;	/*  */
	unsigned int	reserv3b;
	unsigned int	reserv4b;
	unsigned int	lastsect;	/* предыдущий сектор для отката */
	unsigned int	reserv[9];
};

/*
 * Описание полей таблицы файлов
 */
struct	TDR_FILE {
	char	name[8];	/* имя файла */
	char	ext[3];		/* расширение файла */
	char	attr;		/* атрибут файла */
	unsigned int	startsect;	/* начальный сектор */
	unsigned int	startdesc;	/* смещение описания */
	unsigned short	reserv1;
	unsigned short	dt;		/* время файла в формате DOS */
	unsigned short	dd;		/* дата файла в формате DOS */
	unsigned short	clastr;		/* не используется */
	unsigned int	size;		/* размер в байтах */
};

/*
 * Описание полей таблицы каталогов
 */
struct	TDR_DIR {
//	unsigned short		a;	/* бардак */
	unsigned short		level;		/* уровень каталога */
#if 0
	TDR_FILES	f;		/* описание каталога */
#else
	char	name[8];	/* имя файла */
	char	ext[3];		/* расширение файла */
	char	attr;		/* атрибут файла */
#if 0
	unsigned int	startsect;	/* начальный сектор */
	unsigned int	startdesc;	/* смещение описания */
#else
	unsigned short	startsect[2];	/* начальный сектор */
	unsigned short	startdesc[2];	/* смещение описания */
#endif
	unsigned short	reserv1;
#if 1
	unsigned short	dt;		/* время файла в формате DOS */
	unsigned short	dd;		/* дата файла в формате DOS */
#else
	unsigned int	time;
#endif
	unsigned short	clastr;		/* не используется */
#if 0
	unsigned int	size;		/* размер в байтах */
#else
	unsigned short	size[2];		/* размер в байтах */
#endif
#endif
	unsigned short		startf;		/* номер первого файла */
	unsigned short		lastf;		/* номер последнего файла (рудимент) */
	unsigned short		numf;		/* число файлов */
//	unsigned short		b;	/* бардак */
};
#define	SIZETDRDIR	40
