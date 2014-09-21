/* #include "sys/ioctl.h" */
/* #include "fcntl.h" */
#include "avtlib.h"
#include "avtreg.h"
#include "dcheader.h"

#define	SHIFT_G		388
#define	SHIFT_RS	46
#define	SHIFT_RSC	SHIFT_RS
#define	SHIFT_RSC_50	52

#define	HEAD_RS		148
#define	HEAD_RSC	HEAD_RS
#define	HEAD_RSC_50	151

void *DeCompressLZ(u_char *pSrc, u_char *pDst, int nr_sect);
/*
 * Декодирование заголовка кадра.
 * 0 - OK
 */
int DcHeader(void *code, UHEADER *header, int *coffs, int format) {
unsigned int	tstsum;
int		shift;

	*coffs = 0;
	shift = 0;
	while (shift++ < 11) {
		switch(format) {
		case	FORMAT_K7:
			break;
		case	FORMAT_K7CRC:
			break;
		case	FORMAT_RS:
/*	printf("Offs: 46+(%d)/%d\n", *coffs*2, shift);
*/			if (!DecoderRS((void *)header, code+SHIFT_RS+*coffs*2, HEAD_RS, 1, NGR)) {
				if (header->rs.length && header->rs.tapetime) {
					tstsum = crc32((void *)header, (sizeof(HEADER_RS) -4)/2);
					if (tstsum == header->rs.tstsum)
						return(0);
				} /* length && tapetime */
			} /* DecoderRS */
			if (shift & 1)
				*coffs -= shift * SIZESTR200;
			else
				*coffs += shift * SIZESTR200;
/*	printf("New Offs: %d\n", *coffs);
*/			break;
		case	FORMAT_RSC:
			if (!DecoderRS((void *)header, code+SHIFT_RSC+*coffs*2, HEAD_RSC, 1, NGR)) {
				if (header->rsc.length && header->rsc.tapetime) {
					tstsum = crc32((void *)header, (sizeof(HEADER_RSC) -4)/2);
					if (tstsum == header->rsc.tstsum)
						return(0);
				} /* length && tapetime */
			} /* DecoderRS */
			if (shift & 1)
				*coffs -= shift * SIZESTR200;
			else
				*coffs += shift * SIZESTR200;
			break;
		case	FORMAT_RSC_50:
			if (!DecoderRS((void *)header, code+SHIFT_RSC_50+*coffs*2, HEAD_RSC_50, 1, NGR50)) {
				if (header->rsc50.length && header->rsc50.tapetime) {
					tstsum = crc32((void *)header, (sizeof(HEADER_RSC_50) - 4)/2);
					if (tstsum == header->rsc50.tstsum)
						return(0);
				} /* length && tapetime */
			} /* DecoderRS */
			if (shift & 1)
				*coffs -= shift * SIZESTR325;
			else
				*coffs += shift * SIZESTR325;
			break;
		} /* format */
	}
	return(1);
};

/*
 * Флаги кадра.
 */
int flagFromHeader(struct DcStill *Dc) {
	switch(Dc->header.g.format) {
		case	FORMAT_K7:
		case	FORMAT_K7CRC:
			Dc->fDir = Dc->header.g.sector & DIR_MARK;
			Dc->header.g.sector &= SECTORMASK;
			Dc->fSpec = Dc->header.g.length & SPEC_ATTR;
			Dc->fHead = Dc->header.g.length & HEAD_ATTR;
			Dc->fTail = Dc->header.g.length & TAIL_ATTR;
			Dc->header.g.length &= LENGTHMASK;
			break;
		case	FORMAT_RS:
			Dc->fDir = Dc->header.rs.sector & DIR_MARK;
			Dc->header.rs.sector &= SECTORMASK;
			Dc->fSpec = Dc->header.rs.length & SPEC_ATTR;
			Dc->fHead = Dc->header.rs.length & HEAD_ATTR;
			Dc->fTail = Dc->header.rs.length & TAIL_ATTR;
			Dc->header.rs.length &= LENGTHMASK;
			break;
		case	FORMAT_RSC:
			Dc->csect = 0;
			Dc->cdata = Dc->data + 4;
			Dc->fSpec = Dc->header.rsc.numSpec;
			Dc->fDir = Dc->header.rsc.length & DIR_MARK_C;
			Dc->fHead = Dc->header.rsc.length & HEAD_ATTR;
			Dc->fTail = Dc->header.rsc.length & TAIL_ATTR;
			Dc->header.rsc.length &= LENGTHMASK;
			break;
		case	FORMAT_RSC_50:
			Dc->csect = 0;
			Dc->cdata = Dc->data;
			Dc->fSpec = Dc->header.rsc50.numSpec;
			Dc->fDir = Dc->header.rsc50.length & DIR_MARK_C;
			Dc->fHead = Dc->header.rsc50.length & HEAD_ATTR;
			Dc->fTail = Dc->header.rsc50.length & TAIL_ATTR;
			Dc->header.rsc50.length &= LENGTHMASK;
			break;
		}; /* format */
	return(1);
};

