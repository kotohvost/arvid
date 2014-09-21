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
 *      EnvInit (char **env)
 *              Initialixe Env packadge by environment vector.
 *
 *      char *EnvGet (char *name)
 *              Get variable from environment.
 *
 *      EnvPut (char *name, char *value)
 *              Change value of variable.
 *
 *      char **EnvVector
 *              Current environment vector.
 */
#include "string.h"
# include "mem.h"

# define QUANT 10

char **EnvVector;

static nenv;
static last;

/* main */
void EnvInit(char **env) {
	register char **p;
	register i;

	if (EnvVector) {
		for (p=EnvVector; *p; ++p)
			MemFree ((mem *) *p);
		MemFree ((mem *) EnvVector);
		EnvVector = 0;
		nenv = 0;
		last = nenv = 0;
	}
	if (! env)
		return;
	for (i=0, p=env; *p; ++p, ++i) {
		MemCheckIndex (EnvVector, char **, nenv, QUANT, i);
		EnvVector [i] = (char *) MemAlloc (strlen (*p) + 1);
		strcpy (EnvVector [i], *p);
	}
	MemCheckIndex (EnvVector, char **, nenv, QUANT, i);
	last = i;
	EnvVector [last] = 0;
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 *	used for environment searching: see getenv
 */
/* local */
char *nvmatch (register char *s1, register char *s2) {
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return (s2);
	if (*s1 == '\0' && *(s2-1) == '=')
		return (s2);
	return (0);
}

/* local */
char *findvar (register char *name, char ***ptr) {
	register char *v, **p;

	p = EnvVector;
	if (! p)
		return (0);
	for (; *p; ++p)
		if (v = nvmatch (name, *p)) {
			*ptr = p;
			return (v);
		}
	return (0);
}

/* com */
void EnvPut (char *name, char *value) {
	register char *v;
	char **p;

	if (! name)
		return;
	if (! value)
		value = "";
	v = findvar (name, &p);
	if (v)
		MemFree ((mem *) *p);
	else {
		p = EnvVector + last;
		++last;
		MemCheckIndex (EnvVector, char **, nenv, QUANT, last);
		EnvVector [last] = 0;
	}
	*p = (char *) MemAlloc (strlen (name) + 1 + strlen (value) + 1);
	strcpy (*p, name);
	strcat (*p, "=");
	strcat (*p, value);
}

/* com */
void EnvDelete (char *name) {
	register char *v;
	char **p;

	if (! name)
		return;
	v = findvar (name, &p);
	if (! v)
		return;
	MemFree ((mem *) *p);
	for (++p; *p; ++p)
		*(p-1) = *p;
	EnvVector [--last] = 0;
}

/* cap, help, main, run, scr, tm */
char *EnvGet (char *name) {
	char **p;

	return (findvar (name, &p));
}
