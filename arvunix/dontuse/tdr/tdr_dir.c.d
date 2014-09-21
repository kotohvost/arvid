/*
 * tdr_dir.c
 *
 * Читает заголовок TDR файла и выводит на экран его компоненты.
 * Берет один параметр из командной строки - полное имя файла TDR.
 *
 * Писатель:	Владимир Зарожевский (Vladimir Zarozhevsky)
 * Адрес:       2:5050/2.10@fidonet,
 *		vladimir@c-press.udm.ru,
 *		vladimir@mark-itt.ru
 *
 * "Создан по мотивам романа 'Пограммирование DBF Clipper на C'"
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
/* For DOS */
/* #include "io.h" */
#include "../include/tdr.h"	/* Описывает TDR_HEAD */

/* глобальные указатели */
	/* Сюда считывается заголовок */
TDR_HEAD	tdr_head;
	/* флаг старого формата */
int		oldtdr = 0;
	/* Сюда считывается добавочные поля */
TDR_ADD_HEAD	tdr_add_head;
	/* указатель на область описателей и ее размер */
unsigned char	*desc, *ndesc;
unsigned long	sizedesc;

/*
 * Распечатывание полей структуры TDR_FILES.
 * Применяется для TDR_FILES и TDR_DIR.
 * fmt - указатель на строку указывающую формат распечатки заканчивается '\0'
 * N - name, E - ext, A - attr, S - startsect
 * D - startdesc, T - time, Z - size
 * ' ' - разделитель пробел
 * '.' - разделитель точка
 * '\t' - разделитель табуляция
 * '\n' - а это перевод строки :)
 */
void printf_tdr_files(TDR_FILES *tdr_files, char *fmtp)
{
int	i;
char	fmt;	/* символ текущего формата */

	while (fmt = *fmtp++)
		switch (fmt) {
		case 'N':	/* Имя файла */
			for(i=0; i<8; i++)
				printf("%c", tdr_files->name[i]);
			break;
		case 'E':	/* расширение файла */
			for(i=0; i<3; i++)
				printf("%c", tdr_files->ext[i]);
			break;
		case 'A':	/* Аттрибуты */
			if (tdr_files->attr & 0x20)
				printf("A");
			else
				printf(".");
			if (tdr_files->attr & 0x10)
				printf("D");
			else
				printf(".");
			if (tdr_files->attr & 0x08)
				printf("V");
			else
				printf(".");
			if (tdr_files->attr & 0x04)
				printf("S");
			else
				printf(".");
			if (tdr_files->attr & 0x02)
				printf("H");
			else
				printf(".");
			if (tdr_files->attr & 0x01)
				printf("R");
			else
				printf(".");
			break;
		case 'S':	/* Стартовый сектор */
			printf("%10lu", tdr_files->startsect);
			break;
		case 'D':	/* Описатель */
			if ((tdr_files->startdesc < sizedesc) &&
				(tdr_files->startdesc != 0)) {
				int	tmps;
				ndesc = desc + tdr_files->startdesc - 1;
				tmps = *ndesc++;
/*				printf("%lu, %u", tdr_files->startdesc, tmps); */
			/* где начало описания? */
				while ((tmps--) && ((ndesc++ - desc) < sizedesc))
					printf("%c", *ndesc);
				}
			break;
		case 'T':	/* Время */
			printf("%02u:%02u:%02u %02u-%02u-%04u",
				(tdr_files->time >> 11), ((tdr_files->time >> 5) & 0x3f),
				(tdr_files->time & 0x1f), (tdr_files->date & 0x1f),
				((tdr_files->date >> 5) & 0x0f), ((tdr_files->date >> 9)+1980));
			break;
		case 'Z':	/* Размер */
			printf("%10lu", tdr_files->size);
			break;
		default:	/* пробелы, точки, табуляции и т.д. т.п. */
			printf("%c", fmt);
			break;
		}; /* switch */
};

/*
 * Распечатка файлов соответственно формату
 */
