#Makefile avvcrload & viewvcr.
#Makefile avcopier.

CC = gcc
CFLAGS		= -O2

all: vcread vcrwrite vcrload viewvcr

OBJR		= vcread.o
OBJW		= vcrwrite.o
OBJL		= vcrload.o
OBJV		= viewvcr.o

vcrload:	$(OBJL)
		$(CC) -o vcrload $(OBJL)
viewvcr:	$(OBJV)
		$(CC) -o viewvcr $(OBJV)
vcrwrite:	$(OBJW)
		$(CC) -o vcrwrite $(OBJW)
vcread:		$(OBJR)
		$(CC) -o vcread $(OBJR)
clean:
		rm -f vcread vcrwrite vcrload viewvcr *.o *~
