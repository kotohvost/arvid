#include <stdio.h>

main (int arg, char *argv[]) {
char	*filename[80];
FILE	*file;
int	keynum;
char	*keyname[11];
char	*tmpname[11];

	if (arg != 3) {
	/* Имя файла не передано */
		printf("\nUsage: %s <vcrfile> <keyname | numkey>\n", argv[0]);
		exit(1);
	}
	strcpy((char *)filename, argv[1]);
	file = fopen ((char *)filename, "rb");
	/* Открытие успешно? */
	if (file == 0) {
		printf("\nError opening %s\n", argv[1]);
		exit(2);
	}
	/* опpеделяем номер или имя клавиши */
	if (isdigit(*argv[2])) {
	long	offs;

		keynum = atoi(argv[2]);
		if (keynum >= 15) keynum = 0;
		printf("keynum: %d, ", keynum);
		offs = 0xa6e + (keynum * 10);
		if (fseek(file, offs, SEEK_SET) != 0) {
			printf("Error seek file %s", argv[1]);
			fclose(file);
			exit (3);
		}
		fgets((void *)keyname, 10, file);
		keyname[11] = '\0';
		printf("keyname: %s", keyname);
	} else {
		keynum = 0;
		strcpy((char *)keyname, argv[2]);
		printf("keyname: %s, ", keyname);
		if (fseek(file, 0x0a6e, SEEK_SET) != 0) {
			printf("Error seek file %s", argv[1]);
			fclose(file);
			exit (3);
		}
		while ((fgets((void *)tmpname, 10, file) != 0) && (keynum < 15)) {
		long	offs;
			if (strcasecmp((char *)keyname, (char *)tmpname) == 0) {
				printf("keynum: %d\n", keynum);
				fclose(file);
				exit(0);
			}
			keynum++;
			offs = 0xa6e + (keynum * 10);
			if (fseek(file, offs, SEEK_SET) != 0) {
				printf("Error seek file %s", argv[1]);
				fclose(file);
				exit (3);
			}
		}
	}
	printf("\n");
	fclose(file);
}
