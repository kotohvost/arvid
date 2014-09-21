#include <stdio.h>

#include "../include/avtreg.h"
#include "rsgen.h"

static GF info[NRS*NGR];
static GF code[(NRS+NR)*NGR+46];

extern struct _SY XX;
extern int CoderRS(GF *info, GF *code, u_int start, u_int count, u_int group);
extern int codeRSsect(GF *info, GF *code, u_int sect);

main ()
{
int	i;

	fread(info, 1, sizeof(info), stdin);

/*	CoderRS(info, code+46, 0, 149, NGR); */
	for (i=0; i<8; i++) codeRSsect(info+(i*512), code, i);

	for (i=0; i<NGR*(NRS+NR); i++) printf("%c", code[i]);
/*	fprintf(stderr, "Error one:   %u\n", XX.Decod_eX[0]);
	fprintf(stderr, "Error two:   %u\n", XX.Decod_eX[1]);
	fprintf(stderr, "Error tree:  %u\n", XX.Decod_eX[2]);
	fprintf(stderr, "Fatal error: %u\n", XX.Decod_eX[3]); */
}
