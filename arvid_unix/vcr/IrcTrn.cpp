#include "sys/ioctl.h"
#include "vcr.hpp"
#include "../include/avtreg.h"

/*
#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "string.h"
#include "io.h"
#include "alloc.h"
*/

void main (int argc, char *argv[]) {
VCR	vcr;		// класс работы с VCR
int	key, tmp;
int	fdout, done;
struct	set_rw	SetRW;

	if (argc != 3) {
	/* Имя файла не передано */
		printf("\nUsage: %s file.vcr keynum\n", argv[0]);
		exit(1);
		};
	// загрузим таблицу
	tmp = vcr.LoadVCR(argv[1]);
	if (tmp) {
		exit(tmp);
		}
	printf("Number keys: %d\n", vcr.keynums);
	key = atoi(argv[2]);
#if 0
	fdout = open("./VCR.OUT", O_WRONLY|O_CREAT|O_TRUNC, 0644);
#else
	fdout = open("/dev/ravt0", O_WRONLY, 0644);
	SetRW.rate = 200;
	SetRW.flag = IODATA;
	ioctl(fdout, VSET_WRITE, &SetRW);
	SetRW.rate = 200;
	SetRW.flag = IOIRC;
	ioctl(fdout, VSET_WRITE, &SetRW);
#endif
	if (vcr.IrcKeyTrn(fdout, key)) {
		printf("Error key num: %d\n", key);
		close(fdout);
		exit (1);
		};

	printf("sizeper: %X/%d\n", vcr.sizeper, vcr.sizeper);
	printf("modesize: %X/%d\n", vcr.modesize, vcr.modesize);
	getchar();
	close(fdout);

	for(tmp=0; (tmp < 0x100) && (vcr.skey[tmp]); tmp++) {
		printf("%04X.", vcr.skey[tmp]);
		};
	printf("\n");
};
