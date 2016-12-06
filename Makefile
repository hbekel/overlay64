include version.inc

CC=gcc
MINGW32?=i686-w64-mingw32
CFLAGS=-std=gnu99 -g -O2 -Wall -DVERSION=$(VERSION)
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

.PHONY: all linux windows firmware clean firmware-clean

all: linux

linux:   overlay64 binaries
win32: overlay64.exe binaries

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

ihex2bin: intelhex/ihex2bin

intelhex/ihex2bin:
	make -C intelhex

overlay64-application-$(VERSION).hex: firmware
	cp firmware/main.hex $@

overlay64-application-$(VERSION).bin: overlay64-application-$(VERSION).hex ihex2bin
	intelhex/ihex2bin -i overlay64-application-$(VERSION).hex -o $@

overlay64-bootloader-$(VERSION).hex: bootloader
	cp bootloader/firmware/main.hex $@

overlay64-bootloader-$(VERSION).bin: overlay64-bootloader-$(VERSION).hex ihex2bin
	intelhex/ihex2bin -i overlay64-bootloader-$(VERSION).hex -o $@

overlay64-application-and-bootloader-$(VERSION).hex: firmware bootloader
	(head -n-1 firmware/main.hex; cat bootloader/firmware/main.hex) > $@

overlay64-application-and-bootloader-$(VERSION).bin: overlay64-application-and-bootloader-$(VERSION).hex ihex2bin
	intelhex/ihex2bin -i overlay64-application-and-bootloader-$(VERSION).hex -o $@

binaries: overlay64-application-$(VERSION).bin  overlay64-bootloader-$(VERSION).bin overlay64-application-and-bootloader-$(VERSION).bin

release: clean
	git archive --prefix=overlay64-$(VERSION)/ -o ../overlay64-$(VERSION).tar.gz HEAD
	$(MD5SUM) ../overlay64-$(VERSION).tar.gz > ../overlay64-$(VERSION).tar.gz.md5

clean: bootloader-clean firmware-clean
	rm -f *.{bin,hex}
	rm -f overlay64{,.exe{,.stackdump}}
	rm -f test-plot{,.exe{,.stackdump}}
