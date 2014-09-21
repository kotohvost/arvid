#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/ioctl.h"
#include "signal.h"
#include "Tape.hpp"

TAPE	Tape;
char	*arvid="/dev/ravt0";
static	set_rw	SetRW;	/* обязательно ??? */
int	errinkadr, errall;
int	hungup;
int	timeout = 13;

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
void Alarm(int sig) {
	signal(SIGALRM, SIG_IGN);
	timeout--;
	if (timeout < 0)
		hungup = 0;
	Tape.PrintHeader();
	printf("Errors: %d\n", errinkadr);
	errinkadr = 0;
	signal(SIGALRM, Alarm);
	alarm(1);
};

void main (int argc, char *argv[]) {
int	fdin;	// входной поток
int	tmp1;

//	printf("Size HEADER100: %d\n", sizeof(HEADER100));
//	printf("Size HEADER200: %d\n", sizeof(HEADER200));
//	printf("Size UHEADER:   %d\n", sizeof(UHEADER));

	Tape.SetController(Av1052, FORMAT_RS);

	fdin = open(arvid, O_RDWR, 0);
	if (fdin < 0) {
		printf("\nError open file: %s\n", arvid);
		exit(1);
		};
	/* установим режим чтения */
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(fdin, VSET_READ, &SetRW);

	tmp1 = 0;
	errinkadr = 0;
	errall = 0;

	signal(SIGINT, CtrlC);
	signal(SIGALRM, Alarm);
	alarm(1);

	hungup = 1;
	while (hungup) {
//		sleep(1);
		while (Tape.ReadKadr(fdin));
		if (!Tape.DcHeader()) {
			errinkadr++;
			errall++;
	//		printf("Error decode header\n");
			}
		else	{
			tmp1 = Tape.head.g.tapetime;
			Tape.PrintHeader();
			printf("Errors: %d\n", errinkadr);
			hungup = 0;
			}
		}; /* while (1) */
	close(fdin);
	printf("All Errors: %u\n", errall);
};
