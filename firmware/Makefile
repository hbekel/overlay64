MCU=atmega1284
F_CPU = 20000000
ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp -DF_CPU=$(F_CPU) -Wa,-adhlns=$(<:%.S=./%.lst),-gstabs,--listing-cont-lines=100
GCC=avr-gcc
CFLAGS=-std=c99 $(DEBUG) -O3 -Wall -Wno-unused 
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
	$(GCC) $(CFLAGS) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -o main.elf $(SOURCES) 

main.hex: main.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex

font.c: font
	(echo "#include <stdint.h>"; xxd -c 8 -i font) > font.c
	sed -i -r 's/unsigned char/const uint8_t/;/^unsigned int/d' font.c

font: font.rom
	dd if=font.rom of=font bs=8 count=32 skip=32
	dd if=font.rom of=font bs=8 count=32 seek=32

dump: main.elf
	$(OBJDUMP) -d -S main.elf | less

program: main.hex
	$(AVRDUDE) $(DFLAGS) -U flash:w:main.hex

fuse:
	$(AVRDUDE) $(DFLAGS) -U lfuse:w:0xf7:m -U hfuse:w:0xd0:m -U efuse:w:0xfd:m 

clean:
	rm -f *.elf *.hex font font.c test
