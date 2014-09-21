/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Serge Vakulenko not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Serge Vakulenko makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * SERGE VAKULENKO DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SERGE VAKULENKO BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */
#ifndef DECO_REC_H
#define DECO_REC_C

# define SEARCHSZ       41              /* max size of search string */
# define MAXTEMP        2048            /* max of temp file size (in cells) */
# define TEMPSZ         ((MAXTEMP+7)/8) /* temp table size (in bytes) */
# define POOLSZ         50              /* number of lines in cache */

# define XTEMP          1               /* line is in tmp file */
# define XNOEOLN        2               /* no end of line */

/*-----------------------------*/
struct index {                          /* out of core line descriptor */
	long            seek;           /* seek in file */
	short           len;            /* length of line */
	unsigned        poolindex :8;   /* index in pool or NOINDEX */
	unsigned        flags :8;       /* is in tmp file */
};

struct map {                            /* pool cell descriptor */
	short           busy;           /* cell busy */
	short           index;          /* index in lindex */
	long            time;           /* time of last access */
};

typedef struct {                        /* in core line descriptor */
	char            *ptr;           /* pointer to string */
	short           len;            /* length of string */
	short           oldlen;         /* length before mod */
	unsigned        mod :8;         /* line is modified */
	unsigned        noeoln :8;      /* no end of line */
} LINE;

typedef struct {
	struct index    *lindex;        /* out of core line descriptors */
	struct map      map [POOLSZ];   /* line pool */
	LINE            pool [POOLSZ];  /* in core line descriptors */
	char            tmap [TEMPSZ];  /* temp file map */
	long            nindex;         /* number of indexes malloc'ed */
	long            size;           /* length of file in bytes */
	short           fd;             /* file descriptor */
	short           bakfd;          /* bak file descriptor */
	short           tfd;            /* temp file descriptor */
	short           len;            /* length of file in lines */
	short           broken;         /* there are broken lines */
} REC;
/*-----------------------------*/

class RECFILE {
int	ffcopy(int from, int to);
void	tempfree(REC *r, register long seek, int len);
long	tempbusy(REC *r, int len);
long	tempsave(register REC *r, char *str, int len);
int	scanline(int fd);
void	readline(int fd, long seek, int len, LINE *rez);
int	freeline(register REC *r);

public:
REC	*RecOpen(int fd, int wmode);
void	RecClose(register REC *r);
int	RecSave(register REC *r, char *filename);
void	RecBreak(REC *r);
LINE	*RecGet (register REC *r, int n);
void	RecPut (register LINE *p, int newlen);
void	RecInsChar(register REC *r, int line, int off, int sym);
void	RecDelChar(register REC *r, int line, int off);
void	RecInsLine(register REC *r, int n);
void	RecDelLine(register REC *r, int n);
};

#endif	/* DECO_REC_H */