/*
 * Декодирование кадра.
 * 0 - OK
 */
int DcData(void *code, void *data, UHEADER *header, int coffs, u_int *st) {
unsigned int	n_group, size, tstsum, ns;
ADD_RS_BLOCK	block;

	DCErrStat.one = DCErrStat.two = DCErrStat.tree = DCErrStat.fatal = 0;
	*st = 0;
	switch(header->g.format) {
		case	FORMAT_K7:
			break;
		case	FORMAT_K7CRC:
			break;
		case	FORMAT_RS:
			if (header->rs.nsect) {
			/* декодируем все сектора */
				for (ns = 0; ns < header->rs.nsect; ns++) {
					DecoderRS((void *)data + ns*512, code+SHIFT_RS+coffs*2, ns*18, 18, NGR);
					if (!(ns & 1))
						DecoderRS(&block, code+SHIFT_RS+coffs*2, ns/2 + ADD_RS_BEG, 1, NGR);
					bcopy(block.sect[ns & 1], data + ns*512 + 504, 8);
					tstsum = crc32s(data + ns*512, 512/2);
					if (tstsum != block.csum[ns & 1])
						*st |= 1 << ns;
				}; /* for ns */
/*	printf("Status:\t%02X/%d\n", *st, *st);
*/			/* хоть один хороший сектор */
				if (*st != 0xff)
					return(0);
			} else
				return(0); /* nsect */
			break;
		case	FORMAT_RSC:
/*			Dc->csect = 0;
			Dc->cdata = Dc->data + 4;
*/			if (header->rsc.nsect) {
			/* количество информационных групп в кадре */
				if (header->rsc.lenCompress & NO_COMPRESS) {
					n_group = header->rsc.nsect * 512 / 28 + 1;
					size = header->rsc.nsect * 512;
				} else {
					n_group = header->rsc.lenCompress & COMPRESSMASK;
					size = n_group * 28 - 4;
					};
/*	printf("n_group:\t%d/%d\n", n_group, size);
*/				if (!DecoderRS((void *)data, code+SHIFT_RSC+coffs*2, 0, n_group, NGR)) {
					tstsum = crc32((void *)data + 4, size/2);
/*	printf("CRC:\t%08X/%08X\n", tstsum, *(int *)data);
*/					if (tstsum == *(int *)data)
						return(0);
					}; /* DecoderRS */
			} else
				return(0); /* nsect */
			break;
		case	FORMAT_RSC_50:
/*			Dc->csect = 0;
			Dc->cdata = Dc->data;
*/			if (header->rsc50.nsect) {
			/* количество информационных групп в кадре */
				if (header->rsc50.lenCompress & NO_COMPRESS) {
					n_group = (header->rsc50.nsect * 512 - 12) / 44 + 1;
					size = header->rsc50.nsect * 512 - 12;
				} else {
					n_group = header->rsc50.lenCompress & COMPRESSMASK;
					size = n_group * 44;
					};
/*	printf("n_group:\t%d/%d\n", n_group, size);
*/			/* первые 12 байт в заголовке */
				bcopy(header->rsc50.info, data, 12);
			/* оставшееся декодируем */
				if (!DecoderRS((void *)data + 12, code+SHIFT_RSC_50+coffs*2, 0, n_group, NGR50)) {
					tstsum = crc32((void *)data + 12, size/2);
					if (tstsum == header->rsc50.CRC)
						return(0);
					}; /* DecoderRS */
			} else
				return(0); /* nsect */
			break;
		}; /* format */
	return(1);
};

