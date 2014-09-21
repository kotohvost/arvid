#include <stdio.h>

#include "tape.h"
#include "avtlib.h"

static GF info[NRS*NGR];
static GF code[(NRS+NR)*NGR+82];

main ()
{
int	i;
u_char	buf[32];
HEADER_RS	*header_rs;

	fread(code, 1, sizeof(code), stdin);

DCErrStat.one = 0;
DCErrStat.two = 0;
DCErrStat.tree = 0;
DCErrStat.fatal = 0;

	if (DecoderRS(buf, code+82, 148, 1, NGR))
		fprintf(stderr, "Error decode header\n");
	fprintf(stderr, "Error one:\t%u\n", DCErrStat.one);
	fprintf(stderr, "Error two:\t%u\n", DCErrStat.two);
	fprintf(stderr, "Error tree:\t%x\n", DCErrStat.tree);
	fprintf(stderr, "Fatal error:\t%u\n", DCErrStat.fatal);

DCErrStat.one = 0;
DCErrStat.two = 0;
DCErrStat.tree = 0;
DCErrStat.fatal = 0;

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

	DecoderRS(info, code+82, 0, 149, NGR);
	fprintf(stderr, "Error one:\t%u\n", DCErrStat.one);
	fprintf(stderr, "Error two:\t%u\n", DCErrStat.two);
	fprintf(stderr, "Error tree:\t%x\n", DCErrStat.tree);
	fprintf(stderr, "Fatal error:\t%u\n", DCErrStat.fatal);

	for (i=0; i<NGR*NRS; i++) printf("%c", info[i]);
}
