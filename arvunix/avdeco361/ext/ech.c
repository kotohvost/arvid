# include <stdio.h>

char *progname = "ech";

int nflag;

main (argc, argv)
char **argv;
{
	char *p;

	for (++argv; *argv && **argv=='-'; ++argv)
		for (p=1+*argv; *p; ++p)
			switch (*p) {
			case 'n':
				++nflag;
				break;
			default:
				fprintf (stderr, "%s: unknown flag -%c\n",
					progname, *p);
				break;
			}

	for (; *argv; ++argv) {
		echo (*argv);
		if (*(argv+1))
			putchar (' ');
	}
	if (! nflag)
		putchar ('\n');
	return (0);
}

echo (str)
char *str;
{
	register c, i;

	while (c = *str++) {
		if (c == '\\') {
			switch (c = *str++) {
			case 'n':       c = '\n';       break;
			case 'e':       c = '\33';      break;
			case 'f':       c = '\f';       break;
			case 'r':       c = '\f';       break;
			case 'c':
				++nflag;
				return;
			case '0': case '1': case '2':
				i = c & 3;
				if (*str >= '0' && *str <= '9') {
					c = *str++;
					i = i << 3 | c & 7;
					if (*str >= '0' && *str <= '9') {
						c = *str++;
						i = i << 3 | c & 7;
					}
				}
				c = i;
				break;
			case '3': case '4': case '5': case '6': case '7':
				i = c & 7;
				if (*str >= '0' && *str <= '9') {
					c = *str++;
					i = i << 3 | c & 7;
				}
				c = i;
				break;
			}
		}
		putchar (c);
	}
}