void printf_files (int handle, char *fmtp)
{
int	numfiles;	/* количество файлов */
	/* Сюда считываются поля из таблицы файлов */
TDR_FILES	tdr_files;

/* работа с файлами */
	printf("\n------------------------------------------------------------------------\n");
/* указатель на таблицу файлов */
	lseek(handle, tdr_head.startf, SEEK_SET);
	for (numfiles=0; (read(handle, &tdr_files, sizeof(tdr_files)) == sizeof(TDR_FILES))
		&& (lseek(handle, 0, SEEK_CUR) < (tdr_head.startf+tdr_head.sizef)); numfiles++) {
	/* в старом оглавлении нет описаний */
		if (oldtdr)
			tdr_files.startdesc = 0;
	/* распечатываем одно поле из таблицы */
		printf_tdr_files(&tdr_files, fmtp);
		}
	printf("Found %u files\n", numfiles);
}

/*
 * Распечатка каталогов соответственно формату
 */
void printf_dir(int handle, char *fmtp)
{
int	numdir;		/* количество каталогов */
	/* Сюда считываются поля из таблицы каталогов */
TDR_DIR		tdr_dir;
	/* путь каталога - не более 256 символов */
unsigned char	pathd[256], *pathpd, *pathnd;
	/* уровень каталога */
unsigned int	leveld = 0;
int		extf;		/* флаг наличия расширения */
int		i;

printf("%u\n", sizeof(TDR_DIR));
/* очистим буфер */
/*	for(pathnd = pathd; pathnd < pathd+sizeof(pathd); pathnd++)
		*pathnd = '\0'; */
	bzero(pathd, sizeof(pathd));
/* приравняем указатели */
	pathnd = pathpd = pathd;
/* работа с каталогами */
	printf("\n------------------------------------------------------------------------\n");
/* указатель на таблицу каталогов */
	lseek(handle, tdr_head.startd, SEEK_SET);
	for (numdir=0; (read(handle, &tdr_dir, sizeof(tdr_dir)) == sizeof(TDR_DIR))
		&& (lseek(handle, 0, SEEK_CUR) < (tdr_head.startd+tdr_head.sized)); numdir++) {
	/* в старом оглавлении нет описаний */
		if (oldtdr)
			tdr_dir.d.startdesc = 0;
	/* присвоим startsect значение level директории */
		tdr_dir.d.startsect = tdr_dir.level;
	/* печать текущего уровня директории */
		/* текуший уровень = 0 - для профилактики :) */
		if (tdr_dir.level == 0) {
			pathnd = pathpd = pathd;
			}
#if 0
		/* предыдущий уровень */
		if (leveld > tdr_dir.level) {
			for(i=0; (leveld-i) >= tdr_dir.level; i++)
			/* пока не встретим "\" и не начало */
				while((*--pathpd != '\\') && (pathpd > pathd));
			pathpd++;
			}
		/* уровни равны - можно выбросить? */
		if (leveld == tdr_dir.level) {
			pathnd = pathpd;
			}
		/* следующий уровень - не больше +1 */
		if ((leveld+1) == tdr_dir.level) {
			pathpd = pathnd;
			}
		/* #%@#%#$ - а так просто :-I */
		if ((leveld+1) > tdr_dir.level) {
			}
#endif
		pathnd = pathpd;
		for(i=0; (i<8) & (tdr_dir.d.name[i] != ' '); i++) {
			*pathnd++ = tdr_dir.d.name[i];
	printf("%c", tdr_dir.d.name[i]);
			}
		extf = 0;
		for(i=0; (i<3) & (tdr_dir.d.ext[i] != ' '); i++) {
	printf("%c", tdr_dir.d.name[i]);
			if (extf == 0)
				*pathnd++ = '.';
			*pathnd++ = tdr_dir.d.ext[i];
			extf = 1;
			}
		*pathnd++ = '\\';
		*pathnd = '\0';
		printf("%s %lu,%lu, l:%u", pathd, pathd, pathnd-pathd, leveld);
		leveld = tdr_dir.level;
	/* распечатываем одно поле из таблицы */
		printf_tdr_files(&tdr_dir.d, fmtp);
/*		printf("\t%s\n", pathd); */
		printf("\n");
		}
	printf("Found %u directories\n", numdir);
}

