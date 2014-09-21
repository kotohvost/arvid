/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */
#include "sys/stat.h"

# include "scr.h"
# include "dir.h"
# include "deco.h"
# include "menu.h"
# include "ex.h"		/* class InitFile */
# include "ut.h"
//# include "com.h"		/* class CMD */
# include "key.h"
# include "cmd.h"
# include "help.h"
# include "main.h"

/* menu handler */

void usermenu(void);

extern useredit, userview, usecshell;

static struct submenu lmenu [] = {
	{       "Brief",                'b',    setfull,        },
	{       "Long",                 'l',    setfull,        },
	{       "Full",                 'f',    setfull,        },
	{       "Status           ^L",  's',    setstatus,      },
# ifndef COMPAT32
	{       "Align extensions",     'a',    switchalign,    },
	{       "",                     0,      0,              },
	{       "sort by Name",         'n',    setsort,        },
	{       "eXtension",            'x',    setsort,        },
	{       "Time",                 't',    setsort,        },
	{       "siZe",                 'z',    setsort,        },
	{       "Unsorted",             'u',    setsort,        },
	{       "Reverse",              'r',    setrevsort,     },
	{       "sort by tYpe",         'y',    settypesort,    },
	{       "",                     0,      0,              },
# endif
//	{       "Re-read          ^R",  'r',    reread,         },
	{       "Pattern",              'p',    setpattern,     },
	{       0,                      0,      0,              },
};

static struct submenu rmenu [] = {
	{       "Brief",                'b',    setfull,        },
	{       "Long",                 'l',    setfull,        },
	{       "Full",                 'f',    setfull,        },
	{       "Status           ^L",  's',    setstatus,      },
# ifndef COMPAT32
	{       "Align extensions",     'a',    switchalign,    },
	{       "",                     0,      0,              },
	{       "sort by Name",         'n',    setsort,        },
	{       "eXtension",            'x',    setsort,        },
	{       "Time",                 't',    setsort,        },
	{       "siZe",                 'z',    setsort,        },
	{       "Unsorted",             'u',    setsort,        },
	{       "Reverse",              'r',    setrevsort,     },
	{       "sort by tYpe",         'y',    settypesort,    },
	{       "",                     0,      0,              },
# endif
//	{       "Re-read          ^R",  'r',    reread,         },
	{       "Pattern",              'p',    setpattern,     },
	{       0,                      0,      0,              },
};

static struct submenu fmenu [] = {
	{       "Help             F1",  'h',    genhelp,        },
	{       "User menu        F2",  'u',    usermenu,       },
//	{       "View             F3",  'v',    view,           },
//	{       "Edit             F4",  'e',    edit,           },
//	{       "Copy             F5",  'c',    copy,           },
//	{       "Link",                 'l',    makelink,       },
# ifdef S_IFLNK
//	{       "Symlink",              's',    makeslink,      },
# endif
//	{       "Rename/move      F6",  'r',    renmove,        },
//	{       "Make directory   F7",  'm',    makedir,        },
//	{       "Delete           F8",  'd',    mydelete,         },
# ifndef COMPAT32
	{       "",                     0,      0,              },
	{       "select Group     +",   'g',    tagall,         },
	{       "Unselect group   -",   'u',    untagall,       },
	{       "",                     0,      0,              },
# endif
	{       "Quit             F10", 'q',    quit,           },
	{       0,                      0,      0,              },
};

static struct submenu cmenu [] = {
//	{       "run subShell",                 's',    shell,          },
	{       "Compare directories",          'c',    cmpdir,         },
	{       "Find file            ^K",      'f',    findname,       },
//	{       "hIstory              ^B",      'i',    histmenu,       },
# ifndef COMPAT32
	{       "",                             0,      0,              },
# endif
//	{       "hOme directory       ^\\",     'o',    directory,      },
	{       "Root directory       ^^",      'r',    directory,      },
	{       "reDraw screen        ^]",      'd',    VRedraw,        },
# ifndef COMPAT32
//	{       "",                             0,      0,              },
//	{       "Menu file edit",               'm',    menuedit,       },
//	{       "eXtensions file edit",         'x',    extedit,        },
# endif
	{       "",                             0,      0,              },
	{	"Ident",			0,	Ident,		},
	{       0,                              0,      0,              },
};

