//------------------------------------------------------------------------------
// Overlay64 -- LUMA video overlay driver
//
// LM1881 CSYNC -> INT0 (Pin 4)
// LM1881 VSYNC -> INT1 (Pin 5)
// LUMA SIGNAL <- 1k <- VCC via PNP <- MOSI (Pin 17)
//
//------------------------------------------------------------------------------

#define F_CPU 16000000UL
#define TICKS_PER_USEC F_CPU/1000000UL
#define US(n) n*(TICKS_PER_USEC)

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "main.h"
#include "screen.h"
#include "font.h"

static volatile uint16_t line=0; // The current horizontal line

//------------------------------------------------------------------------------

ISR(INT1_vect) { // VSYNC (each frame)...

  // Vertical blanking period starts...
  TCNT1 = 0;
  
  // get input and update screen
  // this must happen within 180us = 2880 cycles @ 16MHz

  //wait for the vertical blanking period to end
  while(TCNT1<US(180));
  line=0;
}

//------------------------------------------------------------------------------

ISR(INT0_vect) { // HSYNC (each line)...

  uint8_t lin, chr, pos;

  TCNT1=0;  
  line++;

  if(line >= SCREEN_TOP || line <= SCREEN_BOTTOM) {

    lin = (line-32);
    chr = lin / CHAR_HEIGHT * SCREEN_WIDTH;
    pos = lin % CHAR_HEIGHT;    

    while(TCNT1<US(8));   

    // Repeat SCREEN_WIDTH times... (4 times for now)
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
    SPDR = font[screen[chr++]*CHAR_HEIGHT+pos];
  }
}

//------------------------------------------------------------------------------

int SetupHardware() {
  
  // Setup Timer1
  TCCR1B = (1<<CS10);              // Run at system clock 
  TIMSK1 = 0;                      // Disable all timer interrupts

  // Setup Interrupts
  EICRA = (1<<ISC01) | (1<<ISC11); // Set interrupt on falling edge
  EIMSK = (1<<INT0) | (1<<INT1);   // Enable interrupts for int0 and int1

  // Turn off ADC  
  ADCSRA&=~(1<<ADEN); 

  // Setup SPI 
  DDRB = (1<<DDB2) | (1<<DDB3);    // SS and MOSI as outputs
  SPCR =
    (1<<SPE) | (1<<MSTR) |         // Enable SPI as Master
    (1<<CPHA) | (1<<CPOL);         // Setup with falling edge of SCK

  // Prepare test screen
  write(screen, 0, 0, "COOL");
  write(screen, 1, 0, "OSD!");
  write(screen, 2, 0, "1234");
  write(screen, 3, 0, "+-*/");      

  // Enable Interrupts
  sei();

  // Setup sleep mode
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  return 1;
}

//------------------------------------------------------------------------------

int main(void) {
  SetupHardware();
  
  while(1) sleep_mode();
  return 0;    
}

//------------------------------------------------------------------------------


