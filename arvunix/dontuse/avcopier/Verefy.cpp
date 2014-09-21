#include "signal.h"
#include "sys/ioctl.h"
#include "Tape.hpp"
#include "Cfg.hpp"

TAPE	Tape;			/* класс работы с данными на ленте */
set_rw	SetRW;
int	fCtrlC = 0;		/* флаг прерывания ^C */
int	curalarm;		/* тайм-аут */
int	sector, sectrs;		/* текущие сектора */

int	stinkadr, sterkadr;	/* статистика по кадрам*/
int	stinsect, stersect;	/* по секторам */

/* extern struct _SY XX; */
extern struct DCStError DCErrStat;

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
		printf("Cur sectr: %10u\n", Tape.cSectr);
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
char	buf[512];

	Tape.SetController(Av1052, FORMAT_RS);
	fdin = open(CfgDev, O_RDWR, 0);
	if (fdin < 0) {
		printf("Error open '%s'.\nExit.\n", CfgDev);
		return(1);
		};

	signal(SIGINT, CtrlC);
	signal(SIGALRM, Alarm);
	signal(SIGINFO, Info);

	/* режим 200K/s */
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(fdin, VSET_READ, &SetRW);
	ioctl(fdin, VSET_PHASE, &CfgPhase);

	curalarm = CfgTime;
	alarm(1);
	stinkadr = sterkadr = stinsect = stersect = 0;
	while (fCtrlC == 0) {
		while (Tape.ReadKadr(fdin));
		stinkadr++;
		if (!Tape.DcHeader()) {
			sterkadr++;
			}
		else	{
		//	sectrs = Tape.GetSectors();
		//	stinsect += sectrs;
			sectrs = 0;
			while (sectrs < Tape.GetSectors()) {
				stinsect++;
				if (Tape.GetSector(sectrs, &buf)) {
					stersect++;
					}
				else	{
					};
				sectrs++;
				};
			if (Tape.fTail)
				fCtrlC = 1;
	//		alarm(curalarm);
			curalarm = CfgTime;
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
