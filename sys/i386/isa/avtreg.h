/*
 * Открыт Владимиром Зарожевским, по исходнику arvid10.exe Ver. 3.46r4
 * и файлу arvidapi.h пробегавшим по эхе SU.HARDW.SUPPORT.ARVID
 *	Файл-описание интерфейса АрВид V0.01beta (Arvid API)
 *  Содержит:
 *	- символическое описание кодов завершения
 *	- описание используемых структур данных
 *
 * составлен для Arvid 1031
 */

/* Про железо */
/*
 * Описание констант и структур используемых в программах работы с Арвид-1000
 */
#define	SIZESTR100	 8			/* длина строки 100 K/s */
#define	SIZE100		256*SIZESTR100*2	/* размер кадра 100 K/s */

#define	SIZESTR200	 9			/* длина строки 200 K/s */
#define	SIZE200		284*SIZESTR200*2	/* размер кадра 200 K/s */

#define	SIZESTR325	13			/* длина строки 325 K/s */
#define	SIZE325		296*SIZESTR325*2	/* размер кадра 325 K/s */

#define SIZE1030	4096		/* размер кадра ОЗУ карты в словах */

/* Все порты используемые устройством 16-ти разрядные
 * ArvidPort	= 270h | 320h
 *
 * чтение/запись
 */
#define	AV30_RDINC(base)	((base) + 0)
#define	AV30_RD(base)		((base) + 2)
/*
 * для записи
 */
#define AV30_RK(base)		((base) + 4)
#define AV30_RA(base)		((base) + 6)
/*
 * для чтение
 */
#define	AV30_RS(base)		((base) + 4)
#define AV30_RS1(base)		((base) + 6)
#define AV30_NPORT		2*4	/* количество используемых портов */

/*
 * Ниже описывается _как_ записаны данные на ленте для АРВИДа 1010
 *
 * перед записью на ленту формируется блок из 1032 слов в следующем порядке:
 * ----------------------------------------------
 * |	первый сектор - [512-8] байт		|
 * ----------------------------------------------
 * |	второй сектор - [512-8] байт		|
 * ----------------------------------------------
 * |	третий сектор - [512-8] байт		|
 * ----------------------------------------------
 * |	четвертый сектор - [512-8] байт		|
 * ----------------------------------------------
 * |	8 байт - из первого сектора		|
 * ----------------------------------------------
 * |	8 байт - из второго сектора		|
 * ----------------------------------------------
 * |	8 байт - контрольная сумма этих 16 байт	| точнее Xor16
 * ----------------------------------------------
 * |	8 байт - из третьего сектора		|
 * ----------------------------------------------
 * |	8 байт - из чевертого сектора		|
 * ----------------------------------------------
 * |	8 байт - контрольная сумма		|
 * ----------------------------------------------
 */
struct read_data {
	u_long	sect0[63];
	u_long	sect1[63];
	u_long	sect2[63];
	u_long	sect3[63];

	u_long	sect0e;
	u_long	sect1e;
	u_int	resrv0;
	u_short	crcS0S1;
	u_short	resrv0e;

	u_long	sect2e;
	u_long	sect3e;
	u_int	resrv1;
	u_short	crcS2S3;
	u_short	resrv1e;
};
/*
 * дальше идет структура TPB на ленте 24 байта
 */
typedef struct {
	u_char	TapeFormat;	/* формат ленты */
	u_char	TapeStartTPB;	/* контрольный байт */
#define	StartTPB	0x33	/* */

	u_char	TapeName[8];	/* имя ленты */
	u_short	TapeID;		/* уникальный индентификатор ленты */
	u_short	TapeTime;	/* длина ленты в минутах */

	u_short	Tapetime;	/* текущее время в секундах */
	u_char	TapeSectr[3];	/* номер певого? сектора в блоке */
#define	SECTRNUM(a,b,c)	(a + (b << 8) + (c << 16))
	u_char	TapeFlag;	/* флаг блока */
#define BLOCKDATA	0	/* данные в блоке относятся к файлам */
#define BLOCKTDR	0x80	/* данные в блоке относятся к TDR */
	u_char	TapeNsector;	/* количество секторов в блоке? */
#define	EndTPB		!StartTPB
	u_char	TapeEndTPB;	/* контрольный байт */
	u_short	TapeCRC;	/* контрольная сумма TPB */
} TPBInfo;

/*
 * потом идет контрольная сумма каждого сектора
 * и в конце их общая контрольная сумма, это тоже 24 байта
 */
typedef struct {
	u_int	block0;
	u_int	block1;
	u_int	block2;
	u_int	block3;
	u_int	reserv;
	u_int	crc;
} TBcrc;

