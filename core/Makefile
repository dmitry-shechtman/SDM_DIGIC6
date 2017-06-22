topdir=../

include $(topdir)makefile.inc

CFLAGS+=$(CTHUMB)

# warning: library order matters!
LDLIBS= -lgcc
LDOPTS=-nostdlib -Wl,--allow-shlib-undefined -Wl,-T,$(topdir)tools/link-boot.ld
LDOPTS+=-Wl,-N,-Ttext,$(MEMISOSTART)

all: main.bin

OPT_OBJS=

ifdef OPT_SERIALIO
CFLAGS+=-DOPT_SERIALIO
OPT_OBJS+=serialio.o 
OPT_OBJS+=serialpacket.o 
OPT_OBJS+=remotecommand.o 
endif

ifdef OPT_PTP
OPT_OBJS+=ptp.o
endif

ifdef OPT_OMNI
OPT_OBJS+=dng.o
endif

ifdef OPT_ALARIK
OPT_OBJS+=console.o
endif

OPT_OBJS+=suba.o

OBJS=entry.o nothumb.o main.o gui_menu.o draw_palette.o mbox_popup.o \
      gui_fselect.o  gui.o kbd.o conf.o gui_read.o\
      batt_grid.o gui_osd.o script.o raw.o \
     gui_lang.o  motion_detector.o  $(OPT_OBJS)

gui.o: FORCE

FORCE:

nothumb.o: nothumb.c
	@echo $< \-\> $@
	$(CC) $(CFLAGS) -marm -nostdinc -c -o $@ $<

main.bin: main.elf
	@echo $< \-\> $@
	$(OBJDUMP) -z -d main.elf > main.dump
	$(OBJCOPY) -O binary main.elf main.bin

main.elf: $(OBJS) $(topdir)platform/$(PLATFORM)/libplatform.a \
          $(topdir)platform/$(PLATFORM)/sub/$(PLATFORMSUB)/libplatformsub.a \
          $(topdir)lib/font/libfont.a $(topdir)lib/math/libmath.a \
          $(topdir)lib/ubasic/libubasic.a $(topdir)lib/lang/liblang.a 
	@echo \-\> $@

	$(CC) $(CFLAGS) -o $@ --start-group $^  $(LDLIBS) --end-group $(LDFLAGS) $(LDOPTS)
	( $(NM) $@ | grep ' U ' > $@.syms ) && exit 1 || exit 0
	$(SIZE) $@ | tee $(topdir)size.txt
	@printf " %s-%s (%s, #%s): MEMISOSIZE used: 0x%s\n" \
	$(PLATFORM) $(PLATFORMSUB) $(PLATFORMOS) $(PLATFORMID) \
	`tail -c16 $(topdir)size.txt | head -c6` >> $(topdir)bin/caminfo.txt
	rm -f $(topdir)size.txt

clean:
	rm -f $(OBJS) main.bin main.elf main.dump main.elf.syms

distclean: clean
	rm -f $(OBJS:.o=.d)

include $(topdir)bottom.inc
