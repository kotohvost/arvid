#include <stdio.h>

#include "../include/avtreg.h"
#include "../include/rsgen.h"

static GF info[12*88*2];
static GF buf[SIZE100];
static GF code[SIZE100];

/* extern struct _SY XX; */
extern int Decoder100(GF *info, GF *code, u_int start, u_int count);
/* extern void decodeRSsect(u_char *info, u_char *code, u_int sect); */

main ()
{
int	i, str;
HEADER_RS	*header_rs;

	fread(code, 1, sizeof(code), stdin);
/*
XX.Decod_eX[0] = 0;
XX.Decod_eX[1] = 0;
XX.Decod_eX[2] = 0;
XX.Decod_eX[3] = 0;
*/
	i = 434;
	str = 0;
	while (str < 253*8*2) {
		int	t;
		for (t=0; t<16; t++) {
			buf[t+str] = code[i];
			i++;
			}
		i += 2;
		str += 16;
		}
	Decoder100(info, buf, 0, 88);
/*	header_rs = (HEADER_RS *)buf;
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

	DecoderRS(info, code+46, 0, 149, NGR);
	fprintf(stderr, "Error one:\t%u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:\t%u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree:\t%x\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error:\t%u\n", XX.Decod_eX[3]);
*/
/*	for (i=0; i<sizeof(info); i++) printf("%c", info[i]); */
	for (i=0; i<4096; i++) printf("%c", buf[i]);

/*	fprintf(stderr, "Error one:   %u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:   %u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree:  %u\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error: %u\n", XX.Decod_eX[3]); */
}
