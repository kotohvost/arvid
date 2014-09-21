#include "fcntl.h"
#include "avtlib.h"
/* #include "tape.h" */
#include "avtreg.h"
#include "sys/ioctl.h"
#include "DcHeader.h"

int main(int argc, char *argv[]) {
int		fd, fdw, i, br, n;
char		code[8192], data[8192], *td, file[1024];
int		fhead, format;
struct set_rw	SetRW;
int		phase;
struct DcStill	Dc;

	Dc.code = code;
/*	Dc.csize = 8192; */
	Dc.csize = SIZE200;
	Dc.data = data;
	Dc.dsize = 8192;

	fd = open("/dev/ravt0", O_RDWR, 0);
	if (fd < 0) {
		printf("Error open '%s'\n", argv[1]);
		return(2);
		};

	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(fd, VSET_READ, &SetRW);
	phase = 17;
	ioctl(fd, VSET_PHASE, &phase);

	phase = 0;
	/* ���� �� ����� */
	while (!Dc.fTail) {
		phase++;
		bzero(Dc.code, Dc.csize);
		while (!(br=read(fd, Dc.code, Dc.csize)));
		bzero(Dc.data, Dc.dsize);
		format = FORMAT_RSC_50;
		fhead = DcHeader(Dc.code, &Dc.header, &Dc.coffs, format);
		if (fhead) {
			format = FORMAT_RS;
			fhead = DcHeader(Dc.code, &Dc.header, &Dc.coffs, format);
		};

		if (fhead)
			printf("%d/%d - header nof found!\n", phase,br);
		else
			flagFromHeader(&Dc);

		if (!fhead && Dc.fDir && !Dc.fTail) {
			if (DcData(Dc.code, Dc.data, &Dc.header, Dc.coffs, &Dc.dstat))
				printf("ERROR decode DATA!!!\n");
			printf("Error's:\t%d/%d/%d %d\n",
				DCErrStat.one, DCErrStat.two,
				DCErrStat.tree, DCErrStat.fatal);
#if 1
			printf("Shift:\t%d\n", Dc.coffs);
			printf("Format:\t%02X", Dc.header.g.format);
			switch(Dc.header.g.format) {
			case	FORMAT_RS:
				printf("\t(RS)\n");
				printf("Tape name:\t");
					for (n=0; n<8; n++)
						printf("%c", Dc.header.rs.name[n]);
				printf("\n");
				printf("Tape length:\t%d min.\n", Dc.header.rs.length & LENGTHMASK);
				printf("Tape time:\t%d sec.\n", Dc.header.rs.tapetime);
				break;
			case	FORMAT_RSC:
				printf("\t(RSC)\n");
				if (!(Dc.header.rsc.lenCompress & 0x8000))
					printf("Compress:\t%d\n", Dc.header.rsc.lenCompress);
				printf("Tape ID:\t%08X\n", Dc.header.rsc.tapeID);
				printf("XOR info:\t%02x %02x %02x %02x\n",
					Dc.header.rsc.num_xor, Dc.header.rsc.num_block,
					Dc.header.rsc.nr_xor, Dc.header.rsc.nr_block);
				printf("Tape length:\t%d min.\n", Dc.header.rsc.length & LENGTHMASK);
				printf("Tape time:\t%d sec.\n", Dc.header.rsc.tapetime);
				printf("Spec sector:\t%d %d/%d/%d\n",
					Dc.header.rsc.numSpec, Dc.header.rsc.x0,
					Dc.header.rsc.x1, Dc.header.rsc.x2);
				break;
			case	FORMAT_RSC_50:
				printf("\t(RSC_50)\n");
				if (!(Dc.header.rsc50.lenCompress & 0x8000))
					printf("Compress:\t%d\n", Dc.header.rsc50.lenCompress);
				printf("Tape ID:\t%08X\n", Dc.header.rsc50.tapeID);
				printf("XOR info:\t%02x %02x %02x %02x\n",
					Dc.header.rsc50.num_xor, Dc.header.rsc50.num_block,
					Dc.header.rsc50.nr_xor, Dc.header.rsc50.nr_block);
				printf("Tape length:\t%d min.\n", Dc.header.rsc50.length & LENGTHMASK);
				printf("Tape time:\t%d sec.\n", Dc.header.rsc50.tapetime);
				printf("Spec sector:\t%d %d/%d/%d\n",
					Dc.header.rsc50.numSpec, Dc.header.rsc50.x0,
					Dc.header.rsc50.x1, Dc.header.rsc50.x2);
				printf("Data CRC:\t%08X\n", Dc.header.rsc50.CRC);
				break;
			}; /* switch (format) */
			printf("Start sector:\t%u\n", getSector(&Dc.header));
			printf("nSectors:\t%d\n", getAvailSector(&Dc.header));
			printf("Sector flags:\t");
			if (Dc.fDir)			printf("DIR.");
			else				printf("DATA.");
			if (Dc.fHead)			printf("START.");
			if (Dc.fTail)			printf("END.");
			if (Dc.fSpec)			printf("SPEC.");
			printf("\n");
#endif
			sprintf(file, "%u\0", getSector(&Dc.header));
			fdw = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
			if (fdw < 0)
				printf("Error open '%s'\n", argv[2]);
			else {
#if 0
				br = getSector(&Dc.header);
				i = getAvailSector(&Dc.header);
				i *= 512;
				td = (char *)malloc(i);
				printf("Read bytes:\t%d\n", readData(&Dc, td, br, i));
				if (write(fd, td, i) < 0) {
					printf("Error write '%s'\n", argv[2]);
					close(fd);
					return(5);
					};
				close(fd);
				free(td);
#else
				if (write(fdw, Dc.data, Dc.dsize) < 0)
					printf("Error write '%s'\n", file);
				close(fdw);
#endif
			}; /* if open */
		}; /* fHead && Dc.fDir */
	}; /* while (Dc.fTail) */
	close(fd);
};