void main(int arg, char *argv[])
{
int	handle;

	desc = ndesc = NULL;

/* Убедимся что имя файла передано */
	if (arg!=2) {
/* Имя файла не передано */
		printf("\n Писатель (Wrote):\tВладимир Зарожевский (Vladimir Zarozhevsky)");
		printf("\n Адрес (Addres):\t2:5050/2.10@fidonet,");
		printf("\n\t\t\tvladimir@c-press.udm.ru,");
		printf("\n\t\t\tvladimir@mark-itt.ru");
		printf("\n\nUsage: %s <filename>\n", argv[0]);
		printf("\nFormat:\tSee source\n");
		exit(1);
		}
/*	handle = open(argv[1], O_RDONLY|O_BINARY); */
	handle = open(argv[1], O_RDONLY);
/* Открытие успешно? */
	if (handle == -1) {
		printf("\nError opening %s\n", argv[1]);
		exit(2);
		}
/* Считаем заголовок в структуру и проверяем */
	if (read(handle, &tdr_head, sizeof(tdr_head)) != sizeof(TDR_HEAD)) {
		printf("\nRead error %s\n", argv[1]);
		exit(1);
		}
/* Теперь высветим поля */
	printf("\n******* TapeDirectoRy header %s *******\n", argv[1]);
	printf("Start Files ........ %08lx\n", tdr_head.startf);
	printf("Start Directory .... %08lx\n", tdr_head.startd);
	printf("Start PosTable ..... %08lx\n", tdr_head.startpt);
	printf("Files size ......... %08lx/%lu\n", tdr_head.sizef, tdr_head.sizef);
	printf("Directory size ..... %08lx/%lu\n", tdr_head.sized, tdr_head.sized);
	printf("PosTable size ...... %08lx/%lu\n", tdr_head.sizept, tdr_head.sizept);
	printf("\n");
	printf("Tape format ............ %04x\n", tdr_head.format);
	printf("Tape ID ................ %04x\n", tdr_head.ID);
	printf("Tape length ............ %04x/%us\n", tdr_head.length, tdr_head.length);
	printf("Tape save length ....... %04x/%us\n", tdr_head.savelen, tdr_head.savelen);
	printf("Next sector for write .. %08lx/%lu\n", tdr_head.nextsect, tdr_head.nextsect);

/* Считаем добавочные поля заголовка */
	if (tdr_head.startf == sizeof(TDR_HEAD) ||
		tdr_head.startd == sizeof(TDR_HEAD) ||
		tdr_head.startpt == sizeof(TDR_HEAD)) {
		printf("**** Old Tape format ****\n");
		oldtdr = 1;
		}
	else {
		if (read(handle, &tdr_add_head, sizeof(tdr_add_head)) != sizeof(TDR_ADD_HEAD)) {
			close(handle);
			printf("\nRead error %s\n", argv[1]);
			exit(1);
			}
		printf("Start description's ........ %08lx\n", tdr_add_head.startdes);
		printf("Size description's ......... %08lx\n", tdr_add_head.sizedes);
		printf("Last sector ............ %08lx/%lu\n", tdr_add_head.lastsect, tdr_add_head.lastsect);
		oldtdr = 0;
	}

/* загружаем дескрипторы */
	if (oldtdr == 0) {
		printf("Description's ...... ");
		lseek(handle, tdr_add_head.startdes, SEEK_SET);
	/* выделим область и приравняем указатели */
		desc = (char *)malloc(tdr_add_head.sizedes);
		if ((sizedesc=read(handle, desc, tdr_add_head.sizedes)) != tdr_add_head.sizedes) {
			printf("Error read 'Description table' - size:%u/",
				tdr_add_head.sizedes);
			}
		printf("Read: %u byte\n", sizedesc);
		}
#if 0
/* работа с файлами */
	printf_files(handle, "S Z T A N E\tD\n");
#endif
/* работа с каталогами */
	printf_dir(handle, "\tD");

	close(handle);
/* освободим область */
	if (desc)
		free(desc);
}
