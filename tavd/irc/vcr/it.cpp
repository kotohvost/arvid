#include "sys/ioctl.h"
#include "Irc.hpp"
#include "../include/avtreg.h"

/*
#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "string.h"
#include "io.h"
#include "alloc.h"
*/

char	*CfgVcr = "/usr/local/lib/arvid/akair110.vcr";

void main (int argc, char *argv[]) {
VCR	vcr;		// класс работы с VCR
int	key, tmp;
int	fdout, done;
struct	set_rw	SetRW;

	// загрузим таблицу
	tmp = vcr.LoadVCR(CfgVcr);
	if (tmp) {
		exit(tmp);
		}
	fdout = open("/dev/ravt0", O_WRONLY, 0);
//	SetRW.rate = 200;
//	SetRW.flag = IODATA;
//	ioctl(fdout, VSET_WRITE, &SetRW);
	SetRW.rate = 200;
	SetRW.flag = IOIRC;

	ioctl(fdout, VSET_WRITE, &SetRW);

//	if (vcr.IrcKeyTrn(fdout, key)) {
	if (vcr.SetMode(fdout, "STOP")) {
		printf("Error key num: %s\n", "STOP");
		close(fdout);
		exit (1);
		};

//	vcr.Move(fdout, -10);
	vcr.IrcKeyTrn(fdout, "PLAY");
	ioctl(fdout, VSET_WRITE, &SetRW);
	vcr.IrcKeyTrn(fdout, "REW");
	ioctl(fdout, VSET_WRITE, &SetRW);
	sleep(2);
	vcr.IrcKeyTrn(fdout, "STOP");
	close(fdout);
};
