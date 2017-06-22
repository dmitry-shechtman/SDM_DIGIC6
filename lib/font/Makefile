topdir=../../
 
include $(topdir)makefile_core.inc

OBJS = $Ofont_8x16.o  

all: $Olibfont.a

$Olibfont.a: $(OBJS)

$Ofont_8x16.o: font_8x16.c codepages.h font_8x16_uni.h

clean:
	rm -f $(OBJS) $Olibfont.a

distclean: clean
	rm -f $(OBJS:.o=.d)

include $(topdir)bottom.inc

 
