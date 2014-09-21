/* main, menu */
void directory(int k, int sk);
void switchcmdreg (void);
void switchpanels (void);
void setstatus (void);
void reread (dir *d);
void setdwid (void);
void view (void);
void edit (void);
void quit (void);
void swappanels (void);
void copy (void);
void renmove (void);
void makedir (void);
void mydelete (void);
void findname (void);
void tagall (void);
void untagall (void);
/* menu */
void switchalign (int k);
void switchhidden (void);
void setfull (int k, int sk);
void setsort (int k, int sk);
void settypesort (int k);
void setrevsort (int k);
void setviewname (void);
void setshellname (void);
void seteditname (void);
void menuedit (void);
void extedit (void);
void shell (void);
void makelink (void);
void setpattern (int k);
void cmpdir (void);
/* com, main, menu */
void fullscreen (void);
/* com */
void syscmd (char *s);
int tagged (void);

# ifdef S_IFLNK
/* menu */
void makeslink (void);
# endif /* S_IFLNK */

void Ident(void);
