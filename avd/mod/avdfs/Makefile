
TARGET = avdfs

include ./../Rules.make

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSIMPLE_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
# Do it, if kernel build with GCC 2.X.X:
# CFLAGS += -D__GNUC__=2
CFLAGS += -I./../../inc
LFLAGS = -s

.SUFFIXES: .c .o
.PHONY: all clean install

all: .depend $(TARGET).o

clean:
	rm -f *.o core .depend

install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).o $(INSTALLDIR)

depend .depend dep:
	$(CC) $(CFLAGS) -MM *.c > $@
