MCU=atmega328p

GCC=avr-gcc
CFLAGS=-std=c99 -O2 -g -Wall -Wno-unused -mmcu=$(MCU)
FORMAT=ihex
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

AVRDUDE=avrdude
DFLAGS=-p m328p -c stk200 -P /dev/parport0

SOURCES=main.c \
	screen.c \
	font.c

HEADERS=main.h \
	screen.h \
	font.h

all: main.hex

main.elf: $(SOURCES) $(HEADERS)
	$(GCC) $(CFLAGS) -o main.elf $(SOURCES) 

main.hex: main.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex

font.c: font
	(echo "#include <stdint.h>"; xxd -c 8 -i font) > font.c
	sed -i 's/unsigned char/uint8_t/;/^unsigned int/d' font.c
	rm font

font: font.rom
	dd if=font.rom of=font bs=8 count=32 skip=32
	dd if=font.rom of=font bs=8 count=32 seek=32

test: clean font.c
	gcc -std=c99 -o test test.c screen.c font.c
	./test | less

dump: main.elf
	$(OBJDUMP) -d -S main.elf | less

program: main.hex
	$(AVRDUDE) $(DFLAGS) -U flash:w:main.hex

fuse:
	$(AVRDUDE) $(DFLAGS) -U lfuse:w:0xf7:m -U hfuse:w:0xd9:m -U efuse:w:0xfc:m 

clean:
	rm -f *.elf *.hex font font.c test
