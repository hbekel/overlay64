include ../Makefile.inc

MCU=atmega1284p
F_CPU = 20000000
ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp -DF_CPU=$(F_CPU) -Wa,-adhlns=$(<:%.S=./%.lst),-gstabs,--listing-cont-lines=100
GCC=avr-gcc
CFLAGS=-std=c99 $(DEBUG) -O3 -Wall -Wno-unused -DVERSION=$(VERSION)
LDFLAGS=-Wl,-section-start=.font=$(OFFSET)
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
FORMAT=ihex

AVRDUDE=avrdude
DFLAGS=-p m1284 -c usbasp -V

SOURCES=main.c \
	font.c \
	eeprom.c \
	config.c \
	../config.c \
	usbdrv/usbdrv.c \
	usbdrv/oddebug.c \
	usbdrv/usbdrvasm.S

HEADERS=main.h \
	font.h \
	config.h \
	../config.h \
	../protocol.h

all: main.hex

%.o : %.S
	$(GCC) -c $(ASFLAGS) $< -o $@

main.elf: $(SOURCES) $(HEADERS)
	$(GCC) $(CFLAGS) -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(LDFLAGS) -o main.elf $(SOURCES)

main.hex: main.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex

font.c: font
	(echo -e "#include <stdint.h>\n#include \"font.h\""; xxd -c 8 -i font) > font.c
	sed -i -r 's/ font/ _font/;s/unsigned char/const uint8_t/' font.c
	set -i -r ';/^unsigned int/d;s/ =/ FONT_SECTION =/' font.c

font: font.rom
	dd if=font.rom of=font bs=8 count=96

dump: main.elf
	$(OBJDUMP) -d -S main.elf | less

program: main.hex
	$(AVRDUDE) $(DFLAGS) -U flash:w:main.hex

fuse:
	$(AVRDUDE) $(DFLAGS) -U lfuse:w:0xf7:m -U hfuse:w:0xd0:m -U efuse:w:0xfd:m 

clean:
	rm -f *.elf *.hex *.o usbdrv/*.o font font.c test

