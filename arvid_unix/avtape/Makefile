#Makefile avcopier.

CC = gcc
CFLAGS		= -O2

all: tape

OBJT		= ../include/avt_rs.o ../include/crc.o tape.o

tape:	$(OBJT)
	$(CC) -o tape $(OBJT)
clean:
		rm -f tape *.o *~

