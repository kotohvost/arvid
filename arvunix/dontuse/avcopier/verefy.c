#include "signal.h"
#include "fcntl.h"
#include "sys/ioctl.h"
#include "Tape.h"
#include "Cfg.hpp"
#include "avt200.h"

struct set_rw	SetRW;
int	fCtrlC = 0;		/* флаг прерывания ^C */
int	curalarm;		/* тайм-аут */
int	sector, sectrs;		/* текущие сектора */

int	stinkadr, sterkadr;	/* статистика по кадрам*/
int	stinsect, stersect;	/* по секторам */

#define	FILENAME	"V325_%02d"

/*
 *
 */
void CtrlC(int sig) {
	signal(SIGINT, SIG_IGN);
	if (CtrlC)
		printf("Signal: %d (SIGINT)\n", sig);
	else
		printf("DUP signal: %d (SIGINT)\n", sig);
	fCtrlC = 1;
	signal(SIGINT, CtrlC);
};

/*
 *
 */
void Alarm(int sig) {
	signal(SIGALRM, SIG_DFL);
	if (curalarm--) {
		printf("Cur sectr: %10u\n", sector);
		printf("\tKadr\t\t|\tSect\n");
		printf("%10u/%-10u", stinkadr, sterkadr);
		printf("   | %10u/%-10u", stinsect, stersect);
		printf("\t%d\t%d\t%d\n", DCErrStat.one,
					DCErrStat.two,
					DCErrStat.tree);
		}
	else	{
		if (CtrlC)
			printf("Signal: %d (SIGALRM)\n", sig);
		else
			printf("DUP signal: %d (SIGALRM)\n", sig);
		fCtrlC = 1;
		};
	signal(SIGALRM, Alarm);
//	alarm(CfgTime);
	alarm(1);
};

/*
 *
 */
void Info(int sig) {
	signal(SIGINFO, SIG_DFL);
	printf("Signal: %d (SIGINFO)\n", sig);
	signal(SIGINFO, Info);
};

int main(int argc, char *argv[]) {
int	fdin;
u_char	head[80];
u_char	data[1024];
u_char	cbuf[SIZE325];
UHEADER	*header;
u_int	stdec, crc;
char	fname[1024];
int	fdout, kadrok=0;

	fdin = open(CfgDev, O_RDWR, 0);
	if (fdin < 0) {
		printf("Error open '%s'.\nExit.\n", CfgDev);
		return(1);
		};

	header = (UHEADER *)head;

	signal(SIGINT, CtrlC);
	signal(SIGALRM, Alarm);
/*	signal(SIGINFO, Info);
*/
	/* режим 325K/s */
	SetRW.rate = 325;
	SetRW.flag = IORAW;
	ioctl(fdin, VSET_READ, &SetRW);
	ioctl(fdin, VSET_PHASE, &CfgPhase3);
	for (sector=0; sector < 65; sector++)
		while (read(fdin, cbuf, sizeof(cbuf)) == 0);

	curalarm = CfgTime;
	alarm(1);
	stinkadr = sterkadr = stinsect = stersect = 0;
	while (fCtrlC == 0) {
		while (read(fdin, cbuf, sizeof(cbuf)) == 0);
		stinkadr++;
		stdec = Decoder200(head, cbuf+52, 151, 1, 152);
		crc = crc32(head, (sizeof(HEADER300C)-4)/2);
		if (!stdec && crc==header->rc50.tstsum) {
			sector = header->rc50.sector;
		//	stinsect += sectrs;
			sectrs = 0;
			printf("nSectors:\t%d\n", header->rc50.nsect);
			sprintf(fname, FILENAME, kadrok);
			fdout = open(fname, O_WRONLY|O_CREAT, 0644);
			if (fdout > 0) {
				write(fdout, cbuf, sizeof(cbuf));
				close(fdout);
				kadrok++;
				};
			if (header->rc50.length & TAIL_ATTR)
				fCtrlC = 1;
	//		alarm(curalarm);
			curalarm = CfgTime;
			}
		else	{
			sterkadr++;
			};
		};
	alarm(0);

	close(fdin);
	printf("Statistics:\n");
	printf("Input/Error\n");
	printf("Kadr:    %10u/%-10u\n", stinkadr, sterkadr);
	printf("Sectors: %10u/%-10u\n", stinsect, stersect);
	signal(SIGINT, SIG_DFL);
};
