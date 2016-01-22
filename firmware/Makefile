MCU=atmega88p
GCC=avr-gcc
CFLAGS=-mmcu=$(MCU)
FORMAT=ihex
OBJCOPY=avr-objcopy

all: main.hex

main.elf: main.c
	$(GCC) $(CFLAGS) -o main.elf main.c

main.hex: main.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex
clean:
	rm -f *.elf *.hex
