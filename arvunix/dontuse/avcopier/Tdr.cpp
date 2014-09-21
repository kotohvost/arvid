/*
 * tdr_dir.cpp
 */

#include "Tdr.hpp"	/* Описывает TDR_HEAD */

void main(int argc, char *argv[])
{
TapeDir	Tdr;

	if (argc != 2) {
		printf("\nUsage: %s file.tdr\n", argv[0]);
		exit(1);
		};
//	printf("TDR_DIR: %d\n", sizeof(TDR_DIR));
	if (Tdr.LoadTDR(argv[1])) {
		printf("Error LoadTDR()\n");
		exit(1);
		};
//	Tdr.Dir(stdout);
}
