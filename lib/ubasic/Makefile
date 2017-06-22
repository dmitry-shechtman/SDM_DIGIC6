topdir=../../
include $(topdir)makefile_core.inc
CFLAGS+=$(CTHUMB)

OBJS = tokenizer.o ubasic.o

all: libubasic.a

libubasic.a: $(OBJS)
 

clean:
	rm -f $(OBJS) libubasic.a  

distclean: clean
	rm -f $(OBJS:.o=.d)

include $(topdir)bottom.inc

