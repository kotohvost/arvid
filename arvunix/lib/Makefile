# Makefile for lib.

CC 		= gcc
CFLAGS		= -O2
CXX		=g++
CXXFLAGS	= -g -O2

CSRC		= crc.c g.c msdos_conv.c dcheader.c format.c
COBJ		= crc.o g.o msdos_conv.o dcheader.o format.o
CXXSRC		= Pt.cpp Irc.cpp Tfs.cpp ReadWrite.cpp
CXXOBJ		= Pt.o Irc.o Tfs.o ReadWrite.o
INCLUDE		= -I ../include

.SUFFIXES:	.c .cpp

.c.o:		$(CSRC)
		$(CC) $(CFLAGS) $(INCLUDE) -c $<

.cpp.o:		$(CXXSRC)
		$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

freebsd:	$(COBJ) $(CXXOBJ)
		-ln -s freebsd/rs.o rs.o

clean:
		rm -f *.o
