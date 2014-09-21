#Makefile avcopier.

CC = gcc
CFLAGS		= -O2

all: main

OBJR		= ../../include/avt_rs.o ../../include/crc.o main.cpp

main:	$(OBJR)
		$(CC) -o main $(OBJR)
clean:
		rm -f main *.o *~

