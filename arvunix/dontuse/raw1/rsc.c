#include <stdio.h>

#include "avtreg.h"
#include "rsgen.h"

static GF info[(NRS+1)*NGR];
static GF code[(NRS+NR+1)*NGR+46];

extern int Coder200(GF *info, GF *code, u_int start, u_int count, u_int group);

main ()
{
int	i;

	fread(info, 1, sizeof(info), stdin);

	Coder200(info, code+46, 0, 149, NGR);

	for (i=0; i<NGR*(NRS+NR); i++) printf("%c", code[i]);
/*	fprintf(stderr, "Error one:   %u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:   %u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree:  %u\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error: %u\n", XX.Decod_eX[3]); */
}
