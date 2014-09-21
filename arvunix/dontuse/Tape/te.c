#include "mystring.h"

void main(int argc, char *argv[]) {

	mystring xx = "sss";
	printf("mystring: %s\n", xx);

	mystring yy;

	yy = " add";
	printf("mystring: = %s\n", yy);

	mystring ss = yy + xx;
	printf("mystring: + %s\n", ss);

	yy = xx;
	printf("mystring: =& %s\n", yy);
};