/*
 * Номер первого сектора в кадре.
 */
int getSector(UHEADER *header) {
	switch(header->g.format) {
		case	FORMAT_K7:
		case	FORMAT_K7CRC:
			return(header->g.sector & SECTORMASK);
			break;
		case	FORMAT_RS:
			return(header->rs.sector & SECTORMASK);
			break;
		case	FORMAT_RSC:
			return(header->rsc.sector);
			break;
		case	FORMAT_RSC_50:
			return(header->rsc50.sector);
			break;
		}; /* format */
	return(0);
};

/*
 * Количество секторов в кадре.
 */
int getAvailSector(UHEADER *header) {
	switch(header->g.format) {
		case	FORMAT_K7:
		case	FORMAT_K7CRC:
			return(header->g.nsect);
			break;
		case	FORMAT_RS:
			return(header->rs.nsect);
			break;
		case	FORMAT_RSC:
			return(header->rsc.nsect);
			break;
		case	FORMAT_RSC_50:
			return(header->rsc50.nsect);
			break;
		}; /* format */
	return(0);
};

/*
 * Попадание сектора в кадр.
 * -1	перелет
 *  0	есть такой
 * +1	недолет
 */
int isSector(UHEADER *header, u_int sector) {
	switch(header->g.format) {
		case	FORMAT_K7:
		case	FORMAT_K7CRC:
			if (sector < header->g.sector & SECTORMASK)
				return(-1);
			if (sector < (header->g.sector & SECTORMASK) + header->g.nsect)
				return(0);
			break;
		case	FORMAT_RS:
			if (sector < header->rs.sector & SECTORMASK)
				return(-1);
			if (sector < (header->rs.sector & SECTORMASK) + header->rs.nsect)
				return(0);
			break;
		case	FORMAT_RSC:
			if (sector < header->rsc.sector)
				return(-1);
			if (sector < header->rsc.sector + header->rsc.nsect)
				return(0);
			break;
		case	FORMAT_RSC_50:
			if (sector < header->rsc50.sector)
				return(-1);
			if (sector < header->rsc50.sector + header->rsc50.nsect)
				return(0);
			break;
		}; /* format */
	return(1);
};

/*
 * Чтение данных из декодированного кадра.
 * возвращает количество прочитанных данных.
 */
int readData(struct DcStill *Dc, void *buf, u_int sect, u_int size) {
u_int	rsize=0;
u_int	nsect;
u_char	nbuf[512];

	switch(Dc->header.g.format) {
		case	FORMAT_K7:
			break;
		case	FORMAT_K7CRC:
			break;
		case	FORMAT_RS:
			if ((sect >= (Dc->header.rs.sector & SECTORMASK)) &&
			    (sect < (Dc->header.rs.sector & SECTORMASK) + Dc->header.rs.nsect)) {
			/* стартовый сектор в кадре */
				nsect = sect - Dc->header.rs.sector & SECTORMASK;
			/* размер */
				rsize = (Dc->header.rs.nsect - nsect) * 512;
				if (rsize > size)
					rsize = size;
				bcopy(Dc->data, buf, rsize);
			}; /* sect <>*/
			break;
		case	FORMAT_RSC:
			if ((sect >= Dc->header.rsc.sector) &&
			    (sect < Dc->header.rsc.sector + Dc->header.rsc.nsect)) {
			/* стартовый сектор в кадре */
				nsect = sect - Dc->header.rsc.sector;
			/* размер */
				rsize = (Dc->header.rsc.nsect - nsect) * 512;
				if (rsize > size)
					rsize = size;
				if (Dc->header.rsc.lenCompress & NO_COMPRESS)
					bcopy(Dc->data + 4, buf, rsize);
				else {
					if (Dc->csect > nsect) {
						Dc->csect = 0;
						Dc->cdata = Dc->data + 4;
					}
					while (Dc->csect < nsect)
						Dc->cdata = DeCompressLZ(Dc->cdata, nbuf, Dc->csect);
					nsect = 0;
					while (rsize - nsect > 0) {
/*						if (rsize - nsect >= 512) {
*/							Dc->cdata = DeCompressLZ(Dc->cdata, buf+nsect, Dc->csect);
							Dc->csect++;
							nsect += 512;
/*						} else {
							(void)DeCompressLZ(Dc->cdata, nbuf, Dc->csect);
							bcopy(nbuf, buf, rsize - nsect);
							nsect += rsize - nsect;
						}
*/					}
				}; /* compress */
			}; /* sect <>*/
			break;
		case	FORMAT_RSC_50:
			if ((sect >= Dc->header.rsc50.sector) &&
			    (sect < Dc->header.rsc50.sector+Dc->header.rsc50.nsect)) {
			/* стартовый сектор в кадре */
				nsect = sect - Dc->header.rsc50.sector;
			/* размер */
				rsize = (Dc->header.rsc50.nsect - nsect) * 512;
				if (rsize > size)
					rsize = size;
				if (Dc->header.rsc50.lenCompress & NO_COMPRESS)
					bcopy(Dc->data, buf, rsize);
				else {
					if (Dc->csect > nsect) {
						Dc->csect = 0;
						Dc->cdata = Dc->data;
					}
					while (Dc->csect < nsect)
						Dc->cdata = DeCompressLZ(Dc->cdata, nbuf, Dc->csect);
					nsect = 0;
					while (rsize - nsect > 0) {
/*						if (rsize - nsect >= 512) {
*/							Dc->cdata = DeCompressLZ(Dc->cdata, buf+nsect, Dc->csect);
							Dc->csect++;
							nsect += 512;
/*						} else {
							(void)DeCompressLZ(Dc->cdata, nbuf, Dc->csect);
							bcopy(nbuf, buf, rsize - nsect);
							nsect += rsize - nsect;
						}
*/					}
				}; /* compress */
			}; /* sect <>*/
			break;
		}; /* format */
	return(rsize);
};

