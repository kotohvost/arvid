#ifdef __cplusplus
extern "C" {
#endif

#define hidecursor()	VMove (LINES-1, COLS-2)
#define mvcaddstr(r, c, s)	VMPutString (r, c - (strlen (s) + 1) / 2, s)

char *strtail (register char *p, int delim, int maxlen);
char *extension (register char *p);
/* int strbcmp(register char *s, register char *b); */
/* char *ltoac (long l); */
void outerr (char *s, char *par);
/* char *getmachine(void); */

#ifdef __cplusplus
}
#endif