/* общая структура на один кадр */

#define SizeD100	1032	/* данные в словах (SizeData 100 Kb/s) */

typedef struct {
	u_short	data[SizeD100];	/* данные в словах */
	TPBInfo	TPB;		/* структура ленты */
	TBcrc	CRC;		/* инфо о данных */
} avt_kadr100;

#define SizeK100	1056

/*
 * Итого мы получаем ровно 1056 слов в одном кадре. После этого
 * кадр кодируется XOR преобразованием из 24 -> 66. Получается
 * чуть меньше размера ОЗУ карты. Дело в том что нерекомендуется
 * использовать первую строку данных (для 1010 = 34 байта)
 * Это связанно с настройкой аппаратуры на ленту. 
 */

/*
 * это внутренние структуры
 */

/*
 * коды завершения, возвращаемые функциями API

#define OK_VCR  0               // успех
enum{
	E_OK=0,                 // успех
	E_DRIVER_NOT_INSTALLED=1,      // драйвер не установлен
	E_DEVICE_INVALID_OR_ALREADY_EXIST=2,// неверный AVHANDLE
	E_SYSTEM_ERROR=3,       // произошла системная ошибка ( Windows: код в GetLastError() )
	E_NOT_TPB=4,            // не определен TPB
	E_TPB_EXIST=5,          // TPB уже определен
	E_NO_FUNCTION=6,        // функция или команда не поддерживается (неверный номер)
	E_UNKNOWN_FORMAT=7,     // считан неизвестный формат ленты
	E_NOT_FOUND_TAPE_DIR=8, // оглавление ленты не найдено
	E_NOT_AVAIL_FORMAT=9,   // неподдерживаемый формат ленты для записи
	E_NOT_POSITION=10,      // сектор не найден (тайм-аут истек)
	E_NOT_IDENTIC_TPB=11,   // несовпадение TPB (наткнулись не на ту ленту)
	E__ABORT=12,            // команда прервана пользователем
	E_OLD_DIR=13,           // оглавление устарело, найден сектор "нашей" ленты
				// после сектора, с которого задано начало записи
				// в режиме контроля оглавления
	E_NOT_DIR=14,           // не найдено оглавление ленты при записи
				// в режиме контроля оглавления
	E_ABORT_DISABLE=15,     // аборт в данный момент запрещен
	E_ABORT_RECORD=16,      // аборт,  когда ВМ был в записи
	E_TRANSL_RECORD=17,     // ошибка контроля "кольца" в режиме REC
	E_NO_IRC_RCV=18,        // не было приема ИК клавиши (для команды ввода ИК)
	// ддалее коды возврата AvdCheckStop
	OFF_VCR=19,             // ВМ вероятно выключен
	TUNER_VCR=20,           // ВМ либо формирует свое изображение на экране TV, либо
				// принимает сигнал с эфира
	PICTURE_VCR=21,         // аналогично, но видео транслируется частично
	REC_OK_VCR=PICTURE_VCR,   // зарезервировано

	E_TAPE_FULL=22,         // раннее предупреждение о конце ленты
				// при записи (выдается за 1 мин. до конца)
				// выдается при записи
	E_FIND_DIR=23,          // при поиске сектора со спец. аттрибутом
				// обнаружено оглавление ленты
	E_INVALID_PARAM=24,     // неверные параметры в команде
        E_INVALID_SEQUENCE=25,  // неверная последовательность команд (чтения или записи)
	E_REACH_END_TAPE=26,    // достигнута метка конца ленты (при чтении)
	E_ERROR_OR_SKIP_DATA=27,// не все данные команды чтения считаны
	E_BAD_VCR_TABLE=28,     // загружается неверная таблица ВМ
	E_INCOMPATIBLE_LOW_LEVEL_DRIVER=29, // несовместимый драйвер драйвер нижнего уровня ( .VxD or .SYS )
	E_UNKNOWN_FORMAT_PT=30, // неизвестный формат таблицы позиционирования
	E_BAD_PT=31,            // неверная таблица позиционирования (ТП)
	E_MISMATCH_FORMAT_PT=32,// несоответствие форматов ленты и ТП
	E_INTERNAL_DRIVE_ERROR=33, // internal drive error
        E_SECTOR_NOT_ACCESSIBLE=34,// сектор для поиска недоступен( истек счетчик откатов )
	E_UNKNOWN_ERROR         // неизвестная ошибка, если >= E_UNKNOWN_ERROR
};

// константы для функции узнать AvdIsTape
#define NOT_TAPE        0       // нет ленты
#define TPB_TAPE        1       // параметры ленты загружены


// режимы перемещения ленты ВМ ( состояния ВМ )
enum {
   VCR_STOP=0           ,          // состояние STOP
   VCR_PLAY             ,          // состояние PLAY
   VCR_PLAY_FF          ,          // ускоренное воспроизведение вперед
   VCR_PLAY_REW         ,          // ускоренное воспроизведение назад
   VCR_nPLAY_FF         ,          // зарезервировано, ускоренное воспроизведение вперед (вторая скорость)
   VCR_nPLAY_REW        ,          // зарезервировано, ускоренное воспроизведение назад  (вторая скорость)
   VCR_FF               ,          // ускоренная перемотка вперед
   VCR_REW              ,          // ускоренная перемотка назад
   VCR_REC              ,          // режим записи на ленту
   VCR_MAX= VCR_REC                // макс значение режима
};



#define VER_PROTO_API   0x103   //

// далее следуют определение структур данных, используемых API

 *	1. Блок параметров драйвера Арвид
 * определяет основные характеристики, постоянные для данной версии драйвера
 */

