#Makefile avcopier.

CC = gcc
CFLAGS		= -O2
INCDIR		= -I../../include

all: avread avreadrs write

OBJR		= avread.o
OBJRS		= avreadrs.o
OBJW		= avwrite.o
OBJW1		= write.o

avwrite:	$(OBJW)
		$(CC) -o avwrite $(OBJW)
avwriteg:
		$(CC) $(INCDIR) -o avwriteg avwriteg.c
avwriters:
		$(CC) $(INCDIR) -o avwriters avwriters.c
avwriteirc:	avwriteirc.c
		$(CC) $(INCDIR) -o avwriteirc avwriteirc.c
avread:
		$(CC) $(INCDIR) -o avread avread.c
avreadrs:	$(OBJRS)
		$(CC) -o avreadrs $(OBJRS)
write:		$(OBJW1)
		$(CC) -o write $(OBJW1)
clean:
		rm -f avread avreadrs *.o *~

avwriters: avwriters.c

