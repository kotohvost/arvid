CPP		= g++
CPPFLAGS	= -O2 -g

INCDIR		= -I../../include
LIBDIR		= ../../lib

SRCCPP		= Pt.cpp
OBJPT		= Pt.o
OBJADD		= ../lib/avt_rs.o ../lib/crc.o
OBJCPP		= $(OBJPT)

all:	Pt

Pt:	$(OBJPT)
	$(CPP) $(OBJPT) -o Pt

clean:
	rm *.o $(all)

depend:
	MKDEP_CPP="$(CPP)"; export MKDEP_CPP; \
	mkdep -a -f .newdep $(INCDIR) $(SRCCPP) $(CPPFLAGS)
	mv .newdep .depend

Pt.o:
	$(CPP) $(CPPFLAGS) $(INCDIR) -c Pt.cpp

