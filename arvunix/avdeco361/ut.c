/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

/* draw, file */
char *extension (register char *p) {
	register char *s;

	if (p[0]=='.' && p[1]=='.' && p[2]==0)
		return (p+2);
	s = 0;
	while (*++p)
		if (*p == '.')
			s = p;
	return (s ? s : p);
}

/* edit, ex */
char *basename (register char *p) {
	register char *s, *e;
	static char buf [32];

	/* convert "zzz/name" to "name" */
	for (s=p; *s; ++s)
		if (*s == '/')
			p = s+1;
	if (! *p)
		return ("");

	/* convert "name.ext" to "name" */
	for (e=s, s=p+1; *s; ++s)
		if (*s == '.')
			e = s;
	strncpy (buf, p, e-p);
	buf [e-p] = 0;
	return (buf);
}

/* cmd, dir  */
char *strtail (register char *p, int delim, int maxlen) {
	register char *s;

	/* return tail of string */
	for (s=p; *s; ++s);
	s -= maxlen;
	if (s <= p)
		return (p);
	for (; *s; ++s)
		if (*s == delim)
			return (s+1);
	return (s - maxlen);
}

/* VARARGS 1 */
/* cap, cmd,  com, main, run */
void outerr (char *s, char *par) {
	char buf [80];
	register char *p;

	sprintf (buf, s, par);
	for (p=buf; *p; ++p)
		if (*p == '\033')
			*p = '[';
	write (2, buf, (unsigned) (p - buf));
}
