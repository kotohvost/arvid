/*
 * tdr.h
 */
#if 0
/* For DOS */
typedef	unsigned char	BYTE;
typedef	unsigned int	WORD;
typedef	unsigned long	DWORD;
#else
/* For UNIX */
typedef	unsigned char	BYTE;
typedef	unsigned short	WORD;
typedef	unsigned int	DWORD;
#endif

/* Заголовок файла TDR */
typedef struct {
	DWORD	startf;		/* смещение таблицы файлов */
	DWORD	startd;		/* смещение таблицы каталогов */
	DWORD	startpt;	/* смещение таблицы позиционирования */
	DWORD	sizef;		/* размер таблицы файлов */
	DWORD	sized;		/* размер таблицы каталогов */
	DWORD	sizept;		/* размер таблицы позиционирования */
	WORD	format;		/* формат ленты */
	WORD	ID;		/* ID ленты */
	WORD	length;		/* длина ленты в секундах */
	WORD	savelen;	/* длина записанного участка ленты */
	DWORD	nextsect;	/* следующий для записи сектор */
} TDR_HEAD;

/* Описание добавочных полей TDR фомата */
typedef struct {
	DWORD	startdes;	/* смещение таблицы description's */
	DWORD	reserv1a;	/*  */
	DWORD	reserv2a;	/*  */
	DWORD	reserv3a;
	DWORD	reserv4a;
	DWORD	sizedes;	/* размер таблицы description */
	DWORD	reserv1b;	/*  */
	DWORD	reserv2b;	/*  */
	DWORD	reserv3b;
	DWORD	reserv4b;
	DWORD	lastsect;	/* предыдущий сектор для отката */
	DWORD	reserv[9];
} TDR_ADD_HEAD;

/* Описание полей таблицы файлов */
typedef	struct {
	BYTE	name[8];	/* имя файла */
	BYTE	ext[3];		/* расширение файла */
	BYTE	attr;		/* атрибут файла */
	DWORD	startsect;	/* начальный сектор */
	DWORD	startdesc;	/* смещение описания */
	WORD	reserv1;
	WORD	time;		/* время файла в формате DOS */
	WORD	date;		/* дата файла в формате DOS */
	WORD	clastr;		/* не используется */
	DWORD	size;		/* размер в байтах */
} TDR_FILES;
typedef	struct {
	BYTE	name[8];	/* имя файла */
	BYTE	ext[3];		/* расширение файла */
	BYTE	attr;		/* атрибут файла */
	BYTE	startsect[4];	/* начальный сектор */
	BYTE	startdesc[4];	/* смещение описания */
	BYTE	reserv1[2];
	BYTE	time[2];	/* время файла в формате DOS */
	BYTE	date[2];	/* дата файла в формате DOS */
	BYTE	clastr[2];	/* не используется */
	BYTE	size[4];	/* размер в байтах */
} TDR_FILESb;

/* Описание полей таблицы каталогов */
typedef	struct {
	WORD		level;		/* уровень каталога */
	TDR_FILES	d;		/* описание каталога */
	WORD		startf;		/* номер первого файла */
	WORD		lastf;		/* номер последнего файла (рудимент) */
	WORD		numf;		/* число файлов */
} TDR_DIR;
typedef	struct {
	BYTE		level[2];
	TDR_FILESb	d;
	BYTE		startf[2];
	BYTE		lastf[2];
	BYTE		numf[2];
} TDR_DIRb;
