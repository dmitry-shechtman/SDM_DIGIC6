topdir=../../

# Makefile for platform independent 'libc' library
# Code copied from GCC for linking modules without including GCC library
 
include $(topdir)makefile_core.inc

CFLAGS+=$(CTHUMB)

OBJS = issmth.o strncmp.o strtol.o strchr.o \
    memcmp.o memset.o memmove.o memcpy.o strlen.o \
    strcmp.o

all: libc.a

libc.a: $(OBJS)

clean:
	rm -f $(OBJS) libc.a

distclean: clean
	rm -f $(OBJS:.o=.d)

include $(topdir)bottom.inc
