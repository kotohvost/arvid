#include "Tdr.hpp"
#include "Cfg.hpp"

#define	SECTOR	1165141
#define	TIME	2900

TDR	Tdr;

int main(int argc, char *argv[]) {
char	strbuf[1024];
int	tlen, time;

	if (argc != 2) {
		printf("Usage: \n");
		return(1);
		};
	tlen = strlen(CfgTdr);
	tlen += strlen(argv[1]);
	tlen++;
	if (tlen >= sizeof(strbuf)) {
		printf("Path to '%s' is long\n", argv[1]);
		return(1);
		};
	sprintf(strbuf, "%s/%s", CfgTdr, argv[1]);
	if (Tdr.LoadTDR(strbuf)) {
		printf("Error load '%s'\n", argv[1]);
		return(1);
		};
	time = Tdr.SectToSec(SECTOR);
	if (time) {
		printf("Sector %u: %u\n", SECTOR, time);
		printf("Find: %d\n", time - TIME);
		}
	else
		printf("Sector %u: not found\n", SECTOR);
};
