topdir=../
include $(topdir)makefile_base.inc
 

SUBDIRS= $(lubasic) $(larmutil) $(lmath) $(llang) $(lfont) $(lcore) $(llibc)

 
clean:
	rm -f $O*.* *.o *.flt *.dump
	rm -f $(core)/*.o $(core)/main.bin* $(core)/main.elf*

distclean: clean
#	rm -f *.flt

include $(topdir)bottom.inc
 
