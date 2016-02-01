CC=gcc
MINGW32?=i686-w64-mingw32
CFLAGS=-std=c99 -g -O2 -Wall

SOURCES=strings.c config.c parser.c overlay64.c
HEADERS=strings.h config.h parser.h

all: overlay64

overlay64: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o overlay64 $(SOURCES)

win32: $(SOURCES) $(HEADERS)
	$(MINGW32)-gcc $(CFLAGS) -o overlay64 $(SOURCES) 

test: all overlay64.conf
	rm -rf tmp
	mkdir tmp
	./overlay64 < overlay64.conf > tmp/overlay64.bin
	./overlay64 < tmp/overlay64.bin > tmp/roundtrip.conf
	./overlay64 < tmp/roundtrip.conf > tmp/roundtrip.bin
	diff tmp/overlay64.bin tmp/roundtrip.bin
	rm -rf tmp

test-plot: clean
	make -C firmware font.c
	$(CC) $(CFLAGS) -o test-plot \
		test.c config.c parser.c strings.c \
		firmware/config.c firmware/font.c
	./test-plot < test.conf | less -S

clean:
	rm -f overlay64
	rm -f overlay64.exe{,.stackdump}
	rm -f test-plot
	rm -f test-plot.exe{,.stackdump}

