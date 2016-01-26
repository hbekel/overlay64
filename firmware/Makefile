MCU=atmega328p

GCC=avr-gcc
CFLAGS=-O2 -Wall -mmcu=$(MCU)
FORMAT=ihex
OBJCOPY=avr-objcopy

AVRDUDE=avrdude
DFLAGS=-p m328p -c stk200 -P /dev/parport0

all: main.hex

main.elf: main.c
	$(GCC) $(CFLAGS) -o main.elf main.c

main.hex: main.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex

program: main.hex
	$(AVRDUDE) $(DFLAGS) -U flash:w:main.hex

clean:
	rm -f *.elf *.hex
