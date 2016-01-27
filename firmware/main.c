#define F_CPU 16000000UL
#define TICKS_PER_USEC F_CPU/1000000UL

#define Wait() while(!(SPSR & (1<<SPIF)))
#define US(a) a*(TICKS_PER_USEC)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "screen.h"
#include "font.h"

// LM1881 CSYNC -> INT0 (Pin 4)
// LM1881 VSYNC -> INT1 (Pin 5)
// Atmega OUT   -> MOSI (Pin 17)

// The current horizontal line
static volatile uint16_t line=0;

ISR(INT1_vect) {
  //wait 180us for the vertical blanking period to end
  TCNT1 = 0;
  while(TCNT1<US(180));
  line=0;
}

ISR(INT0_vect) {

  line++;
  TCNT1=0;

  if(line >= SCREEN_TOP || line <= SCREEN_BOTTOM) {

    uint8_t lin = (line-32);
    uint8_t chr = lin / CHAR_HEIGHT * SCREEN_WIDTH;
    uint8_t pos = lin % CHAR_HEIGHT;    

    while(TCNT1<US(8));   

    // Repeat SCREEN_WIDTH times... (4 times for now)
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
  }
}

int setup() {
  TCCR1B = (1<<CS10) + (1<<WGM12);
  TIMSK1=0;
  OCR1A = 0xffff;
  
  EICRA = (1<<ISC01) | (1<<ISC11); // Set interrupt on falling edge
  EIMSK = (1<<INT0) | (1<<INT1);   // Enable interrupts for int0 and int1

  ADCSRA&=~(1<<ADEN); // Turn off ADC

  //Setup SPI
  SPDR = 0;
  DDRB = (1<<DDB2) | (1<<DDB3);
  SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);

  uint8_t i = 0;
  
  screen[i++] = 'C'-0x20;
  screen[i++] = '6'-0x20;
  screen[i++] = '4'-0x20;
  screen[i++] = '?'-0x20;

  screen[i++] = 'O'-0x20;
  screen[i++] = 'S'-0x20;
  screen[i++] = 'D'-0x20;
  screen[i++] = '!'-0x20;  

  screen[i++] = 'F'-0x20;
  screen[i++] = 'U'-0x20;
  screen[i++] = 'C'-0x20;
  screen[i++] = 'K'-0x20;    

  screen[i++] = 'Y'-0x20;
  screen[i++] = 'E'-0x20;
  screen[i++] = 'A'-0x20;
  screen[i++] = 'H'-0x20;    
  
  sei();
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  return 1;
}

int main(void) {
  setup();
  while(1) sleep_mode();
  return 0;    
}


