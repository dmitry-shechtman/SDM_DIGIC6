topdir=../../

include $(topdir)makefile_core.inc
 



OPT_OBJS=

ifdef OPT_SERIALIO
CFLAGS+=-DOPT_SERIALIO
OPT_OBJS+=serialio.o 
OPT_OBJS+=serialpacket.o 
OPT_OBJS+=remotecommand.o 
endif

ifdef OPT_PTP
OPT_OBJS+=$Optp.o
endif

OPT_OBJS+=$Odng.o
 

ifdef OPT_ALARIK
OPT_OBJS+=$Oconsole.o
endif

OPT_OBJS+=$Osuba.o

OBJS=  $Omain.o $Onothumb.o $Ogui_menu.o $Odraw_palette.o $Ombox_popup.o \
      $Ogui_fselect.o $Ogui.o $Okbd.o $Oconf.o $Ogui_read.o\
      $Obatt_grid.o $Ogui_osd.o $Oscript.o $Oraw.o \
     $Ogui_lang.o  $Omotion_detector.o $(OPT_OBJS)

all: $Olibcore.a
 
$Olibcore.a: $(OBJS)

clean:
	rm -f $O*.* *.o *.a

distclean: clean
	rm -f $(OBJS:.o=.d)

include $(topdir)bottom.inc