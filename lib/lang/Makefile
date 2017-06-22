topdir=../../
 
include $(topdir)makefile_core.inc

CFLAGS+=$(CTHUMB)

OBJS = lang.o

all: liblang.a

liblang.a: $(OBJS)

clean:
	rm -f $(OBJS) libfont.a liblang.a

distclean: clean
	rm -f $(OBJS:.o=.d)

include $(topdir)bottom.inc
