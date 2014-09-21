/*
 * Класс используемый для работы с таблицой управления ВМ.
 */
#ifndef IRC
#define IRC

/*
 * Структура VCR файла
 */
struct vcrfile {
	unsigned char	a[0];	/* пока для совместимости */
	int	major, minor;	/* +8 я думаю это Version Id = 2.0*/
	unsigned char	b[0x10];
	unsigned char	PowerOn[32];	/* PowerOn */
	struct	To {
		unsigned char	key[6];
	};
	struct	To	Sto[10];	/* Stop to x */
	struct	To	Pto[10];	/* Play to x */
	struct	From {
		unsigned char	st[6];
		unsigned char	pl[6];
	};
	struct	From	Tox[10];	/* From x to Stop/Play */
	unsigned char	z[0x5c];
	unsigned short	rep;		/* +184 наличие репетитора */
	unsigned short	nrep;		/* кол-во повторов репетитора */
	unsigned short	nkey;		/* кол-во использ. клавиш */
	unsigned char	key[50];	/* +18A указатели на номер буфера кнопки */
	unsigned short	period;		/* +1BC период сигнала в мкс */
	struct	type {
		unsigned short	paket[11];
		unsigned short	pause[11];
	}	type;	/* +1BE */
	unsigned char	z1[78];		/* +1EA */
	unsigned short	mod;		/* 238 */
	unsigned char	pack[0x10][0x68];
	unsigned short	tstsum;		/* 8BA CRC16 */
	unsigned short	tstsum1;
	struct	SPLP {
		unsigned short	keytokey;	/* пауза между клавишами (кадр) */
		unsigned short	speedFF;	/* угловая скорость ленты */
		unsigned short	speedREW;
		unsigned short	cPLAY_FF;	/* коэфициенты скорости в PLAY */
		unsigned short	cPLAY_REW;
		unsigned short	cnPLAY_FF;
		unsigned short	cnPLAY_REW;
		unsigned short	delayFF;	/* задержки переходов (кадр) */
		unsigned short	delayREW;
		unsigned short	skipPLAY_FF;	/* смещение цели */
		unsigned short	skipPLAY_REW;
	} SPLP;
	unsigned char	z2[410];
	char	kname[16][10];	/* 2670 */
};

class VCR {
 public:
	vcrfile	*svcr;		// указатель на таблицу управления ВМ
	int	keynums;	// количество клавиш
	int	curmode;	// текущее состояние ВМ
	/* используется при распаковке */
	unsigned short	*skey;		// указатель на буфер для последовательности
	int	word_23e;	// непонятно (???)
	int	IsRepet;	// флаг наличия репетитора
	int	vcr_2;		// +02h ??
	unsigned short	*Sbuffer;	// +06h буфер кадра
	int	modesize;	// +08h разм. кадра Arvid'a для ровного кол. периодов
	int	kadrsize;	// +0Ah разм. кадра Arvid'a (4096)
	int	numkadr;	// +0Eh	...
	int	fEndKey;	// +12h конец последовательности
	int	pause;		// +14h флаг сигнала паузы
	unsigned short	*Sbufkod;	// +16h еще один адрес буфера
	unsigned short	*Nbufkod;	// +1Ah указатель на следующий код
	unsigned short	kodn;		// +1Eh код для преобразования
	int	sizeper;	// +20h длина одного периода в битах
	int	bufbit;		// +22h размер modesize в битах
	int	startflag;	// +26h старт
	int	bit;		// +28h текущий бит в последнем слове
	unsigned short	*bufkadr;	// +2Ah буфер кадра
	int	vcr_2C;		// +2Ch ??
	int	fStartP;	// +2Eh первой выводится пауза
	int	vcr_30;		// +30h ??
	int	vcr_32;		// +32h ??
	int	vcr_34;		// +34h ?????? неиспользуется(?)
 public:
	VCR();			// constructor
	~VCR();			// destructor
 int	LoadVCR(char *);	// загрузка из файла
 int	KeyNum();		// количество кнопок в VCR'e
 int	Uncompress (int);	// заполнение последовательности кодов
 int	FindRepetitor(int);	// наличие репетитора у клавиши
 int	VCR_1950();		// распаковка сигналов
 void	IRCtoPause(int, int, unsigned short, unsigned short *);
 void	IRCtoMod(int, int, unsigned short, unsigned short *);
 void	IRCtoNMod(int, int, unsigned short, unsigned short *);
 void	CorrectBufBit(int bits);
	/* управление ВМ с отслеживанием текущего состояния */
 int	SetMode(int, int);
 int	SetMode(int, char *);
	/* управление ВМ */
 int	IrcKeyTrn(int, int);
 int	IrcKeyTrn(int, char *);
	/* перевод символьного имени клавиши в цифровое */
 int	NameToNum(char *);
	/* перемещать ленту вперед/назад в секундах */
 int	Move(int, int);
};

#endif
