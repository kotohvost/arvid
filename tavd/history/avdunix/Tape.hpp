#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "signal.h"

#include "Tape.h"
#include "tpb.h"
#include "rsgen.h"	// для _SY

#define	BEGHEAD200	148	// заголовок кадра
#define	RS_NGR		149	// групп в кадре

/* extern "C" struct _SY XX; */
extern "C" int Decoder200(void *, u_char *, int, int, int);
extern "C" u_int crc32(void *, u_int);
extern "C" u_int crc32s(void *, u_int);

class TAPE {
 private:
	enum controller	arvid;		// тип контроллера
	u_int		sizecode;	// размер code в байтах
	u_char		*code;		// указатель на кадр
	u_char		*codeoffs;	// указатель на начало данных
	UHEADER		head;		// заголовок
	UADD_BLOCK	uadd;		// дополнительные группы (100/200)
	u_char		fHeader;	// правильность UHEADER'a
	u_char		cFormat;	// текущий формат
	u_char		fHead;		// начало записи
	u_char		fDir;		// кадр с оглавлением
	u_char		fSpec;		// кадр с log
	//
 public:
	u_char		fTail;		// конец записи
	// для всех
	CTPB		cTpb;		// инфа по текущай ленте
	struct {			// статистика ошибок
		int	inkadr;
		int	insect;
		} err;
	// частные переменные
	u_int		cSectr;		// текущий сектор
	// статистика по ошибка в блоке декодирования
	u_char		stblock;	// номер блока с ошибкой
 public:
	TAPE();
	~TAPE();
 void	SetController(controller, u_char);	// установить тип контроллера и формат
 int	ReadKadr(int);		// читать данные
 int	DcHeader();		// декодировать заголовок кадра
 void	PrintHeader();		// распечатать заголовок кадра
 int	GetSectors();		// количество секторов в текущем кадре
 int	GetSector(int, void *);	// читаем (512b) сектор N, в
 int	Ident(int);		// индентифицировать ленту
};

TAPE::TAPE() {
	code = codeoffs = NULL;
	arvid = UNKNOWN;
	fHeader = fHead = fDir = fTail = fSpec = 0;
	cFormat = FORMAT_RS;
	cSectr = 0;
	bzero(&cTpb, sizeof(CTPB));
};

TAPE::~TAPE() {
	if (code)
		free(code);
	// code = NULL;
};

/*
 * Чтение данных из fdin. Читается один кадр размером sizecode.
 * 0 - успешно.
 */
int TAPE::ReadKadr(int fdin) {
int	count;

	if (!code)
		return(1);
	count = read(fdin, code, sizecode);
	if (count == sizecode)
		return(0);
//	printf("Read: %d\n", count);
	return(1);
};

/*
 * Установить тип контроллера и формат.
 * зарезервировать буфер для чтения под тип и формат.
 */
void TAPE::SetController(controller av, u_char frmt) {

	arvid = av;
	switch (av) {
	case	Av1010:	sizecode = SIZE100;
			cFormat = frmt & (FORMAT_K4 | FORMAT_K7 | FORMAT_K7CRC);
			break;
	case	Av1020:
	case	Av1030:
	case	Av1031:	sizecode = SIZE200;
			cFormat = frmt & (FORMAT_K4 | FORMAT_K7
					| FORMAT_K7CRC | FORMAT_RS
					| FORMAT_RSC);
			break;
	case	Av1051:
	case	Av1052:	cFormat = frmt & (FORMAT_K4 | FORMAT_K7
					| FORMAT_K7CRC | FORMAT_RS
					| FORMAT_RSC | FORMAT_RSC_50);
			if (cFormat == FORMAT_RSC_50)
				sizecode = SIZE325;
			else
				sizecode = SIZE200;
			break;
	/* для ... */
	default:	arvid = Av1010;
			sizecode = SIZE100;
		}; /* switch (cFormat) */
	if (code) {
		free(code);
		code = NULL;
		};
	if (!code)
		code = (u_char *) malloc(sizecode);
};

/*
 * Декодирование заголовка кадра.
 * устанавливает codeoffs
 * 1 - OK
 */
