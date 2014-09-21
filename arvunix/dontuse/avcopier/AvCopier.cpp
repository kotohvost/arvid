/*
 * ������ ��� ����� �� ������� ������
 */

#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "sys/ioctl.h"

#include "Irc.hpp"
#include "Tape.hpp"
/* #include "avtreg.h" */
/* #include "tpb.h" */

/* �������� ������ ����� */
#if 0
Config		Cfg;
#else
char		*CfgDev = "/dev/ravt0";
char		*CfgTdr = "/usr/local/lib/arvid/tdr";
char		*CfgAvt = "/usr/local/lib/arvid/avt";
char		*CfgVcrd = "/usr/local/lib/arvid";
char		*CfgVcr = "akair110.vcr";
char		*CfgLog = "/var/log/arvid/arvid.log";
char		*CfgErr = "/var/log/arvid/arvid.err";
char		CfgPhase = 2;
char		CfgTimeOut = 13;
#endif

static int	arvidio;	/* ���������� ������� ������ */
struct set_rw	SetRW;		/* ��������� ������ ������ */
static int	ident_up;
static int	timeout, atimeout;

VCR	Vcr;		/* ����� VCR */
TAPE	Tape;		/* ����� TAPE */

u_int		debug = 0xff;	/* 0000 0000 1111 1111 */
/* ��������� ������ �� ����� � ��������� */
#define		SIZECMDSTR	1024
char		cmdstr[SIZECMDSTR];
char		*pcmdstr;

void	ParseCmdf(FILE *cmdfile);
void	RunCommand(char *cstr, int size);
int	IrcCommand(char *cstr, int size);
int	Ident();

extern int Ident(int, TAPE *, CTPB *);

int main(int argc, char *argv[]) {
FILE	*cmdfile;		/* ��������� �� ��������� ���� */
int	ncmdf=0;		/* ������� ����� ���������� ����� */

	/* �������� �� ���������� ������� ���������� */
	if (argc < 2) {
		printf("Usage: %s <command.file> [<command.file> [..]]\n", argv[0]);
		return(1);
		}
	/* ������ ������ ���� */
	//	Cfg.Read();
	/* ������� */
	arvidio = open(CfgDev, O_RDWR, 0);
	if (arvidio < 0) {
		printf("Error open %s\n", CfgDev);
		return(1);
		};

	Tape.SetController(Av1052, FORMAT_RS);

	/* �������� � �������� ������� */
	for (ncmdf=1; argc > ncmdf; ncmdf++) {
		cmdfile = fopen(argv[ncmdf], "r");
		if (cmdfile == (FILE *)0)
			printf("Error open %s file\n", argv[ncmdf]);
		else	{
			ParseCmdf(cmdfile);
			fclose(cmdfile);
			}; /* cmdfile */
		}; /* for (ncmd++) */

	/* ��������� ������ */
	if (arvidio)
		close(arvidio);
	/* ���������� ����� */
	return(0);
};

/*
 * �������� ������
 */
void ParseCmdf(FILE *cmdfile) {
	while (fgets(cmdstr, SIZECMDSTR, cmdfile)) {
		/* ���������� ���������� */
		if ((cmdstr[0] == '#') || (cmdstr[0] == '\n')) {
			printf("ignore line: %s", cmdstr);
			}
		else	{
			RunCommand(cmdstr, SIZECMDSTR);
			}
		}; /* while */
};

/*
 * �������� ��������� �����
 * �����: <������, ������>
 */
void RunCommand(char *cstr, int size) {
int	sz;
char	*pcstr;

	/* ������� '\n' */
	sz = 0;
	pcstr = cstr;
	while (isprint(*pcstr) && (sz++ < size)) pcstr++;
	*pcstr = '\0';

	printf(": %s\n", cmdstr);
	/* ���������� ������ ������ */
	sz = 0;
	pcstr = cstr;
	while ((isblank(*pcstr)) && (sz++ < size)) pcstr++;
	/* ���� ������� */
	if (strncasecmp(pcstr, "IDENT", 5) == 0) {
		printf("IDENT command found\n");
		Ident();
		}
	else if (strncasecmp(pcstr, "IRC", 3) == 0) {
		pcstr += 3;
		sz += 3;
		printf("IRC command: %s\n", pcstr);
		IrcCommand(pcstr, size - sz);
		}
	else	{
		printf("Error command, ignore\n");
		};
};

/*
 * ���������� ������ ���������� �������.
 * 0 - OK
 */
int IrcCommand(char *cstr, int size) {
int	sz, exstat;

	sz = 0;
	while ((isblank(*cstr)) && (sz++ < size)) cstr++;
	/* �������� VCR ���� ��� ��� */
	if (Vcr.svcr == NULL) {
		char	*vcrtmp;
		int	len;

		len = strlen(CfgVcrd);
		len += strlen(CfgVcr);
		len += 2;
		vcrtmp = (char *)malloc(len);
		strcpy(vcrtmp, CfgVcrd);
		strcat(vcrtmp, "/");
		strcat(vcrtmp, CfgVcr);
		printf("Loading VCR: %s\n", vcrtmp);
		if (Vcr.LoadVCR(vcrtmp)) {
			printf("Error loading VCR: %s\n", vcrtmp);
			free(vcrtmp);
			return(1);
			};
		free(vcrtmp);
		};
	/* ��������� ����� */
	SetRW.rate = 200;
	SetRW.flag = IOIRC;
	ioctl(arvidio, VSET_WRITE, &SetRW);
	/* ��������� ������� */
	exstat = Vcr.IrcKeyTrn(arvidio, cstr);
	return(exstat);
};

void AlarmIdent(int sig) {
	if (atimeout-- < 0)
		ident_up = 0;
	printf("Alarm\n");
	alarm(1);
};

int Ident() {
int	idok, len;
char	*tpbtmp;

	idok = 1;
	/*  */
	signal(SIGALRM, AlarmIdent);
	/* ��������� ����� */
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvidio, VSET_READ, &SetRW);
	atimeout = CfgTimeOut;
	ident_up = 1;
	alarm(1);
	while(ident_up && idok) {
		if (Tape.Ident(arvidio))
			idok = 0;
		};
	alarm(0);
	signal(SIGALRM, SIG_DFL);
	if (idok)
		return(1);
	printf("%s, %d, %d, %d min, %d csec, %ld\n",
		Tape.cTpb.name, Tape.cTpb.id, Tape.cTpb.format,
		Tape.cTpb.length, Tape.cTpb.time, Tape.cTpb.sector);
	switch(Tape.cTpb.format) {
	case FORMAT_RS:
		len = strlen(CfgTdr);
		len += strlen(Tape.cTpb.name);
		len += 2 + 4;
		tpbtmp = (char *)malloc(len);
		sprintf(tpbtmp, "%s/%s.tdr", CfgTdr, Tape.cTpb.name);
	printf("Tape Dir: %s\n", tpbtmp);
		break;
	defautl:
		break;
		};

	return(0);
};
