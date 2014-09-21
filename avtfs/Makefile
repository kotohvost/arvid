# LG SoftLab, 2001y.
# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

# This Makefile has been simplified as much as possible, by putting all
# generic material, independent of this specific directory, into
# ../Rules.make. Read that file for details

#TOPDIR  := $(shell cd ..; pwd)
#include $(TOPDIR)/Rules.make
include Rules.make

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSIMPLE_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
CFLAGS += -I.. -D__GNUC__=2
CXXFLAGS += -I.. -D__GNUC__=2
LFLAGS = -s

TARGET = avtfs
OBJS = $(TARGET).o
SRC = avtfs.c

.SUFFIXES: .cpp .c .o
.PHONY: all clean install zip

all: .depend $(TARGET).o avtfs_test

clean:
	rm -f *.o core .depend

install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).o $(INSTALLDIR)

.cpp.o:
	$(CC) -c $(CXXLAGS) -o $@ $<

zip:
	@echo --------------------------------------
	@echo Backing up to archive $(TARGET).zip
	@echo --------------------------------------
	-@zip -ru9D ../$(TARGET).zip ../$(TARGET) -x@exclude.lst

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > $@

avtfs_test: avtfs_test.o	$(CC) $(LFLAGS) -o $@ $<

ifeq (.depend,$(wildcard .depend))
include .depend
endif
