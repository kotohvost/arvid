#include "sys/ioctl.h"
#include "Irc.hpp"
#include "avtreg.h"

char	*CfgVcr = "/usr/local/lib/arvid/video.vcr";

void main (int argc, char *argv[]) {
VCR	vcr;		// класс работы с VCR
int	key, tmp;
int	fdout, done;
struct	set_rw	SetRW;

	if (argc != 2) {
	/* Имя файла не передано */
		printf("\nUsage: %s keynum\n", argv[0]);
		exit(1);
	}
	// загрузим таблицу
	tmp = vcr.LoadVCR(CfgVcr);
	if (tmp) {
		exit(tmp);
	}
//	printf("Number keys: %d\n", vcr.keynums);
	key = atoi(argv[1]);
#if 1
	fdout = open("./VCR.OUT", O_WRONLY|O_CREAT|O_TRUNC, 0644);
#else
	fdout = open("/dev/ravt0", O_WRONLY, 0);
	SetRW.rate = 200;
	SetRW.mode = IOIRC;
	ioctl(fdout, VSET_WRITE, &SetRW);
#endif
	if (vcr.IrcKeyTrn(fdout, key)) {
//	if (vcr.SetMode(fdout, argv[1])) {
//		printf("Error key num: %d\n", key);
		printf("Error key num: %s\n", argv[1]);
		close(fdout);
		exit (1);
	}

//	printf("sizeper: %X/%d\n", vcr.sizeper, vcr.sizeper);
//	printf("modesize: %X/%d\n", vcr.modesize, vcr.modesize);
//	sleep(2);
//	vcr.Move(fdout, -10);
	close(fdout);
#if 0
	for(tmp=0; (tmp < 0x100) && (vcr.skey[tmp]); tmp++) {
		printf("%04X.", vcr.skey[tmp]);
	};
#endif
	printf("\n");
};
