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

/*
 *      Record-oriented file access package.
 *      -------------------------------------
 *
 *      REC *RecOpen (int fd, int mode)
 *              - initialize Rec package for file fd,
 *              for writing if mode!=0.
 *
 *      int RecSave (REC *r, char *filename)
 *              - and write out changes,
 *              returns 0 if ok or -1 if cannot write file.
 *
 *      int RecClose (REC *r)
 *              - close Rec discarding changes.
 *
 *      RecBroken (REC *r)
 *              - if Rec has broken lines, return 1, else 0.
 *
 *      LINE *RecGet (REC *r, int linenum)
 *              - get line.
 *
 *      RecPut (LINE *l, int newlen)
 *              - put line.
 *
 *      RecInsLine (REC *r, int linenum)
 *              - insert empty line before line #linenum
 *
 *      RecDelLine (REC *r, int linenum)
 *              - delete line #linenum
 *
 *      RecInsChar (REC *r, int line, int off, int sym)
 *              - insert char into line
 *
 *      RecDelChar (REC *r, int line, int off)
 *              - delete char from line
 */

# include "unistd.h"
# include "fcntl.h"
# include "string.h"
# include "rec.h"
# include "mem.h"
#include "choice.h"

# define BUFSIZ         512
# define TEMPCELL       16              /* minimal size of temp space */
# define QUANT          512
# define MAXLINE        1024            /* maximum length of unbroken line */
# define NOINDEX        (POOLSZ+2)

# define BAKSUFFIX      ".b"

static char mask [8] = { 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, };
static char tfilepattern [] = "/tmp/recXXXXXX";

static lenbuf, eoln;
static char *scanbuf, *pbuf;
static char *tfilename;
static MAXLEN = ((unsigned int) (int) -1 >> 1) / sizeof (struct index);

int RECFILE::ffcopy(int from, int to) {
	char buf [512], *p;
	register n, k;

	for (;;) {
		n = read (from, buf, sizeof (buf));
		if (n < 0) {
			error ("Cannot read file");
			return (-1);
		}
		if (n == 0)
			return (0);
		p = buf;
		while (n > 0) {
			k = write (to, p, (unsigned) n);
			if (k <= 0) {
				error ("Cannot write file");
				return (-1);
			}
			n -= k;
			p += k;
		}
	}
}

void RECFILE::tempfree(REC *r, register long seek, int len) {
	/* mark temp space free */
	register i;

	len = (len + TEMPCELL - 1) / TEMPCELL;
	seek /= TEMPCELL;
	for (i=0; i<len; ++i, ++seek)
		r->tmap [seek>>3] &= ~mask [seek & 7];
}

long RECFILE::tempbusy(REC *r, int len) {
	/* search & mark temp space busy */
	register i, n;
	register long seek;

	len = (len + TEMPCELL - 1) / TEMPCELL;
	for (seek=0; seek<MAXTEMP; ++seek) {
		for (n=0; n<len; ++n)
			if (r->tmap [seek>>3] & mask [seek & 7])
				break;
		if (n >= len)
			break;
	}
	if (seek >= MAXTEMP) {
		error ("No temp space");
		return (0);
	}
	for (n=seek, i=0; i<len; ++i, ++n)
		r->tmap [n>>3] |= mask [n & 7];
	return (seek * TEMPCELL);
}

long RECFILE::tempsave(register REC *r, char *str, int len) {
	/* save string in temp file, return seek */
	register long seek;

	if (! len)
		return (0);
	seek = tempbusy (r, len);
	if (lseek (r->tfd, seek, 0) < 0)
		error ("Cannot lseek on writing temp file");
	if (write (r->tfd, str, (unsigned) len) != len)
		error ("Cannot write temporary file");
	return (seek);
}

int RECFILE::scanline(int fd) {
	register len;

	len = 0;
	eoln = 0;
	for (;;) {
		if (pbuf >= &scanbuf[lenbuf]) {
			lenbuf = read (fd, pbuf = scanbuf, (unsigned) BUFSIZ);
			if (lenbuf <= 0)
				return (len ? len : -1);
		}
		if (*pbuf++ == '\n') {
			eoln = 1;
			return (len);
		}
		if (++len >= MAXLINE)
			return (len);
	}
}

