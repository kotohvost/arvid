#Makefile avinfo.

CC = gcc
CFLAGS		= -O2

all: rsd readrsd rsc

OBJD100		= ../../lib/avt100.o d100.o
OBJRSD		= ../lib/rs.o rsd.o
OBJRSD1		= ../lib/rs.o rsd.o
OBJRRSD		= ../../lib/avt_rs.o readrsd.o
OBJRSC		= ../../lib/avt200.o rsc.o

INCLUDE		= -I../include

.c.o:
	$(CC) $(INCLUDE) -c $<

d100:		$(OBJD100)
		$(CC) -o d100 $(OBJD100)
rsd:		$(OBJRSD)
		$(CC) -o rsd $(OBJRSD)
rsd1:		$(OBJRSD1)
		$(CC) -o rsd1 $(OBJRSD1)
readrsd:	$(OBJRRSD)
		$(CC) -o readrsd $(OBJRRSD)
rsc:		$(OBJRSC)
		$(CC) -o rsc $(OBJRSC)
clean:
		rm -f rsd *.o *~

