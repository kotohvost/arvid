#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "signal.h"

#include "Tape.h"
#include "tpb.h"
#include "rsgen.h"	// ��� _SY

#define	BEGHEAD200	148	// ��������� �����
#define	RS_NGR		149	// ����� � �����

/* extern "C" struct _SY XX; */
extern "C" int Decoder200(void *, u_char *, int, int, int);
extern "C" u_int crc32(void *, u_int);
extern "C" u_int crc32s(void *, u_int);

class TAPE {
 private:
	enum controller	arvid;		// ��� �����������
	u_int		sizecode;	// ������ code � ������
	u_char		*code;		// ��������� �� ����
	u_char		*codeoffs;	// ��������� �� ������ ������
	UHEADER		head;		// ���������
	UADD_BLOCK	uadd;		// �������������� ������ (100/200)
	u_char		fHeader;	// ������������ UHEADER'a
	u_char		cFormat;	// ������� ������
	u_char		fHead;		// ������ ������
	u_char		fDir;		// ���� � �����������
	u_char		fSpec;		// ���� � log
	//
 public:
	u_char		fTail;		// ����� ������
	// ��� ����
	CTPB		cTpb;		// ���� �� ������� �����
	struct {			// ���������� ������
		int	inkadr;
		int	insect;
		} err;
	// ������� ����������
	u_int		cSectr;		// ������� ������
	// ���������� �� ������ � ����� �������������
	u_char		stblock;	// ����� ����� � �������
 public:
	TAPE();
	~TAPE();
 void	SetController(controller, u_char);	// ���������� ��� ����������� � ������
 int	ReadKadr(int);		// ������ ������
 int	DcHeader();		// ������������ ��������� �����
 void	PrintHeader();		// ����������� ��������� �����
 int	GetSectors();		// ���������� �������� � ������� �����
 int	GetSector(int, void *);	// ������ (512b) ������ N, �
 int	Ident(int);		// ����������������� �����
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
 * ������ ������ �� fdin. �������� ���� ���� �������� sizecode.
 * 0 - �������.
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
 * ���������� ��� ����������� � ������.
 * ��������������� ����� ��� ������ ��� ��� � ������.
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
	/* ��� ... */
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
 * ������������� ��������� �����.
 * ������������� codeoffs
 * 1 - OK
 */
int TAPE::DcHeader() {
u_int	count, crc;

	if (!code)
		return (0);
	/* ��������� ������������� */
	codeoffs = code + 1;
	count = 6;
	fHeader = fHead = fDir = fTail = fSpec = 0;
	switch (cFormat) {
	case FORMAT_RS:	/* ������������� ��������� RS */
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
	default:	/* ������������� ��������� Goley */
		printf("Decode Goley Header\n");
		break;
		}; /* switch (cFormat) */
	if (fHeader) {
	/* ��������� ������ */
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
 * ������ ��������� �����.
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
 * ������������� ������� � �����
 * 0 - Ok
 */
int TAPE::GetSector(int nsec, void *buf) {

	if (!code || !fHeader)
		return(1);

	stblock = 0;

	switch(cFormat) {
	case FORMAT_RS:
		/* �������� �� ����� �� ������� */
		if ((nsec > NUM_RS_SECT) || (nsec >= head.rs.nsect))
			return(1);
		cSectr = head.rs.sector + nsec;

		/* ���������� ������ ���� */
		if (Decoder200(buf, codeoffs, nsec*SIZE_RS_BLOCK1, SIZE_RS_BLOCK1, RS_NGR)) {
			/* ��������� ������ */
			stblock = 1;
			};
		/* ���������� �������������� ������ - ������������ */
	/* �� ����� ��� nsec � �������� 0-7 :) */
		if (Decoder200(&uadd.rs[nsec>>1], codeoffs, ADD_RS_BEG+(nsec>>1), 1, RS_NGR)) {
			/* ��������� ������ */
			stblock = 2;
			};
		/* ���������� */
		/* ���� !!! */
		bcopy(uadd.rs[nsec>>1].sect[nsec&1], buf+504, 8);

		/* �� ��� �� �������� */
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
 * ������������� �����
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

	/* ��������� ���� */
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
