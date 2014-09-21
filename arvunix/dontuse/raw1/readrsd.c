#include <stdio.h>
#include <fcntl.h>

#include "../include/avtreg.h"
#include "../include/rsgen.h"

static GF info[NRS*NGR];
static GF code[(NRS+NR)*NGR+46];

extern struct _SY XX;
extern int Decoder200(u_char *info, u_char *code, int start, int count, int group);
extern void decode200sect(u_char *info, u_char *code, u_int sect);

main ()
{
int	i, tdr;
u_char	buf[32];
HEADER_RS	*header_rs;
char	*filetdr="ZVN12-96.TDR\0";
u_char	*codep;

	fread(code, 1, sizeof(code), stdin);
	tdr=open(filetdr, O_WRONLY|O_CREAT, 0446);

XX.Decod_eX[0] = 0;
XX.Decod_eX[1] = 0;
XX.Decod_eX[2] = 0;
XX.Decod_eX[3] = 0;
	codep = code + 46;

	Decoder200(buf, codep, 148, 1, NGR);
	header_rs = (HEADER_RS *)buf;
	fprintf(stderr, "Format: %x\n", header_rs->format);
	fprintf(stderr, "NSector: %d\n", header_rs->nsect);
	fprintf(stderr, "Tape name: %c\n", header_rs->name[0]);
	fprintf(stderr, "Ident: %x\n", header_rs->ident);
	fprintf(stderr, "Length: %d Min\n", header_rs->length & 0x3ff);
	fprintf(stderr, "Tape time: %d sec\n", header_rs->tapetime);
	fprintf(stderr, "Start sector: %u\n", header_rs->sector & 0x3fffffffL);
	fprintf(stderr, "Distance: %d sec\n", header_rs->distance);
	fprintf(stderr, "Error one:  %u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:  %u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree: %u\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error: %u\n", XX.Decod_eX[3]);

	while (!(header_rs->sector & 0x80000000L)) {
		fread(code, 1, sizeof(code), stdin);
		Decoder200(buf, code+46, 148, 1, NGR);
		}
XX.Decod_eX[0] = 0;
XX.Decod_eX[1] = 0;
XX.Decod_eX[2] = 0;
XX.Decod_eX[3] = 0;
	while (!(header_rs->length & 0x2000)) {
/*	while ((header_rs->nsect > 0 )) { */
		for (i=0; i<header_rs->nsect; i++) {
			int c;
			decode200sect(info, code+46, i);
			printf("\nSector: %d\n", (header_rs->sector & 0x3fffffffL) + i);
/*			for (c=0; c<512; c++) printf("%c", info[c]); */
			write(tdr, info, 512);
			}
		while (!fread(code, 1, sizeof(code), stdin));
		Decoder200(buf, code+46, 148, 1, NGR);
		}

/*	for (i=0; i<NGR*NRS; i++) printf("%c", info[i]); */
	fprintf(stderr, "Format: %x\n", header_rs->format);
	fprintf(stderr, "NSector: %d\n", header_rs->nsect);
	fprintf(stderr, "Tape name: %c\n", header_rs->name[0]);
	fprintf(stderr, "Ident: %x\n", header_rs->ident);
	fprintf(stderr, "Length: %d Min\n", header_rs->length & 0x3ff);
	fprintf(stderr, "Tape time: %d sec\n", header_rs->tapetime);
	fprintf(stderr, "Start sector: %u\n", header_rs->sector & 0x3fffffffL);
	fprintf(stderr, "Distance: %d sec\n", header_rs->distance);
	fprintf(stderr, "Error one:  %u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:  %u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree: %u\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error: %u\n", XX.Decod_eX[3]);
	fprintf(stderr, "Error one:   %u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:   %u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree:  %u\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error: %u\n", XX.Decod_eX[3]);

	close (tdr);
}
