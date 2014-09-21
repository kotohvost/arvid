#Makefile avinfo.

CC = gcc
CFLAGS		= -O2

all: rsd readrsd rsc

OBJD100		= ../include/avt100.o d100.o
OBJRSD		= ../include/avt_rs.o rsd.o
OBJRRSD		= ../include/avt_rs.o readrsd.o
OBJRSC		= ../include/avt_rs.o rsc.o

d100:		$(OBJD100)
		$(CC) -o d100 $(OBJD100)
rsd:		$(OBJRSD)
		$(CC) -o rsd $(OBJRSD)
readrsd:	$(OBJRRSD)
		$(CC) -o readrsd $(OBJRRSD)
rsc:		$(OBJRSC)
		$(CC) -o rsc $(OBJRSC)
clean:
		rm -f rsd *.o *~