void *DeCompressLZ(u_char *pSrc, u_char *pDst, int nr_sect) {
static u_char	Indx[0x1000+256+18];
static u_short	edx, dx;
u_char		*pDstEnd, *pIndx;
u_short		eax, ecx;

	if (nr_sect == 0)
		edx = 0;
	pIndx = Indx + (nr_sect & 7) * 512;
	pDstEnd = pDst + 512;
	while (pDst < pDstEnd) {
		edx += edx;
		dx += dx;
		if (edx == 0) {
			edx = 1;	/* 0x00010000 */
			dx = *(u_short *)pSrc;
/*	printf("%d:\t%X\n", pIndx-Indx, dx); */
			pSrc += 2;
		}; /* add edx,edx; jb */
		if (!(dx & 0x8000)) {
			*pIndx++ = *pDst++ = *pSrc++;
		} else {
	/* 161068CB */
			eax = ecx = *(u_short *)pSrc;
			pSrc += 2;
			eax >>= 4;
			ecx &= 0x0f;
			if (ecx == 0) {
/*	printf("%d:* 161068CB * 00\n", pIndx-Indx); */
				*pDst++ = *pIndx++ = Indx[eax++];
				*pDst++ = *pIndx++ = Indx[eax++];
				*pDst++ = *pIndx++ = Indx[eax++];
			} else if (ecx != 0x0f) {
	/* 16106908 */
				u_short	secx=ecx;	/* [epb] = ecx */
/*	printf("%d:* 16106908 * %02d\n", pIndx-Indx, ecx); */
				*pDst++ = *pIndx++ = Indx[eax++];
				*pDst++ = *pIndx++ = Indx[eax++];
				*pDst++ = *pIndx++ = Indx[eax++];
				*pDst++ = *pIndx++ = Indx[eax++];
				if (pDst < pDstEnd) {
				 if (--ecx) {
				  *pDst++ = *pIndx++ = Indx[eax++];
				  if (--ecx) {
				   *pDst++ = *pIndx++ = Indx[eax++];
				   if (--ecx) {
				    *pDst++ = *pIndx++ = Indx[eax++];
				    if (--ecx) {
				     *pDst++ = *pIndx++ = Indx[eax++];
				     if (--ecx) {
				      *pDst++ = *pIndx++ = Indx[eax++];
				      if (--ecx) {
				       *pDst++ = *pIndx++ = Indx[eax++];
				       if (--ecx) {
				        *pDst++ = *pIndx++ = Indx[eax++];
				        if (--ecx) {
				         *pDst++ = *pIndx++ = Indx[eax++];
				         if (--ecx) {
				          *pDst++ = *pIndx++ = Indx[eax++];
				          if (--ecx) {
				           *pDst++ = *pIndx++ = Indx[eax++];
				           if (--ecx) {
				            *pDst++ = *pIndx++ = Indx[eax++];
				            if (--ecx) {
				             *pDst++ = *pIndx++ = Indx[eax++];
				             if (--ecx) {
				              *pDst++ = *pIndx++ = Indx[eax++];
				              if (--ecx) {
				               *pDst++ = *pIndx++ = Indx[eax++];
				              }
				             }
				            }
				           }
				          }
				         }
				        }
				       }
				      }
				     }
				    }
				   }
				  }
				 }
				}; /* pDst < pDstEnd */ 
			} else {	/* ecx == 0x0f */
	/* 16106A8C */
				ecx = pIndx - Indx - 1;
				if (eax == ecx) {
	/* 16106B0D */
					ecx = 18 + *pSrc++;
/*	printf("* 16106B0D * 0x0F == 0/%d\n", ecx); */
					eax = Indx[eax];
					while (ecx--)
						*pDst++ = *pIndx++ = eax;
				} else {
					u_short	pcx, secx=ecx;
					ecx = 18 + eax + *pSrc++;
					pcx = ecx - eax;
/*	printf("* 16106A8C * 0x0F - %d\n", pcx); */
					if (pcx & 1) {
						*pDst++ = *pIndx++ = Indx[eax++];
/*	printf("* 16106A8C * 0x0F - %d & 1\n", pcx); */
					}
					if (pcx & 2) {
						*pDst++ = *pIndx++ = Indx[eax++];
						*pDst++ = *pIndx++ = Indx[eax++];
/*	printf("* 16106A8C * 0x0F - %d & 2\n", pcx); */
					}
					if (secx == 2) {
/*	printf("* 16106A30 * 0x0F == 2\n"); */
						while (eax < ecx) {
							*pDst++ = *pIndx++ = Indx[eax++];
							*pDst++ = *pIndx++ = Indx[eax++];
						}; /* eax < ecx */
					} else if (secx < 2) {
						u_char	cal=Indx[eax];
/*	printf("* 16106A61 * 0x0F < 2\n"); */
						while (eax < ecx) {
							*pDst++ = *pIndx++ = cal;
							*pDst++ = *pIndx++ = cal;
							*pDst++ = *pIndx++ = cal;
							*pDst++ = *pIndx++ = cal;
						}; /* eax < ecx */
					} else {
/*	printf("* 16106AC0 * 0x0F > 2\n"); */
						while (eax < ecx) {
							*pDst++ = *pIndx++ = Indx[eax++];
							*pDst++ = *pIndx++ = Indx[eax++];
							*pDst++ = *pIndx++ = Indx[eax++];
							*pDst++ = *pIndx++ = Indx[eax++];
						}; /* eax < ecx */
					}; /* =>< 2 */
				}; /* ecx == 0 */
			}; /* ecx = 0x0f */
		}; /* !(dx & 0x8000) */
	}; /* while */
	if (nr_sect & 7)
		bcopy(Indx, Indx+0x1000, 256+18);
	return(pSrc);
};

#if 0
int main(int argc, char *argv[]) {
char	*f;
int	sect;

u_char	Dst[512], *pDst;
u_char	Src[8192], *pSrc;

	if (argc != 3) {
		printf("Usage: %s infile outfile.\n", argv[0]);
		return (1);
		};
	if ((f = argv[1], (infile  = fopen(f, "r")) == NULL)
	 || (f = argv[2], (outfile = fopen(f, "w")) == NULL)) {
		printf("??? %s\n", f);
		return (1);
		};
/*********************************************/
	pSrc = Src;
	pDst = Dst;
	fread(pSrc, sizeof(Src), 1, infile);
	for (sect = 0; sect < 25; sect++) {
		pSrc = DeCompressLZ(pSrc, pDst, sect);
		fwrite(pDst, sizeof(Dst), 1, outfile);
		fflush(outfile);
	}

/*********************************************/
	fclose(infile); fclose(outfile);
};
#endif
