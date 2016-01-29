#include <stdio.h>
#include <avr/io.h>

static int ReadEeprom(FILE* file) {

  static volatile uint16_t addr = 0;
  
  while(EECR & (1<<EEPE));
  EEAR = addr;
  EECR |= (1<<EERE);
  addr += 1;

  return EEDR;
}

FILE eeprom = FDEV_SETUP_STREAM(NULL, ReadEeprom, _FDEV_SETUP_READ);
