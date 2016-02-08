//------------------------------------------------------------------------------
// Overlay64 -- Video Overlay Driver -- Atmega328p @ 20MHz
//------------------------------------------------------------------------------

#define F_CPU 20000000UL
#define TICKS_PER_USEC F_CPU/1000000UL
#define US(n) n*(TICKS_PER_USEC)
#define NOP() do { __asm__ __volatile__ ("nop"); } while (0)
#define NOPS(n) for(uint8_t i=0; i<n; i++) NOP()

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "font.h"
#include "eeprom.h"
#include "config.h"

#define OE   (1<<PD7)
#define MOSI (1<<PB3)
#define SS   (1<<PB2)

#define ENABLE_SPI  DDRB |= MOSI
#define DISABLE_SPI DDRB &= ~MOSI

volatile uint8_t frame;     // frame counter for timing the auto-disable feature
volatile uint16_t scanline; // current scanline of the whole video frame
volatile uint8_t enabled;   // whether or not the display is currently enabled

volatile Config* config;    // Configuration read from eeprom

//------------------------------------------------------------------------------

static void setup() {

  // Create config and assign ports
  config = Config_new_with_ports(&PINC, &PIND, &PINB);

  // Read config from eeprom
  Config_read(config, &eeprom);
  
  // Setup Timer1
  TCCR1B = (1<<CS10);              // Run at system clock 
  TIMSK1 = 0;                      // Disable all timer interrupts

  // Setup Interrupts
  EICRA = (1<<ISC01) | (1<<ISC11); // Set interrupt on falling edge
  EIMSK = (1<<INT0) | (1<<INT1);   // Enable interrupts for int0 and int1

  // Turn off ADC  
  ADCSRA &= ~(1<<ADEN); 

  // Setup SPI 
  DDRB = SS | MOSI;                // /SS and MOSI as outputs
  SPCR =
    (1<<SPE) | (1<<MSTR) |         // Enable SPI as Master
    (1<<CPHA) | (1<<CPOL);         // Setup with falling edge of SCK
  SPSR = (1<<SPI2X);               // Double speed
  
  // Setup /OE (PD7) as input
  DDRD  &= ~OE;
  PORTD |= OE;

  // Setup Inputs
  DDRB  &= ~((1<<PB0) | (1<<PB1));
  PORTB |=  ((1<<PB0) | (1<<PB1));

  DDRC  &= ~((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5));
  PORTC |=  ((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5));

  DDRD  &= ~((1<<PD0) | (1<<PD1) | (1<<PD4) | (1<<PD5) | (1<<PD6));
  PORTD |=  ((1<<PD0) | (1<<PD1) | (1<<PD4) | (1<<PD5) | (1<<PD6));
  
  //Enable Interrputs
  sei();
    
  // Setup sleep mode
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}

//------------------------------------------------------------------------------

ISR(INT1_vect) { // VSYNC (each frame)...

  // Vertical blanking period starts...
  TCNT1 = 0;

  // Update frame counter
  frame++;
  
  // Get input and update screen according to config
  // This must happen within 180us = 3600 cycles @ 20MHz

  Config_apply(config);

  // Wait for the remaining time until the end of the
  // vertical blanking period (180us)
  while(TCNT1<US(180));

  // We're on the first scanline now
  scanline = 0;
}

//------------------------------------------------------------------------------

ISR(INT0_vect) { // HSYNC (each line)...

  uint8_t* row;   // Character data for the current row
  uint8_t line;   // Logical line of the visible screen
  uint8_t column; // Current character column in the current row 
  uint8_t byte;   // Current byte of the character bitmap
  
  TCNT1=0;  
  scanline++;

  if(!enabled) goto skip;
  
  if(scanline >= SCREEN_TOP && scanline < SCREEN_BOTTOM) {

    // Still outside of the visible area...
    // Precalculate values for the current line
    
    line = scanline - SCREEN_TOP;
    row = config->rows[line / CHAR_HEIGHT];
    byte = line % CHAR_HEIGHT;

    // Skip empty rows
    if(row == NULL) goto skip;

    // Enable the SPI Output pin
    ENABLE_SPI;
    
    // Wait for the remaining time until visible area is reached
    while(TCNT1<US(9)); NOPS(10);

    // Bitbang font data for each column via SPI
    for(column=0; column<SCREEN_COLUMNS; column++) {
      SPDR = font[row[column]*CHAR_HEIGHT+byte];
      NOPS(2);
    }   
  }
  
  if(scanline >= SCREEN_BOTTOM) {          
    
  skip:
    DISABLE_SPI;
    
    if(!(PIND & OE)) {
      enabled = true;
      frame = 0;
    }
    
    if(enabled && frame > config->timeout) {
      enabled = false;
    }
  }  
}

//------------------------------------------------------------------------------

int main(void) {

  setup();
  while(1) sleep_mode();
  return 0;    
}

//------------------------------------------------------------------------------