int TAPE::DcHeader() {
u_int	count, crc;

	if (!code)
		return (0);
	/* первичная инициализация */
	codeoffs = code + 1;
	count = 6;
	fHeader = fHead = fDir = fTail = fSpec = 0;
	switch (cFormat) {
	case FORMAT_RS:	/* декодирование заголовка RS */
//		printf("Decode RS Header\n");
		while (count) {
			if (!Decoder200(&head.rs.format, codeoffs, BEGHEAD200, 1, RS_NGR)) {
				if ((head.rs.length) && (head.rs.tapetime)) {
					crc = crc32(&head.rs.format, (sizeof(HEADER200) - 4) / 2);
					if (crc == head.rs.tstsum) {
		//				printf("OK! %d\n", count);
						fHeader = 1;
						count = 0;
						};
					};
				};
			if (count) {
				codeoffs += SIZESTR200;
				count--;
				};
			}; /* while (count) */
		break;
	default:	/* декодирование заголовка Goley */
		printf("Decode Goley Header\n");
		break;
		}; /* switch (cFormat) */
	if (fHeader) {
	/* установка флагов */
		if (head.g.length & HEAD_ATTR)
			fHead = 1;
		if (head.g.length & TAIL_ATTR)
			fTail = 1;
		if (head.g.length & SPEC_ATTR)
			fSpec = 1;
		head.g.length &= LENGTHMASK;
		if (head.g.sector & DIR_MARK)
			fDir = 1;
		head.g.sector &= SECTORMASK;
		cSectr = head.g.sector;
		};
	return(fHeader);
};

/*
 * Печать заголовка кадра.
 */
void TAPE::PrintHeader() {
int	tmp1;
char	h, m, s;

	printf("Tape name: ");
	for(tmp1=0; tmp1<8; tmp1++)
		printf("%c", head.g.name[tmp1]);
	printf("\n");
	printf("Format: %02X", head.g.format);
	switch(head.g.format) {
	case	FORMAT_K7CRC:	printf("\tFORMAT_K7CRC(100K/s CRC32"); break;
	case	FORMAT_RS:	printf("\tFORMAT_RS(200K/s)"); break;
		};
	printf("\n");
	printf("Nsect:  %d\n", head.rs.nsect);
	printf("Ident:  %02X\n", head.g.ident);
	printf("Length: %u\n", head.g.length);
	h = head.g.tapetime/3600;
	m = (head.g.tapetime - (h * 3600))/60;
	s = head.g.tapetime - (h * 3600) - (m * 60);
	printf("Time:   %us\t(%02d:%02d:%02d)\n", head.g.tapetime,
		h, m, s);
	printf("Sector: %lu\n", head.g.sector);
};

/*
 *
 */
int TAPE::GetSectors() {
	if (!code || !fHeader)
		return(0);
	switch(cFormat) {
	case FORMAT_RS:
		return(head.rs.nsect);
		};
	return(0);
};

/*
 * декодирование сектора в буфер
 * 0 - Ok
 */
int TAPE::GetSector(int nsec, void *buf) {

	if (!code || !fHeader)
		return(1);

	stblock = 0;

	switch(cFormat) {
	case FORMAT_RS:
		/* проверим на выход за пределы */
		if ((nsec > NUM_RS_SECT) || (nsec >= head.rs.nsect))
			return(1);
		cSectr = head.rs.sector + nsec;

		/* декодируем первый блок */
		if (Decoder200(buf, codeoffs, nsec*SIZE_RS_BLOCK1, SIZE_RS_BLOCK1, RS_NGR)) {
			/* фатальная ошибка */
			stblock = 1;
			};
		/* декодируем дополнительную группу - НЕОПТИМАЛЬНО */
	/* мы знаем что nsec в пределах 0-7 :) */
		if (Decoder200(&uadd.rs[nsec>>1], codeoffs, ADD_RS_BEG+(nsec>>1), 1, RS_NGR)) {
			/* фатальная ошибка */
			stblock = 2;
			};
		/* дописываем */
		/* блин !!! */
		bcopy(uadd.rs[nsec>>1].sect[nsec&1], buf+504, 8);

		/* ща еще КС проверим */
		if (stblock) {
			return(1);
			};
		if (crc32s(buf, 512/2) == uadd.rs[nsec>>1].csum[nsec&1]) {
			return(0);
			};
		break;
		};
	return(1);
};

/*
 * идентификация ленты
 * 0 - error
 */
int TAPE::Ident(int fd) {
int	cnt;

	while (ReadKadr(fd));

	if (!DcHeader()) {
		err.inkadr++;
	//	printf("Error decode header\n");
		return(0);
		}
	else	{
		PrintHeader();
	//	printf("Errors: %d\n", errinkadr);
		}

	/* переносим инфу */
	switch(cFormat) {
	case FORMAT_RS:
		if (cTpb.name)
			free(cTpb.name);
		cTpb.name = (char *)malloc(9);
		cnt = 0;
		while ((cnt < 8) && !isblank(cTpb.name[cnt])) {
			cTpb.name[cnt] = head.rs.name[cnt];
			cnt++;
			};
		cnt++;
		cTpb.name[cnt] = '\0';
		cTpb.id = head.rs.ident;
		cTpb.format = head.rs.format;
		cTpb.length = head.rs.length;
		cTpb.time = head.rs.tapetime;
		cTpb.sector = head.rs.sector;
		cnt = 1;
		break;
	default:
		bzero(&cTpb, sizeof(CTPB));
		cnt = 0;
		break;
		};
	printf("Error kadr\'s: %u\n", err.inkadr);
	return(cnt);
};
