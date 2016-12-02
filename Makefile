include version.inc

CC=gcc
MINGW32?=i686-w64-mingw32
CFLAGS=-std=gnu99 -g -O2 -Wall -DVERSION=$(VERSION)
LIBS=-lusb-1.0

PREFIX?=/usr/local
DESTDIR=

SOURCES=strings.c config.c parser.c usb.c overlay64.c
HEADERS=strings.h config.h parser.h usb.h overlay64.h

.PHONY: firmware clean firmware-clean

all: overlay64

overlay64: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o overlay64 $(SOURCES) $(LIBS)

win32: $(SOURCES) $(HEADERS)
	$(MINGW32)-gcc $(CFLAGS) -o overlay64 $(SOURCES) $(LIBS) 

overlay64.bin: overlay64.conf
	./overlay64 convert overlay64.conf overlay64.bin

firmware: 
	make -C firmware

firmware-clean:
	make -C firmware clean

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

clean:
	rm -f *.bin
	rm -f overlay64{,.exe{,.stackdump}}
	rm -f test-plot{,.exe{,.stackdump}}

release: clean
	git archive --prefix=overlay64-$(VERSION)/ -o ../overlay64-$(VERSION).tar.gz HEAD
