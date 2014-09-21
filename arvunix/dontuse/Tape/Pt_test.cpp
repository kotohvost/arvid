#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "Pt.h"
#include "tdr.h"
#include "avt.h"
#include "format.h"

#define	SECTOR	2035080

void main(int argc, char *argv[]) {
PosTable	*pt;
int		fdin;
void		*buf;
off_t		ptseek, ptsize;
struct	HEAD {
	union	{
	TDR_HEAD	thead;
	AVT_HEAD	ahead;
	};
}	head;
AVT_IPHYSTAPE	iphystape;
char	fDir;
long		sector;
unsigned short	time;
unsigned int	tsector;

	if (argc != 2)
		return 0;
	fdin = open(argv[1], O_RDONLY, 0600);
	if (fdin < 0) {
		return 1;
	}

	printf("Size head: %d\n", sizeof(HEAD));
	if (read(fdin, &head, sizeof(HEAD)) != sizeof(HEAD)) {
		close(fdin);
		return 2;
	}
	if (!bcmp(head.ahead.sig, "AVTP", 4)) {
		fDir = 1;				/* AVT */
		if (lseek(fdin, head.ahead.iphystape, SEEK_SET) != head.ahead.iphystape) {
			close(fdin);
			return 2;
		}
		if (read(fdin, &iphystape, sizeof(AVT_IPHYSTAPE)) != sizeof(AVT_IPHYSTAPE)) {
			close(fdin);
			return 2;
		}
		ptseek = iphystape.PToffset;
		ptsize = iphystape.PTsize;
		tsector = sectformat(iphystape.tpb.format);
		printf("This is AVT, format: %02X, sect: %u\n",
			iphystape.tpb.format, tsector);
	} else {
		fDir = 0;				/* TDR */
		ptseek = head.thead.startpt;
		ptsize = head.thead.sizept;
		tsector = sectformat(head.thead.format);
		printf("This is TDR, format: %02X, sect: %u\n",
			head.thead.format, tsector);
	}

	buf = (void *)malloc(ptsize);
	if (buf == NULL) {
		close(fdin);
		return 3;
	}

	if (lseek(fdin, ptseek, SEEK_SET) != ptseek) {
		free(buf);
		close(fdin);
		return 6;
	}
	if (read(fdin, buf, ptsize) != ptsize) {
		free(buf);
		close(fdin);
		return 7;
	}
	if (fDir)
		pt = new PT_AVT(buf, tsector);
	else
		pt = new PT_TDR(buf, tsector);

	sector = SECTOR;
	time = pt->TimeOfSect(sector);
	printf("Sector: %lu -> Time: %u\n", sector, time);
	printf("Time: %u\n", time);
	pt->print();
	delete pt;
	sector = 0;

	free(buf);
	close(fdin);
};
