#include <stdio.h>

#include "../include/avtreg.h"
#include "../include/rsgen.h"

static GF info[NRS*NGR];
static GF code[(NRS+NR)*NGR+46];

extern struct _SY XX;
extern int Decoder200(GF *info, GF *code, u_int start, u_int count, u_int group);
extern void decod200sect(u_char *info, u_char *code, u_int sect);

main ()
{
int	i;
u_char	buf[32];
HEADER_RS	*header_rs;

	fread(code, 1, sizeof(code), stdin);

XX.Decod_eX[0] = 0;
XX.Decod_eX[1] = 0;
XX.Decod_eX[2] = 0;
XX.Decod_eX[3] = 0;

	Decoder200(buf, code+46, 148, 1, NGR);
	header_rs = (HEADER_RS *)buf;
	fprintf(stderr, "Format:\t\t%x\n", header_rs->format);
	fprintf(stderr, "NSector:\t%d\n", header_rs->nsect);
	fprintf(stderr, "Tape name:\t%c\n", header_rs->name[0]);
	fprintf(stderr, "Ident:\t\t%x\n", header_rs->ident);
	fprintf(stderr, "Length:\t\t%d Min\n", header_rs->length & 0x3ff);
	fprintf(stderr, "Length flag:\t%x\n", header_rs->length & 0xfc00);
	fprintf(stderr, "Tape time:\t%d sec\n", header_rs->tapetime);
	fprintf(stderr, "Start sector:\t%u\n", header_rs->sector & 0x3fffffffL);
	fprintf(stderr, "Flag sector:\t%x\n", header_rs->sector & 0xc0000000L);
	fprintf(stderr, "Distance:\t%d sec\n", header_rs->distance);

	Decoder200(info, code+46, 0, 149, NGR);
	fprintf(stderr, "Error one:\t%u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:\t%u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree:\t%x\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error:\t%u\n", XX.Decod_eX[3]);

	for (i=0; i<NGR*NRS; i++) printf("%c", info[i]);
/*	fprintf(stderr, "Error one:   %u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:   %u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree:  %u\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error: %u\n", XX.Decod_eX[3]); */
}
