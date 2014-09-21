#include <stdio.h>

#define	AV1020DMAREQ	0xFFBF
#define	AV1020DMAIRQ	0xFFDF

void WriteDprep(u_short *pbuf, int parity) {
int	str, c;
u_short	cmd;

	cmd = 0x79;
	if (parity)
		str = 12;
	else
		str = 13;
	for (c = 0; c < str; c++) {
		*pbuf++ = cmd;
		*pbuf++ = 0xAAAA;
		memset(pbuf, 0, 8*2);
		pbuf += 8;
	}
	if (parity)
		str = 9;
	else
		str = 8;
	for (c = 0; c < str; c++) {
		*pbuf++ = cmd;
		memset(pbuf, 0xAA, 9*2);
		pbuf += 9;
	}
	for (c = 0; c < 256; c++) {
		*pbuf = cmd;
		pbuf += 10;
	}
	*pbuf++ = cmd;
	memset(pbuf, 0xAA, 9*2);
	pbuf += 9;
	for (c = 0; c < 4; c++) {
		*pbuf++ = cmd;
		memset(pbuf, 0, 9*2);
		pbuf += 9;
	}
	*pbuf++ = cmd;
	memset(pbuf, 0xFF, 9*2);
}

u_short WriteSync(u_short *pbuf, char oper, char str, u_short cmd) {
u_short	rc = str*13;

printf("str: %d, oper: %d, rc: %d\n", str, oper, rc);
	while (str--) {
		*pbuf = cmd;
		memset(pbuf+1, 0xFF, 12*2);
		switch(oper) {
		case 0:
			pbuf[10] = 0x0F;
			pbuf[11] = 0xFFFC;
			break;
		case 1:
			pbuf[4] = pbuf[10] = 0xF80F;
			break;
		case 2:
			pbuf[4] = 0xF80F;
			pbuf[10] = 0x0F;
			pbuf[11] = pbuf[12] = 0;
			break;
		case 3:
			memset(pbuf+1, 0, 12*2);
			pbuf[3] = pbuf[9] = 0xFFC0;
			pbuf[4] = pbuf[10] = 0x0F;
			break;
		case 4:
			pbuf[1] = pbuf[2] = 0;
			pbuf[3] = 0xFFC0;
			pbuf[4] = pbuf[10] = 0xF80F;
			break;
		case 5:
			pbuf[10] = 0xF80F;
			break;
		case 6:
			pbuf[4] = pbuf[10] = 0x0F;
			pbuf[9] = 0xFFC0;
			pbuf[5] = pbuf[6] = pbuf[7] = pbuf[8] = pbuf[11] = pbuf[12] = 0;
			break;
		case 7:
			pbuf[3] = pbuf[9] = 0xFFC0;
			pbuf[4] = 0x0F;
			pbuf[10] = 0xF80F;
			pbuf[1] = pbuf[2] = pbuf[5] = pbuf[6] = pbuf[7] = pbuf[8] = 0;
			break;
		default:
			break;
		}
		pbuf += 13;
	}
	return(rc);
}

void WriteSprep(u_short *pbuf, int parity) {
char	*ppar;
int	psize;
char	odd[]  = {4, 0, 1, 5, 2, 1, 1, 6, 1, 3, 1, 7,  2, 1, 18, 0};
char	even[] = {4, 0, 2, 1, 1, 2, 2, 3, 1, 4, 2, 1, 17, 0};

	pbuf += 5660/2;
	if (parity) {
		ppar = odd;
		psize = 8;
	} else {
		ppar = even;
		psize = 7;
	}
	while (psize-- > 0) {
		printf("PBuf: %d, ", pbuf);
		pbuf += WriteSync(pbuf, *ppar++, *ppar++, 0x7C);
		printf("Pbuf: %d\n", pbuf);
	}
}

u_short GetWDsize(int parity) {
	if (parity)
		return(3220);
	else
		return(3207);
}

main(int argc, char *argv[]) {
int	parity;
u_short	*buf, *pbuf;
FILE	*fdout;

	buf = (u_short *)malloc(12854*2);
	memset(buf, 0xa5, 12854*2);
	pbuf = buf;
	printf("\n\nPBUF: %d\n", pbuf);

	parity = 1;
	WriteDprep(pbuf, parity);
	WriteSprep(pbuf, parity);
	pbuf[0] &= AV1020DMAREQ;
	pbuf[5660/2] &= AV1020DMAREQ;
	pbuf[5660/2 + 13] &= AV1020DMAREQ;
	pbuf[5660/2 + 26] &= AV1020DMAIRQ;
	pbuf += GetWDsize(parity);

	parity ^= 1;
	printf("PBUF: %d\n", pbuf);
	WriteDprep(pbuf, parity);
	WriteSprep(pbuf, parity);
	pbuf[0] &= AV1020DMAREQ;
	pbuf[5660/2] &= AV1020DMAREQ;
	pbuf[5660/2 + 13] &= AV1020DMAREQ;
	pbuf[5660/2 + 26] &= AV1020DMAIRQ;
	pbuf += GetWDsize(parity);

	parity ^= 1;
	WriteDprep(pbuf, parity);
	WriteSprep(pbuf, parity);
	pbuf[0] &= AV1020DMAREQ;
	pbuf[5660/2] &= AV1020DMAREQ;
	pbuf[5660/2 + 13] &= AV1020DMAREQ;
	pbuf[5660/2 + 26] &= AV1020DMAIRQ;
	pbuf += GetWDsize(parity);

	parity ^= 1;
	printf("PBUF: %d\n", pbuf);
	WriteDprep(pbuf, parity);
	WriteSprep(pbuf, parity);
	pbuf[0] &= AV1020DMAREQ;
	pbuf[5660/2] &= AV1020DMAREQ;
	pbuf[5660/2 + 13] &= AV1020DMAREQ;
	pbuf[5660/2 + 26] &= AV1020DMAIRQ;

	if (argc > 1) {
		fdout = fopen(argv[1], "w");
		if (fdout != NULL) {
			fwrite(buf, 12854*2, 1, fdout);
			fclose(fdout);
		}
	}
	free(buf);
};
