/*
 * Tdr.h
 */

/*
 * Заголовок файла TDR
 */
struct	tdr_head {
	u_int	startf;		/* смещение таблицы файлов */
	u_int	startd;		/* смещение таблицы каталогов */
	u_int	startpt;	/* смещение таблицы позиционирования */
	u_int	sizef;		/* размер таблицы файлов */
	u_int	sized;		/* размер таблицы каталогов */
	u_int	sizept;		/* размер таблицы позиционирования */
	u_short	format;		/* формат ленты */
	u_short	id;		/* ID ленты */
	u_short	length;		/* длина ленты в секундах */
	u_short	savelen;	/* длина записанного участка ленты */
	u_int	nextsect;	/* следующий для записи сектор */
};

/*
 * Описание добавочных полей TDR фомата.
 * добавили в версии 3.05
 */
struct	tdr_head_new {
	u_int	startdes;	/* смещение таблицы description's */
	u_int	reserv1[4];
	u_int	sizedes;	/* размер таблицы description */
	u_int	reserv2[4];
	u_int	lastsect;	/* предыдущий сектор для отката */
	u_int	reserv3[9];
};

/*
 * Описание полей таблицы файлов
 */
struct	tdr_file {
	char	name[8];	/* имя файла */
	char	ext[3];		/* расширение файла */
	char	attr;		/* атрибут файла */
	u_int	startsect;	/* начальный сектор */
	u_int	startdesc;	/* смещение описания */
	u_short	reserv1;
	u_short	dt;		/* время файла в формате DOS */
	u_short	dd;		/* дата файла в формате DOS */
	u_short	clastr;		/* не используется */
	u_int	size;		/* размер в байтах */
};

/*
 * Описание полей таблицы каталогов
 */
struct	tdr_dir {
//	u_short		a;	/* бардак */
	u_short		level;		/* уровень каталога */
#if 0
	TDR_FILES	f;		/* описание каталога */
#else
	char	name[8];	/* имя файла */
	char	ext[3];		/* расширение файла */
	char	attr;		/* атрибут файла */
#if 0
	u_int	startsect;	/* начальный сектор */
	u_int	startdesc;	/* смещение описания */
#else
	u_short	startsect[2];	/* начальный сектор */
	u_short	startdesc[2];	/* смещение описания */
#endif
	u_short	reserv1;
#if 1
	u_short	dt;		/* время файла в формате DOS */
	u_short	dd;		/* дата файла в формате DOS */
#else
	u_int	time;
#endif
	u_short	clastr;		/* не используется */
#if 0
	u_int	size;		/* размер в байтах */
#else
	u_short	size[2];		/* размер в байтах */
#endif
#endif
	u_short		startf;		/* номер первого файла */
	u_short		lastf;		/* номер последнего файла (рудимент) */
	u_short		numf;		/* число файлов */
//	u_short		b;	/* бардак */
};
#define	SIZETDRDIR	40

struct	tdr_pt {
	u_short		size;
	u_short		h[0x8d];
	u_char		l[0x1114];
};
