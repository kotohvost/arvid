#Makefile avcopier.

CC = gcc
CFLAGS		= -O2

all: avread avreadrs

OBJR		= avread.o
OBJRS		= avreadrs.o
OBJW		= avwrite.o

avwrite:	$(OBJW)
		$(CC) -o avwrite $(OBJW)
avread:		$(OBJR)
		$(CC) -o avread $(OBJR)
avreadrs:	$(OBJRS)
		$(CC) -o avreadrs $(OBJRS)
clean:
		rm -f avread avreadrs *.o *~

