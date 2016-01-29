MCU=atmega328p
CC=gcc
GCC=avr-gcc
CFLAGS=-std=c99 $(DEBUG) -O3 -Wall -Wno-unused 
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
FORMAT=ihex

AVRDUDE=avrdude
DFLAGS=-p m328p -c stk200 -P /dev/parport0

SOURCES=main.c \
	screen.c \
	font.c \
	eeprom.c \
	config.c \
	../config.c

HEADERS=main.h \
	screen.h \
	font.h \
	config.h \
	../config.h

all: main.hex

main.elf: $(SOURCES) $(HEADERS)
	$(GCC) $(CFLAGS) -mmcu=$(MCU) -o main.elf $(SOURCES) 

main.hex: main.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex

font.c: font
	(echo "#include <stdint.h>"; xxd -c 8 -i font) > font.c
	sed -i 's/unsigned char/uint8_t/;/^unsigned int/d' font.c

font: font.rom
	dd if=font.rom of=font bs=8 count=32 skip=32
	dd if=font.rom of=font bs=8 count=32 seek=32

test: clean font.c
	$(CC) $(CFLAGS) -o test test.c screen.c font.c
	./test | less

dump: main.elf
	$(OBJDUMP) -d -S main.elf | less

program: main.hex
	$(AVRDUDE) $(DFLAGS) -U flash:w:main.hex

fuse:
	$(AVRDUDE) $(DFLAGS) -U lfuse:w:0xf7:m -U hfuse:w:0xd0:m -U efuse:w:0xfd:m 

clean:
	rm -f *.elf *.hex font font.c test
