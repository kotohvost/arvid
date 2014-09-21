/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

struct submenu {
	char *name;                     /* name of submenu */
	int key;                        /* return key */
	void (*exec) (...);             /* function to execute */
	int active;                     /* is it active for now */
	int tag;                        /* tag of submenu */
	int row;                        /* row of submenu */
};

struct menu {
	char *name;                     /* name of menu */
	int key;                        /* return key */
	struct submenu *submenu;        /* array of submenus */
	int col;                        /* base column of submenu window */
	int len;                        /* length of head name */
	int hgh;                        /* height of submenu window */
	int wid;                        /* width of submenu window */
	int nsm;                        /* current submenu */
	BOX *box;                       /* box under menu */
};

extern struct menu menu [];             /* main menu array */

class MENU {
	int	nmenu;			/* current menu */

void	initmenu(void);
void	getboxes(void);
void	clrmenu(register struct menu *m);
void	drawhead(int nm);
void	drawmenu(register struct menu *m);
int	hotkey(int c, register struct menu *m);
void	upmenu(register struct menu *m);
void	downmenu(register struct menu *m);
int	getmenu(void);
int	tag (int k, int sk);

public:
void	runmenu (int mkey);
};
