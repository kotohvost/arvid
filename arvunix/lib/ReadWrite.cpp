#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ReadWrite.h"

#define	MIN_ALLOC_FILES	25

/*
 * Constructor
 */
ReadFiles::ReadFiles() {
	numList = 0;
	alloc = 0;
	list = NULL;
};

/*
 * Destructor
 */
ReadFiles::~ReadFiles() {
int	i;

	if (list) {
		for (i = 0; i < numList; i++) {
			fprintf(stdout, "From: %s\n",list[i].from);
			fprintf(stdout, "To: %s\n",list[i].to);
			fprintf(stdout, "\n");
		}
		free(list);
	}
};

/*
 * Добавление запроса.
 * Сопируем входную структуру в массив,
 * если необходимо выделяем дополнительную память.
 */
int ReadFiles::AddReadFile(ReadWrite *rw) {
int	rc = 0;

	if (numList == alloc) {
		alloc += MIN_ALLOC_FILES;
		list = (ReadWrite *)realloc(list, alloc * sizeof(ReadWrite));
		memset(&list[numList], 0, MIN_ALLOC_FILES * sizeof(ReadWrite));
	}
	memcpy(&list[numList], rw, sizeof(ReadWrite));
	numList++;
	return rc;
};
