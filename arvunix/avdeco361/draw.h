/*
 *
 */
// PAGEWID, H, fullwin, visualwin, BASECOL(), gidnum, gidlist, machine
// tty, user, group, cmdreg

class DRAW {
	BOX		*cbox;	/* box under cursor */
	struct dir	*cdir;	/* drawed directory */

void	drawframe(register c, int full, int stawin);
void	drawcwd(int col, register char *wd);
void	drawpat(int col, register char *pat, int width);
void	drawsdir(register struct dir *c, struct dir *left, struct dir *right);
void	drawlong(register struct file *f);
void	drawtime(register struct file *f);
void	drawmode(register char *m, register u, register g);
char	*filnam(register char *p);
void	drawfull(register struct file *f);
void	drawstatus(int col, struct file *f, int full);
void	drawpage(int col, register struct file *c, register n, int full);
void	draw1dir(register dir *curdir, dir *left, dir *right);
char	*ltoac(long l);

public:
	/* redraw everything */
void	draw(dir *curdir, dir *left, dir *right);
	/* redraw directory; if statusflag is on, */
	/* then redraw status in opposite window  */
void	drawdir(register dir *curdir, int statusflag, dir *left, dir *right);
	/* draw cursor in current window */
void	drawcursor(dir *curdir);
	/* clear cursor in current window */
void	undrawcursor(dir *curdir);
	/* draw banners at first and last lines, */
	/* and frames of windows		 */
void	drawbanners();
};

extern DRAW draw;
