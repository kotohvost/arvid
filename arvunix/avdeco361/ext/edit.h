#ifndef DECO_EDIT_H
#define DECO_EDIT_H

#include "view.h"

class EDITFILE : VIEWFILE {
	int	editmodflg;

int	charcode(int l, int n);
void	edithead(char *filename, int line, int col, int off);
int	lastpos(int n);
int	ischar(register c);
int	wordbeg(register LINE *p, register off);
void	clearline(register REC *r, int line);
void	joinlines(register REC *r, int line);
void	breakline(register REC *r, int line, int off);
void	cleartoeoln(register REC *r, int line, int off);
int	wordend(register LINE *p, register off);

public:
//void	edit(int d, char *filename, char *basename);
	EDITFILE(int d, char *filename, char *basename);
};

#endif	/* DECO_EDIT_H */
