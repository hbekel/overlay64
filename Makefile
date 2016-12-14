include Makefile.inc

CC=gcc
MINGW32?=i686-w64-mingw32
CFLAGS=-std=gnu99 -g -O2 -Wall -DVERSION=$(VERSION) -DOFFSET=$(OFFSET)
LIBS=-lusb-1.0

PREFIX?=/usr/local
DESTDIR=

UNAME=$(shell uname)

MD5SUM=md5sum
ifeq ($(UNAME), Darwin)
  MD5SUM=md5 -r
endif

UDEV=0
ifeq ($(UNAME), Linux)
  UDEV=1
endif

SOURCES=strings.c config.c parser.c usb.c intelhex.c overlay64.c
HEADERS=strings.h config.h parser.h usb.h intelhex.h overlay64.h

FIRMWARE=firmware/main.c firmware/main.h \
	firmware/config.c firmware/config.h \
	firmware/eeprom.c firmware/eeprom.h \
	firmware/font.h firmware/font.rom

.PHONY: all linux windows firmware download clean firmware-clean intelhex

all: linux

linux: overlay64 
win32: overlay64.exe 

overlay64: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o overlay64 $(SOURCES) $(LIBS)

overlay64.exe: $(SOURCES) $(HEADERS)
	$(MINGW32)-gcc $(CFLAGS) -o overlay64 $(SOURCES) $(LIBS) 

overlay64.bin: overlay64.conf
	./overlay64 convert overlay64.conf overlay64.bin

firmware: firmware/main.hex

firmware/main.hex: $(FIRMWARE)
	make -C firmware

firmware-clean:
	make -C firmware clean
	if [ "x$$OSTYPE" = "xcygwin" ]; then \
	git checkout firmware/usbdrv/usbdrvasm.S; else true; fi

bootloader: bootloader/firmware/main.hex

bootloader/firmware/main.hex:
	make -C bootloader

bootloader-clean:
	make -C bootloader clean
	if [ "x$$OSTYPE" = "xcygwin" ]; then \
	git checkout bootloader/firmware/usbdrv/usbdrvasm.S; else true; fi

boot: overlay64
	./overlay64 boot || true

touch:
	avrdude -p m1284 -c usbasp

flash: overlay64.bin
	avrdude -p m1284 -c usbasp -U eeprom:w:overlay64.bin:r	

program:
	make -C firmware program

test: all overlay64.conf
	rm -rf tmp
	mkdir tmp
	./overlay64 convert overlay64.conf tmp/overlay64.bin
	./overlay64 convert tmp/overlay64.bin tmp/roundtrip.conf
	./overlay64 convert tmp/roundtrip.conf tmp/roundtrip.bin
	diff tmp/overlay64.bin tmp/roundtrip.bin
	rm -rf tmp

test-plot: clean
	make -C firmware font.c
	$(CC) $(CFLAGS) -o test-plot \
		test.c config.c parser.c strings.c \
		firmware/config.c firmware/font.c
	./test-plot < test.conf | less -S

install: overlay64
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m755 overlay64 $(DESTDIR)$(PREFIX)/bin
	([ "$(UDEV)x" = "1x" ] && make udev-install) || true

udev-install:
	install -d $(DESTDIR)/etc/udev/rules.d
	install -m644 overlay64.rules $(DESTDIR)/etc/udev/rules.d/10-overlay64.rules

uninstall:
	rm -f $(PREFIX)/bin/keyman64
	([ "$(UDEV)x" = "1x" ] && make udev-uninstall) || true

udev-uninstall:
	rm -f /etc/udev/rules.d/10-overlay64.rules


intelhex: intelhex/ihex2bin

intelhex/ihex2bin:
	make -C intelhex

intelhex-clean:
	make -C intelhex clean

overlay64-application-$(VERSION).hex: firmware
	cp firmware/main.hex $@

overlay64-application-$(VERSION).bin: overlay64-application-$(VERSION).hex intelhex
	intelhex/ihex2bin -i overlay64-application-$(VERSION).hex -o $@

overlay64-bootloader-$(VERSION).hex: bootloader
	cp bootloader/firmware/main.hex $@

overlay64-bootloader-$(VERSION).bin: overlay64-bootloader-$(VERSION).hex intelhex
	intelhex/ihex2bin -i overlay64-bootloader-$(VERSION).hex -o $@

overlay64-application-and-bootloader-$(VERSION).hex: firmware bootloader
	(head -n-1 firmware/main.hex; cat bootloader/firmware/main.hex) > $@

overlay64-application-and-bootloader-$(VERSION).bin: overlay64-application-and-bootloader-$(VERSION).hex intelhex
	intelhex/ihex2bin -i overlay64-application-and-bootloader-$(VERSION).hex -o $@

hex: overlay64-application-$(VERSION).hex  overlay64-bootloader-$(VERSION).hex overlay64-application-and-bootloader-$(VERSION).hex

bin: overlay64-application-$(VERSION).bin  overlay64-bootloader-$(VERSION).bin overlay64-application-and-bootloader-$(VERSION).bin

download: clean overlay64 bin hex
	mkdir download
	cp *.{hex,bin} download/
	make -C hardware/gerber
	cp hardware/gerber/*.zip download/
	git archive --prefix=overlay64-$(VERSION)/ -o download/overlay64-$(VERSION).tar.gz HEAD
	for f in download/*.{hex,bin,gz,zip}; do $(MD5SUM) "$$f" > "$$f".md5; done
	tar -v -c -z --transform 's/download/overlay64/' -f overlay64.tar.gz download/

clean: bootloader-clean firmware-clean intelhex-clean
	rm -rf download
	rm -f overlay64.tar.gz
	rm -f *.{bin,hex}	
	rm -f overlay64{,.exe{,.stackdump}}
	rm -f test-plot{,.exe{,.stackdump}}
