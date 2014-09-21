#include "Sector.hpp"

int main (int argc, char *argv[]) {
SectorList	SL;
char		tbuf[80], *buf;
int		news, i;
int		st, send, slog;

	news = 1;
	buf = tbuf;
	st = send = slog = 0;
	// ввод значений
	while (news) {
		if (st == 0) {
			printf("Enter start sector: ");
			if (buf=fgets(tbuf, sizeof(tbuf), stdin))	st = atoi(buf);
			else					news = 0;
			};
		if ((send == 0) && news) {
			printf("Enter end sector: ");
			if (fgets(tbuf, sizeof(tbuf), stdin))	send = atoi(buf);
			else					news = 0;
			if (send == 0)				news = 0;
			};
		if ((slog = 0) && news) {
			printf("Enter start logic sector: ");
			if (fgets(tbuf, sizeof(tbuf), stdin))	slog = atoi(buf);
			};
		if (news) {
			printf("Enter filename: ");
			if (fgets(tbuf, sizeof(tbuf), stdin)) {
/*				for (i=0; !SL.Insert(slog, st, send, buf) && (i<65535); i++); */
				if (SL.Add(slog, st, send, buf, NULL, 10)) {
					printf("ERROR: Insert fault\n");
					news = 0;
					}
				else	{
					st = send = slog = 0;
					};
				};
			};
		}; /* while */
	SL.PrintAll();
};
