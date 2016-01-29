CC=gcc
CFLAGS=-std=c99 -g -O2 -Wall

SOURCES=strings.c config.c overlay64.c
HEADERS=strings.h config.h overlay64.h

all: overlay64

overlay64: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o overlay64 $(SOURCES)

test: all overlay64.conf
	rm -rf tmp
	mkdir tmp
	./overlay64 < overlay64.conf > tmp/overlay64.bin
	./overlay64 < tmp/overlay64.bin > tmp/roundtrip.conf
	./overlay64 < tmp/roundtrip.conf > tmp/roundtrip.bin
	diff tmp/overlay64.bin tmp/roundtrip.bin
	rm -rf tmp


clean:
	rm -f overlay64
	rm -f overlay64.exe{,.stackdump}