/* биты аттрибутов (в поле attributs) */

#define USED_AUTO_PHASE 0x1	/* драйвер поддерживает автофазирование */
#define USED_COMPRESS   0x2	/* драйвер поддерживает ленты со сжатием */
#define USED_XOR        0x4	/* драйвер поддерживает избыточные кадры XOR */

enum controller {
	UNKNOWN	= 0,	/* unknown type, driver disabled */
	Av1010	= 1010,	/* 1010 - 100 Kb/s (180min = 1Gb) */
	Av1020	= 1020,	/* 1020 - 200 Kb/s (180min = 2Gb) */
	Av1030	= 1030,	/* 1030 - 200 Kb/s */
	Av1031	= 1031,	/* 1031 - 200 Kb/s, 32 Kw, (300 Kb/s with compression) */
	Av1051	= 1051,	/* 1051 - 325 Kb/s, 64 Kw, (180min = 3.3Gb) */
	Av1052	= 1052,	/* 1052 - 325 Kb/s, 256 Kw, -//--//- */
};

struct avt_info {		/* Driver Parameters Block */
/* Неизменяемые характеристики в процессе работы */
	enum controller arvid;	/* тип контроллера */
	u_short	port;			/* номер базового порта */
	u_short	irq;			/* прерывание */
	u_short	drq;			/* DMA канал */
	u_char	ver_major,
		ver_minor;		/* версия драйвера */
	u_short	maxrate;	/* макс. скорость передачи [100-200] Кбайт/cек */
#define	MAXRATE10	100	/* 100 Kb/s */
#define	MAXRATE20	200	/* 200 Kb/s */
#define	MAXRATE30	200	/* 200 Kb/s */
#define	MAXRATE50	325	/* 325 Kb/s */
	u_char	maxphase;	/* макс. величина параметра phase */
#define MAXPHASE10	3	/* для 1010 (0-3) */
#define MAXPHASE20	7	/* для 1020 (0-7) */
#define MAXPHASE30	15	/* для 103x (0-7 + A0-A7) */
#define MAXPHASE50	23	/* для 103x (0-7 + A0-A7 + B0-B7) */

/*	2. Блок режимов драйвера Арвид
 * определяет характеристики, которые могут изменяться для указанного Device
 */
/*     struct CDPB {           // Current Device Parameters Block */
	u_short	flags;		/* статус драйвера */
#define	VTCLOSE	0x0000		/* драйвер свободен */
#define	VTOPEN	0x0001		/* драйвер используется */
#define	VCRLOAD	0x0002		/* флаг загрузки VCR таблицы */
#define	VTBUSY	0x8000		/* драйвер работает */
	u_char	commands;	/* текущая команда */
	u_char	time_out;	/* время тайм-аута, секунд, т.е. время,
				   по истечении которого операция прекращается,
				   если на ленте не найдено никакой информации
				   (по умолчанию 13 секунд) ??? */
	u_short	rate;		/* скорость передачи [100-200] Кбайт/cек */
	u_char	phase;		/* текущая фаза приема */
#define DEFAULT_PHASE10	3
#define DEFAULT_PHASE20	3
#define DEFAULT_PHASE30 0
#define DEFAULT_PHASE50 1
	u_char	check_rec;	/* флаг контроля кольца в реж. Record */
/*	u_short	lp; */		/* флаг работы в режиме LongPlay
				   ( д.б. 0 ) reserved
				   и ваще должен быть в структуре видика */
	u_char	auto_phase;	/* флаг: текущая работа в режиме
				   автоподстройки фазы */
/* ONLY 1030 and above */
/* #define AUTOTUNON	0000100B */
/* #define AUTOTUNOFF	!AUTOTUNON */
/*	U8      compress_enable; // флаг: при записи данных в формате FORMAT_RSC
				 // сжатие разрешено
	U8      xor_enable;  //  разрешено формировать XOR блоки при записи
	U8      xor_block_size;   // кол-во кадров в XOR блоке при записи
	U8      xor_block_number; // кол-во блоков в XOR области при записи
	U32     reserv;     // reserv
*/
};
typedef	struct	avt_info avt_info_t;