static struct submenu omenu [] = {
	{       "sWitch panels      ^I",        'w',    switchpanels,   },
	{       "Swap panels        ^U",        's',    swappanels,     },
	{       "Full screen        ^F",        'f',    fullscreen,     },
	{       "Double width       ^W",        'd',    setdwid,        },
	{       "Command line mode  ^P",        'c',    switchcmdreg,   },
# ifndef COMPAT32
	{       "show hiddeN files",            'n',    switchhidden,   },
	{       "",                             0,      0,              },
# endif
//	{       "Viewer...",                    'v',    setviewname,    },
//	{       "Editor...",                    'e',    seteditname,    },
//	{       "sHell...",                     'h',    setshellname,   },
# ifndef COMPAT32
//	{       "",                             0,      0,              },
# endif
	{       "sAve setup",                   'a',    writeinitfile,  },
	{       0,                              0,      0,              },
};

struct menu menu [] = {
	{       "Left",         'l',            lmenu,  },
	{       "Files",        'f',            fmenu,  },
	{       "Commands",     'c',            cmenu,  },
	{       "Options",      'o',            omenu,  },
	{       "Right",        'r',            rmenu,  },
	{       0,              0,              0,      },
};

/* local */
void MENU::initmenu (void) {
	register struct menu *m;
	register struct submenu *sm;
	int col, w;

	col = 3;
	for (m=menu; m->name; ++m) {
		m->len = 4 + strlen (m->name);
		m->col = col;
		col += m->len;
		m->hgh = 0;
		m->wid = 0;
		for (sm=m->submenu; sm->name; ++sm) {
			sm->row = m->hgh;
			w = 4 + strlen (sm->name);
			if (w > m->wid)
				m->wid = w;
			++m->hgh;
		}
	}
}

/* local */
void MENU::getboxes (void) {
	register struct menu *m;

	for (m=menu; m->name; ++m) {
		if (m->box)
			VFreeBox (m->box);
		m->box = VGetBox (1, m->col-1, m->hgh+3, m->wid+4);
	}
}

/* local */
void MENU::clrmenu (register struct menu *m) {
	VUngetBox (m->box);
}

/* local */
void MENU::drawhead (int nm) {
	register i, reverse;

	VSetDim ();
	reverse = VStandOut ();
	VMove (0, 0);
	for (i=0; i<80; ++i)
		VPutChar (' ');
	VMove (0, 2);
	for (i=0; menu[i].name; ++i) {
		if (i == nm) {
			VStandEnd ();
			VSetBold ();
		}
		VPutChar (' ');
		VPutChar (i==nm && !reverse ? '[' : ' ');
		VPutString (menu[i].name);
		VPutChar (i==nm && !reverse ? ']' : ' ');
		VPutChar (' ');
		if (i == nm) {
			VStandOut ();
			VSetDim ();
		}
	}
	VSetNormal ();
	VStandEnd ();
}

/* local */
void MENU::drawmenu (register struct menu *m) {
	register i, reverse;

	VClearBox (1, m->col-1, m->hgh+3, m->wid+4);
	VSetBold ();
	VDrawBox (1, m->col, m->hgh+2, m->wid+2);
	VSetDim ();
	for (i=0; m->submenu[i].name; ++i)
		if (! m->submenu[i].name[0])
			VHorLine (i+2, m->col+2, m->wid-2);
	VSetNormal ();
	for (i=0; m->submenu[i].name; ++i) {
		if (! m->submenu[i].name[0])
			continue;
		if (i == m->nsm)
			reverse = VStandOut ();
		else if (! m->submenu[i].active)
			VSetDim ();
		VMove (2+i, m->col+1);
		VPutChar (i==m->nsm && !reverse ? '[' : ' ');
		printw ("%c %-*s", m->submenu[i].tag ? '*' : ' ',
			m->wid-4, m->submenu[i].name);
		VPutChar (i==m->nsm && !reverse ? ']' : ' ');
		if (i == m->nsm)
			VStandEnd ();
		else if (! m->submenu[i].active)
			VSetNormal ();
	}
}

/* local */
void MENU::upmenu (register struct menu *m) {
	for (;;) {
		if (--m->nsm < 0) {
			for (m->nsm=0; m->submenu[m->nsm].name; ++m->nsm);
			--m->nsm;
		}
		if (m->submenu[m->nsm].active)
			break;
	}
}

/* local */
int MENU::hotkey (int c, register struct menu *m) {
	register nsm, ret;

	if (c>='A' && c<='Z') {
		c += 'a' - 'A';
		ret = 2;
	} else
		ret = 1;
	for (nsm=0; m->submenu[nsm].name; ++nsm)
		if (c == m->submenu[nsm].key && m->submenu[nsm].active) {
			m->nsm = nsm;
			return (ret);
		}
	return (0);
}

/* local */
void MENU::downmenu (register struct menu *m) {
	for (;;) {
		if (! m->submenu[++m->nsm].name)
			m->nsm = 0;
		if (m->submenu[m->nsm].active)
			break;
	}
}