REC *RECFILE::RecOpen(int fd, int wmode) {
	register REC *r;
	register struct index *x;
	register i;

	r = (REC *) MemAlloc (sizeof (REC));
	r->fd = fd;
	r->bakfd = -1;
	if (wmode) {
		if (! tfilename)
			tfilename = tfilepattern;
		r->tfd = creat (tfilename, 0600);
		if (r->tfd < 0) {
			error ("Cannot open temporary file");
			return (0);
		}
		close (r->tfd);
		r->tfd = open (tfilename, 2);
		if (r->tfd < 0) {
			error ("Cannot reopen temporary file");
			return (0);
		}
		unlink (tfilename);
	} else
		r->tfd = -1;

	for (i=0; i<TEMPSZ; ++i)
		r->tmap [i] = 0;

	for (i=0; i<POOLSZ; ++i)
		r->map[i].busy = 0;

	lseek (r->fd, 0L, 0);
	r->broken = 0;
	r->len = 0;
	r->lindex = 0;
	r->nindex = 0;
	r->size = 0;
	scanbuf = (char *) MemAlloc (BUFSIZ);
	pbuf = scanbuf;
	lenbuf = 0;
	for (;;) {
		MemCheckIndex (r->lindex, struct index *, r->nindex, QUANT, r->len+1);
		x = &r->lindex[r->len];
		x->seek = r->size;              /* store seek of current line */
		x->poolindex = NOINDEX;         /* clear pool index of current line */
		x->flags = 0;                   /* clear flags */
		x->len = scanline (r->fd);      /* scan next line */
		if (x->len < 0)
			break;
		r->size += x->len;              /* increment seek by length of line */
		if (eoln)
			++r->size;              /* \n at end of line */
		else {
			x->flags |= XNOEOLN;    /* no end of line */
			++r->broken;
		}
		if (++r->len >= MAXLEN) {
			error ("file too long");
			return (0);
		}
	}
	MemFree ((mem *) scanbuf);
	return (r);
}

void RECFILE::RecClose(register REC *r) {
	register i;

	for (i=0; i<POOLSZ; ++i)
		if (r->map[i].busy) {
			if (r->pool[i].len)
				MemFree ((mem *) r->pool[i].ptr);
			r->lindex[r->map[i].index].poolindex = NOINDEX;
			r->map[i].busy = 0;
		}
	MemFree ((mem *) r->lindex);
	if (r->tfd >= 0)
		close (r->tfd);
	if (r->bakfd >= 0)
		close (r->bakfd);
}

int RECFILE::RecSave(register REC *r, char *filename) {
	register i, fd;
	register LINE *p;
	char bak [40];

	if (r->bakfd < 0) {
		strcpy (bak, filename);
		strcat (bak, BAKSUFFIX);
		r->bakfd = creat (bak, 0600);
		if (r->bakfd < 0) {
			error ("Cannot create %s", bak);
			return (-1);
		}
		lseek (r->fd, 0L, 0);
		if (ffcopy (r->fd, r->bakfd) < 0) {
			close (r->bakfd);
			r->bakfd = -1;
			unlink (bak);
			return (-1);
		}
		close (r->bakfd);
		r->bakfd = open (bak, 0);
		if (r->bakfd < 0) {
			error ("Cannot open %s", bak);
			unlink (bak);
			return (-1);
		}
		close (r->fd);
		r->fd = r->bakfd;
	}
	fd = creat (filename, 0664);
	if (fd < 0) {
		error ("Cannot create %s", filename);
		unlink (bak);
		return (-1);
	}
	for (i=0; i<r->len; ++i) {
		p = RecGet (r, i);
		if (p->len)
			write (fd, p->ptr, (unsigned) p->len);
		write (fd, "\n", 1);
	}
	close (fd);
	return (0);
}

void RECFILE::RecBreak(REC *r) {
	register i;
	register struct index *x;

	x = r->lindex;
	for (i=0; i<r->len; ++i, ++x)
		x->flags &= ~XNOEOLN;
}

void RECFILE::readline(int fd, long seek, int len, LINE *rez) {
	register l, n;
	register char *s;

	rez->len = len;
	rez->mod = 0;
	if (! len) {
		rez->ptr = "";
		return;
	}
	rez->ptr = (char *) MemAlloc (rez->len);
	if (lseek (fd, seek, 0) < 0)
		error ("Cannot lseek on reading fd=%d, seek=%ld", fd, seek);
	for (l=rez->len, s=rez->ptr; l>0; l-=n, s+=n) {
		n = read (fd, s, (unsigned) l);
		if (n <= 0) {
			error ("Cannot read line");
			if (rez->len)
				MemFree ((mem *) rez->ptr);
			rez->len = 0;
			rez->ptr = "";
			return;
		}
	}
}

