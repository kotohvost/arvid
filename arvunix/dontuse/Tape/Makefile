# for gmake

CXX		= g++
CPPFLAGS	= -O2 -g
CXXFLAGS	= -O
SRCCPP		= Tape.cpp mystring.cpp \
		main.cpp Pt_test.cpp Tfs.cpp Tfs_test.cpp

INCDIR		= -I. -I../../include
PTOBJ		= ../../lib/Pt.o ../../lib/format.o
TFOBJ		= ../../lib/Tfs.o Tfs_test.o
#LIBDIR		= ../../lib
# g++ -g -O2 -c 

.SUFFIXES:	.cpp

.cpp.o:		$(CXXSRC)
		$(CXX) $(CPPFLAGS) $(INCDIR) -c $<

OBJS		= Dir.o \
		Irc.o \
		Tape.o \
		mystring.o \
		main.o

all:	main

Pt_test:	Pt_test.o $(PTOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(PTOBJ) Pt_test.o

Tfs_test:	$(TFOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(TFOBJ)

main:	$(OBJS)
	$(CXX) $(CPPFLAGS) -o $@ $(OBJS)

clean:
	-rm *.o $(ALL)
