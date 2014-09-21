#Makefile avcopier.

CC = gcc
CFLAGS		= -O2

all: avread avwrite

OBJR		= ../include/avt_rs.o avread.o
OBJW		= ../include/avt_rs.o avwrite.o

avwrite:	$(OBJW)
		$(CC) -o avwrite $(OBJW)
avread:		$(OBJR)
		$(CC) -o avread $(OBJR)
clean:
		rm -f avread avwrite *.o *~

