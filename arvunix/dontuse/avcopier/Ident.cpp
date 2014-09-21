#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/ioctl.h"
#include "signal.h"
#include "ctype.h"
#include "Tape.hpp"
#include "tpb.h"

int	errinkadr, errall;
static int	hungup;
int	timeout = 13;

extern char *CfgDev;

/*
 * Ctrl-C
 */
void CtrlC(int sig) {
	printf("Signal: %d\n", sig);
	hungup = 0;
};

/*
 * Alarm
 */
void AlarmIdent(int sig) {
	signal(SIGALRM, SIG_IGN);
	timeout--;
	if (timeout < 0)
		hungup = 0;
//	Tape.PrintHeader();
//	printf("Errors: %d\n", errinkadr);
	errinkadr = 0;
	signal(SIGALRM, AlarmIdent);
	alarm(1);
};

int Ident(int fd, TAPE *Tape, CTPB *cTpb) {
int	tmp1, fdsts;
static	set_rw	SetRW;	/* обязательно ??? */
int	cnt;

	/* откроем если не открыт */
	if (fd == 0) {
		fd = open(CfgDev, O_RDWR, 0);
		fdsts = 1;
		}
	else
		fdsts = 0;
	if (fd < 0) {
		printf("\nError open file: %s\n", CfgDev);
		return(1);
		};

	/* установим режим чтения */
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(fd, VSET_READ, &SetRW);

	tmp1 = 0;
	errinkadr = 0;
	errall = 0;

	signal(SIGINT, CtrlC);
	signal(SIGALRM, AlarmIdent);
	alarm(1);

	hungup = 1;
	while (hungup) {
		while (Tape->ReadKadr(fd));
		if (!Tape->DcHeader()) {
			errinkadr++;
			errall++;
	//		printf("Error decode header\n");
			}
		else	{
			tmp1 = Tape->head.g.tapetime;
			Tape->PrintHeader();
			printf("Errors: %d\n", errinkadr);
			hungup = 0;
			}
	//	hungup = 0;
		}; /* while (1) */
	signal(SIGINT, SIG_DFL);
	signal(SIGALRM, SIG_DFL);
	alarm(0);
	/* закроем если открывали */
	if (fdsts)
		close(fd);
	/* переносим инфу */
	switch(Tape->cFormat) {
	case FORMAT_RS:
		if (cTpb->name)
			free(cTpb->name);
		cTpb->name = (char *)malloc(9);
		cnt = 0;
		while ((cnt < 8) && !isblank(cTpb->name[cnt])) {
			cTpb->name[cnt] = Tape->head.rs.name[cnt];
			cnt++;
			};
		cnt++;
		cTpb->name[cnt] = '\0';
		cTpb->id = Tape->head.rs.ident;
		cTpb->format = Tape->head.rs.format;
		cTpb->length = Tape->head.rs.length;
		cTpb->time = Tape->head.rs.tapetime;
		cTpb->sector = Tape->head.rs.sector;
		break;
	default:
		bzero(cTpb, sizeof(CTPB));
		break;
		};
	printf("All Errors: %u\n", errall);
};
