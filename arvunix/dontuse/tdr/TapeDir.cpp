#include "TapeDir.hpp"

void main(int argc, char *argv[]) {
TapeDir		Td;
DirFile_t	name;
int		i, f;
char		str[128];

//	getchar();

	/* директории */
#if 1
	for (i=0; i < 100; i++) {
		sprintf(str, "Num for directory %04d", i);
		name.Name = str;
		name.Desc = "Описание директории для тестирования";
		name.Attr = 0xffffffffL;
		name.Dtime = 857463542L;
//		printf("Err: %d\n", Td.InsD(&name));
		Td.InsD(&name);
		Td.Cd(str);
#if 1
	for (f=0; f < 100; f++) {
		sprintf(str, "Num for files %04d", f);
		name.Name = str;
		name.Desc = "Описание файла для тестирования";
		name.Attr = 0xffffffffL;
		name.Dtime = 857463542L;
//		printf("Err: %d\n", Td.InsF(&name));
		Td.InsF(&name);
		};
#endif
		Td.Cd(".");
//		Td.Cd("..");
		};
		Td.Cd("../../../../../../..");
#else
	char *t1 = (char *)malloc(100);
	for (i=0; i<10; i++)		t1[i] = i;

	char *t2 = (char *)malloc(200);
	for (i=10; i<20; i++)		t2[10-i] = i;

	t1 = (char *)realloc(t1, 110);
	t2 = (char *)realloc(t2, 210);
	for (i=0; i<10; i++)	printf("i=%d, ", t1[i]);	printf("\n");
	for (i=10; i<20; i++)	printf("i=%d, ", t2[10-i]);	printf("\n");

#endif
	/* File */
#if 1
	for (i=0; i < 100; i++) {
		sprintf(str, "Num for files %04d", i);
		name.Name = str;
		name.Desc = "Описание файла для тестирования";
		name.Attr = 0xffffffffL;
		name.Dtime = 857463542L;
//		printf("Err: %d\n", Td.InsF(&name));
		Td.InsF(&name);
		};
#endif
#if 0
	name.Name = "New Directory";
	name.Desc = "Comment for 'New Directory'";
	name.Attr = 0xffffffffL;
	name.Dtime = 857463542L;
	printf("Err: %d\n", Td.InsF(&name));

	name.Name = "New File";
	name.Desc = "Comment for 'New File'";
	name.Attr = 0xffffffffL;
	name.Dtime = 857463542L;
	printf("Err: %d\n", Td.InsF(&name));

	name.Name = "New File";
	name.Desc = "Comment for 'New File'";
	name.Attr = 0xffffffffL;
	name.Dtime = 857463542L;
	printf("Err: %d\n", Td.InsF(&name));

	name.Name = "New File";
	name.Desc = "Comment for 'New File'";
	name.Attr = 0xffffffffL;
	name.Dtime = 857463542L;
	printf("Err: %d\n", Td.InsD(&name));
#endif

	Td.Cd("/Num/for/directory");
	Td.Dir();
//	getchar();
};