/* local */
int MENU::getmenu (void) {
	if (! menu[0].len)
		initmenu ();

	getboxes ();
	VMPutString (LINES-1, 0, "\0011\16      \17 2\16      \17 3\16      \17 4\16      \17 5\16      \17 6\16      \17 7\16      \17 8\16      \17 9\16      \01710\16Quit \17\2");
	for (;;) {
		drawhead (nmenu);
		for (;;) {
			int c, k;

			drawmenu (&menu[nmenu]);
			hidecursor ();
			VSync ();
			c = KeyGet ();
			switch (c) {
			default:
				k = hotkey (c, &menu[nmenu]);
				if (k) {
					clrmenu (&menu[nmenu]);
					return (k);
				}
				VBeep ();
				continue;
			case cntrl (']'):          /* redraw screen */
				VRedraw ();
				continue;
			case cntrl ('M'):
				clrmenu (&menu[nmenu]);
				return (1);
			case cntrl ('J'):
				clrmenu (&menu[nmenu]);
				return (2);
			case cntrl ('C'):
			case cntrl ('['):
			case meta ('J'):        /* f0 */
				clrmenu (&menu[nmenu]);
				return (0);
			case meta ('r'):        /* right */
				clrmenu (&menu[nmenu]);
				if (! menu[++nmenu].name)
					nmenu = 0;
				break;
			case meta ('l'):        /* left */
				clrmenu (&menu[nmenu]);
				if (--nmenu < 0) {
					for (nmenu=0; menu[nmenu].name; ++nmenu);
					--nmenu;
				}
				break;
			case meta ('u'):        /* up */
				upmenu (&menu[nmenu]);
				continue;
			case meta ('d'):        /* down */
				downmenu (&menu[nmenu]);
				continue;
			}
			break;
		}
	}
}

/* local */
int MENU::tag (int k, int sk) {
	switch (k) {
	case 'l':
		switch (sk) {
		case 'b':       return (left->d.full==0);
		case 'l':       return (left->d.full==1 || !fullwin && left->d.full==2);
		case 'f':       return (fullwin && left->d.full==2);
		case 's':       return (right->d.status);
		case 'p':       return (left->d.pattern[0]);
		case 'n':       return (left->d.sort == SORTNAME);
		case 'x':       return (left->d.sort == SORTEXT);
		case 't':       return (left->d.sort == SORTTIME);
		case 'z':       return (left->d.sort == SORTSIZE);
		case 'u':       return (left->d.sort == SORTSKIP);
		case 'a':       return (left->d.alignext);
		case 'y':       return (left->d.typesort);
		case 'r':       return (left->d.revsort);
		}
		break;
	case 'r':
		switch (sk) {
		case 'b':       return (right->d.full==0);
		case 'l':       return (right->d.full==1 || !fullwin && right->d.full==2);
		case 'f':       return (fullwin && right->d.full==2);
		case 's':       return (left->d.status);
		case 'p':       return (right->d.pattern[0]);
		case 'n':       return (right->d.sort == SORTNAME);
		case 'x':       return (right->d.sort == SORTEXT);
		case 't':       return (right->d.sort == SORTTIME);
		case 'z':       return (right->d.sort == SORTSIZE);
		case 'u':       return (right->d.sort == SORTSKIP);
		case 'a':       return (right->d.alignext);
		case 'y':       return (right->d.typesort);
		case 'r':       return (right->d.revsort);
		}
		break;
	case 'o':
		switch (sk) {
		case 's':       return (left->d.basecol != 0);
		case 'f':       return (H == LINES-7);
		case 'h':       return (usecshell);
		case 'd':       return (fullwin);
		case 'c':       return (cmdreg);
		case 'v':       return (userview);
		case 'e':       return (useredit);
		case 'n':       return (showhidden);
		}
		break;
	}
	return (0);
}

/* public: */
void MENU::runmenu (int mkey) {
	register struct menu *m;
	register struct submenu *sm;
	int ret;

	nmenu = 0;
	for (m=menu; m->name; ++m) {
		if (m->key == mkey)
			nmenu = m-menu;
		for (sm=m->submenu; sm->name; ++sm)
			sm->active = sm->exec != 0;
		m->nsm = -1;
		downmenu (m);
	}
oncemore:
	for (m=menu; m->name; ++m) {
		for (sm=m->submenu; sm->name; ++sm) {
			sm->active = sm->exec != 0;
			sm->tag = tag (m->key, sm->key);
		}
	}
	ret = getmenu ();
	if (! ret)
		return;
	m = &menu[nmenu];
	sm = &m->submenu[m->nsm];
	if (sm->exec)
		(*sm->exec) (m->key, sm->key);
	if (ret > 1)
		goto oncemore;
}