int RECFILE::freeline(register REC *r) {
	register struct map *m;
	register struct index *x;
	register LINE *l;
	register mintime, minindex;

	/* find free place in pool */
	for (m=r->map; m<r->map+POOLSZ; ++m)
		if (! m->busy)
			return (m - r->map);
	/* pool is full; find the oldest line */
	mintime = r->map[0].time;
	minindex = 0;
	for (m=r->map; m<r->map+POOLSZ; ++m)
		if (m->time < mintime)
			minindex = m - r->map;
	m = &r->map[minindex];
	l = &r->pool[minindex];
	x = &r->lindex[m->index];
	/* remove line from pool */
	if (l->mod) {           /* line is modified, save it in temp file */
		if ((x->flags & XTEMP) && l->oldlen)
			tempfree (r, x->seek, l->oldlen);
		x->seek = tempsave (r, l->ptr, l->len);
		x->len = l->len;
		x->flags |= XTEMP;
	}
	if (l->len)
		MemFree ((mem *) l->ptr);
	x->poolindex = NOINDEX;
	m->busy = 0;
	return (minindex);
}

LINE *RECFILE::RecGet(register REC *r, int n) {
	register struct index *x;
	register struct map *m;
	register LINE *p;
	static long timecount = 1;              /* time stamp */

	if (n < 0 || n >= r->len)
		return (0);
	x = &r->lindex[n];
	if (x->poolindex != NOINDEX) {          /* line is in cache */
		r->map[x->poolindex].time = ++timecount;
		return (&r->pool[x->poolindex]);
	}
	x->poolindex = freeline (r);            /* get free pool index */
	p = &r->pool[x->poolindex];
	m = &r->map[x->poolindex];
	m->time = ++timecount;
	m->index = n;
	m->busy = 1;
	/* read line from file */
	readline ((x->flags & XTEMP) ? r->tfd : r->fd, x->seek, x->len, p);
	p->noeoln = (x->flags & XNOEOLN) != 0;
	return (p);
}

void RECFILE::RecPut(register LINE *p, int newlen) {
	if (! p->mod)
		p->oldlen = p->len;
	p->mod = 1;
	p->len = newlen;
}

void RECFILE::RecDelChar(register REC *r, int line, int off) {
	register LINE *p;
	char *s;

	if (! (p = RecGet (r, line)))
		return;
	if (p->len <= off)
		return;
	s = (char *) MemAlloc (p->len-1);
	if (off)
		MemCopy (s, p->ptr, off);
	if (off <= p->len-1)
		MemCopy (s+off, p->ptr+off+1, p->len-off-1);
	MemFree ((mem *) p->ptr);
	p->ptr = s;
	RecPut (p, p->len - 1);
}

void RECFILE::RecInsChar(register REC *r, int line, int off, int sym) {
	register LINE *p;
	char *s;

	if (! (p = RecGet (r, line)))
		return;
	s = (char *) MemAlloc (p->len+1);
	if (off)
		MemCopy (s, p->ptr, off);
	s [off] = sym;
	if (off < p->len)
		MemCopy (s+off+1, p->ptr+off, p->len-off);
	if (p->len)
		MemFree ((mem *) p->ptr);
	p->ptr = s;
	RecPut (p, p->len + 1);
}

void RECFILE::RecInsLine(register REC *r, int n) {
	register struct index *x, *i;
	register struct map *m;
	register k;

	if (n<0 || n>r->len)
		return;
	++r->len;
	MemCheckIndex (r->lindex, struct index *, r->nindex, QUANT, r->len+1);
	i = &r->lindex[n];
	for (x= &r->lindex[r->len-1]; x>i; --x)
		x[0] = x[-1];
	i->seek = 0;
	i->len = 0;
	i->poolindex = NOINDEX;
	i->flags = XTEMP;
	m = r->map;
	for (k=0; k<POOLSZ; ++k, ++m)
		if (m->index >= n)
			++m->index;
}

void RECFILE::RecDelLine(register REC *r, int n) {
	register struct index *x, *i;
	register struct map *m;
	register LINE *l;
	register k;

	if (n<0 || n>=r->len)
		return;
	x = &r->lindex[n];
	m = r->map;
	if (x->poolindex != NOINDEX) {          /* exclude line from pool */
		l = &r->pool[x->poolindex];
		if (x->flags & XTEMP)
			if (l->mod) {
				if (l->oldlen)
					tempfree (r, x->seek, (long) l->oldlen);
			} else if (l->len)
				tempfree (r, x->seek, l->len);
		if (l->len)
			MemFree ((mem *) l->ptr);
		m[x->poolindex].busy = 0;
	}
	for (k=0; k<POOLSZ; ++k, ++m)
		if (m->index > n)
			--m->index;
	i = &r->lindex[r->len-1];
	for (x= &r->lindex[n]; x<i; ++x)
		x[0] = x[1];
	--r->len;
}
