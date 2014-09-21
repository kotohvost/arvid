CPP		= g++
CPPFLAGS	= -O2 -g

INCDIR		= -I../../include
LIBDIR		= ../lib

SRCCPP		= AvCopier.cpp Ident.cpp Verefy.cpp Tdr.cpp
OBJCOPY		= AvCopier.o
OBJVEREFY	= Verefy.o
OBJTDR		= Tdr.o
OBJADD		= ../lib/avt200.o ../lib/crc.o
OBJADD1		= ../lib/avt_rs.o ../lib/crc.o
OBJCPP		= $(OBJCOPY) $OBJVEFERY)

all:	AvCopier Verefy Tdr

#.o.cpp:
#	$(CPP) -c $< $(CPPFLAGS) $(INCDIR)

AvCopier:	$(OBJCOPY)
	$(CPP) $(OBJCOPY) $(OBJADD) -o AvCopier

AvCopier.o:
	$(CPP) $(CPPFLAGS) $(INCDIR) -c AvCopier.cpp

Ident.o:
	$(CPP) $(CPPFLAGS) $(INCDIR) -c Ident.cpp

Verefy:		$(OBJVEREFY)
	$(CPP) $(OBJVEREFY) $(OBJADD) -o Verefy

Verefy1:		$(OBJVEREFY)
	$(CPP) $(OBJVEREFY) $(OBJADD1) -o Verefy1

Verefy.o:
	$(CPP) $(CPPFLAGS) $(INCDIR) -c Verefy.cpp

Tdr:		$(OBJTDR)
	$(CPP) $(OBJTDR) -o Tdr ../lib/msdos_conv.o

Tdr.o:
	$(CPP) $(CPPFLAGS) $(INCDIR) -c Tdr.cpp

clean:
	rm *.o $(all)

depend:
	MKDEP_CPP="$(CPP)"; export MKDEP_CPP; \
	mkdep -a -f .newdep $(INCDIR) $(SRCCPP) $(CPPFLAGS)
	mv .newdep .depend