/*
 *	3. Блок параметров ленты:
 *
 * определяет характеристики ленты, с которой сейчас работает Арвид
 */
struct TPB {
	u_short	format;		/* формат ленты */
	char	name[8];	/* имя ленты */
	u_int	ident;		/* идентификатор ленты (уникальный номер) */
	u_short	length;		/* длина ленты в минутах */
	u_short	time;		/* текущее время на ленте */
	u_int	sector;		/* текущий сектор на ленте */
	u_char	Nsector;	/* количество секторов в кадре на ленте */
	u_int	flags;		/* флаги */
};

#if 0
     struct TPBNEW {       // Tape Parameters Block для формата FORMAT_RSC
	U16     format;		// формат ленты
	U16     length;		// длина ленты в минутах
	U32     tapeID;		// имя ленты ( идентификатор-уникальный номер )
	U8      reserv[sizeof(TPBOLD)-8];     // reserv
     };

     struct TPB {	// общая структура TPB
	U16	format;
	U16	length;
	union {
	  struct {
	    char	name[8];
	    U16		ident;
		U16		resrv;
	    U32		reserv;
	  };
	  U32	tapeID;
	};
     };

//     4. Блок текущих параметров устройства при работе с лентой
// может использоваться при отображении текущего состояния
     struct CurTPB {       // Current Tape Parameters Block
	U16     cur_time;  // текущее время от начала ленты, сек
	U16     targ_time; // время цели, сек
	U32     cur_sect;  // текущий сектор
	U32     targ_sect; // сектор цели
	U16     state_vcr; // текущее состояние ВМ (константы см выше)
	// далее идет часть, используемая AVSETUP
	U32     pure_st;   // кол-во чистых кадров ( без ошибок )
			   // от начала операции чтения
        U32     reserv[4];     // reserv
     };

//      5. Блок аппаратных установок устройства
     struct DHB {       // Device Hardware parameters Block
	U16     port;   // базовый порт
	U16     dma;    // номер канала DMA
	U16     irq;    // номер канала IRQ
	U16     busy;   // 0, если может быть открыт AvdOpen
	U8      reserv[8];
     };

// описание типа: сектор данных
#define SIZE_SECTOR_BYTES       512    // размер сектора для ArvidApi
#define SIZE_SECTOR_WORDS       (SIZE_SECTOR_BYTES/2)
#define SIZE_SECTOR_DWORDS      (SIZE_SECTOR_BYTES/4)
typedef char SectData[SIZE_SECTOR_BYTES];
typedef SectData _FAR_ *SectDataBlock; // указатель на сектора
#endif /* if 0 */

/*
 *======================================================
 * пытаемся создать кольцевой буфер для чтения данных из
 * карты, это если мы не успеваем их отдавать, в конце
 * концов будет найден единственный для использования.
 *======================================================
 */
#define	SIZEKADR	296 * 13	/* максимальное количество СЛОВ
					   данных в одном кадре 1052 */
struct buf_raw {
	struct buf_raw	*next;	/* указатель на следующий буфер */
	u_short	data[SIZEKADR];	/* данные из карты */
};
#define	NBUFRAW	20		/* количество буферов */

#define	SIZE_VCR	0x996	/* размер VCR таблицы */

struct	set_rw {
	u_int	rate;		/* скорость чтения: 100/200/325 */
#define	IORAW	0x0
#define	IODATA	0x1
#define	IOIRC	0x2
	u_char	flag;		/* RAW/IRC/DATA */
};
/* ioctl функции */
#define	VGET_INFO	_IOR('A',  1, struct avt_info)	/* читаем конфиг драйвера */
#define	VSET_READ	_IOW('A',  2, struct set_rw)	/* читаем данные */
#define	VSET_WRITE	_IOW('A',  3, struct set_rw)	/* пишем данные */
#define	VGET_PHASE	_IOR('A',  4, unsigned char)	/* читаем фазу чтения */
#define	VSET_PHASE	_IOW('A',  5, unsigned char)	/* установка фазы чтения */
