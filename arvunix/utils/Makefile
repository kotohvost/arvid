#Makefile avinfo.

CC = gcc
CXX = g++
CFLAGS		= -g -O2
CXFLAGS		= -g -O2
INCDIR		= -I../include
LIBDIR		= -I../lib
ALL		= avinfo avphase avirc avident

all:	$(ALL)

avinfo:
	$(CC) $(CFLAGS) $(INCDIR) -o avinfo avinfo.c
avphase:
	$(CC) $(CFLAGS) $(INCDIR) -o avphase avphase.c
avirc:
	$(CXX) $(CXFLAGS) $(INCDIR) -o avirc avirc.cpp
avident:
	$(CC) $(CFLAGS) $(INCDIR) $(LIBDIR) -o avident avident.c ../lib/dcheader.o ../lib/rs.o ../lib/crc.o
vcr:
	$(CC) $(INCDIR) -o vcr vcr.c
clean:
	rm -f $(ALL)

avirc: ../include/Irc.hpp avirc.cpp
avinfo: avinfo.c
avident: avident.c
