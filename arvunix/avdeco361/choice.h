char *getwstring (register w, char *str, char *head, char *mesg);
char *getstring (register w, char *str, char *head, char *mesg);
int getchoice (int bold, char *head, char *msg, char *mesg2, char *c1, char *c2, char *c3);
void endmesg(void);
void error (char *s, long aa=0, long bb=0, long cc=0, long dd=0);
void message(char *name, char *s, long aa=0, long bb=0, long cc=0, long dd=0);
