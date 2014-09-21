#Makefile avcopier.

CC = gcc
CFLAGS		= -O2

all: avcopier avreadtof

OBJS		= avcopier.o
OBJR		= ../include/avt_rs.o ../include/crc.o avreadtof.o

avcopier:	$(OBJS)
		$(CC) -o avcopier $(OBJS)
avreadtof:	$(OBJR)
		$(CC) -o avreadtof $(OBJR)
clean:
		rm -f avcopier avreadtof *.o *~

