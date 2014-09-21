#ifndef DECO_VIEW_H
#define DECO_VIEW_H

#include "rec.h"

/* Global */
extern char viewsrch [SEARCHSZ];	/* search string */
extern int viewraw;			/* raw mode */

class VIEWFILE {
	char	viewsbuf [SEARCHSZ];	/* search string buffer */

int	textview(char *filename);
int	hexview(char *filename);
void	viewhead(char *filename, int line, int col);
int	strequal(register char *a, register char *b, register n);
int	cvtsrch(register char *from, register char *to);
int	hsearch(int l, int c, int *pline, int *pcol);
void	viewhpage(int line);
void	viewhline(int line, int i);
void	prhline(register char *p, int len);
void	viewhhead(char *filename, int line);
void	viewchar(register c);

public:
	int	viewfd;
//	char	viewsrch [SEARCHSZ];	/* search string */
	REC	*viewrec;
	RECFILE	rec;

int	offset(int l, register n);
int	column(int l, int n);
void	viewline(int line, register col, int scrline);
int	search(char *str, int l, int c, int *pline, int *pcol);
void	viewpage(int line, int col);
	VIEWFILE(int d, char *filename);
	VIEWFILE(){};
//void	view(int d, char *filename);
};

#endif	/* DECO_VIEW_H */
