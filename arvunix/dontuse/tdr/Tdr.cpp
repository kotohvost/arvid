/*
 * tdr_dir.cpp
 */

// #include "TapeDir.hpp"
#include "Tdr.hpp"	/* Описывает TDR_HEAD */

void main(int argc, char *argv[])
{
TDR	Tdr;

	if (argc != 2) {
		printf("\nUsage: %s file.tdr\n", argv[0]);
		exit(1);
		};
//	printf("TapeDir: %d\n", sizeof(TapeDir_t));
	printf("long: %d\n", sizeof(long));
	printf("TDR_FILESn: %d\n", sizeof(TDR_FILESn));
	printf("TDR_DIR: %d\n", sizeof(TDR_DIR));
	if (Tdr.LoadTDR(argv[1])) {
		printf("Error LoadTDR()\n");
		exit(1);
		};
	Tdr.Dir(stdout);
}
