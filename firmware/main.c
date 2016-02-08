//------------------------------------------------------------------------------
// Overlay64 -- Video Overlay Driver -- Atmega328p @ 20MHz
//------------------------------------------------------------------------------

#define F_CPU 20000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "main.h"
#include "font.h"
#include "eeprom.h"
#include "config.h"

#define OE   (1<<PD6)  // Output Enable (active low)
#define OR   (1<<PD7)  // Output Request (active low) (enable output for timeout frames)
#define MOSI (1<<PB3)  // SPI output pin (where the actual bitmap data is send)
#define SS   (1<<PB2)  // SPI slave select (must be pulled up in master mode)

#define ENABLE_SPI  DDRB |= MOSI  // Enable the SPI output pin
#define DISABLE_SPI DDRB &= ~MOSI // Disable (tristate) the SPI output pins

volatile uint8_t output_enable[2]  = { true, true }; // state/edge for OE
volatile uint8_t output_request[2] = { true, true }; // state/edge for OR

volatile uint16_t scanline; // current scanline of the whole video frame
volatile uint8_t enabled;   // whether or not the display is currently enabled
volatile uint8_t timeout;   // timeout counter until disabling display 

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
  
  // Setup OE (PD6) and OR (PD7) as inputs with pullups
  DDRD  &= ~OE; PORTD |= OE;
  DDRD  &= ~OR; PORTD |= OR;

  // Setup Inputs with pullups
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

  // Decrease timeout counter
  if(timeout > 0) timeout--;
  
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

  if(enabled && (scanline >= SCREEN_TOP && scanline < SCREEN_BOTTOM)) {

    // The display is enabled and we're on a vertical line inside of
    // the logical screen area, but still outside of the visible horizonzal area.
    // So there is time to precalculate some values for the current line...
    
    line = scanline - SCREEN_TOP;
    row = config->rows[line / CHAR_HEIGHT];
    byte = line % CHAR_HEIGHT;

    // ...and to decide whether we're on an empty row...
    if(row == NULL) goto skip;

    // ...not an empty row, so we'll enable the SPI Output pin...
    ENABLE_SPI;
    
    // ...and wait for the remaining time until visible area is reached
    while(TCNT1<US(9)); NOPS(10);

    // Bitbang font data for each column via SPI
    for(column=0; column<SCREEN_COLUMNS; column++) {
      SPDR = font[row[column]*CHAR_HEIGHT+byte];
      NOPS(2);
    }   
  }  
  else {
    // The display is not enabled or we're outside of the logical screen
    // or there is nothing to display on this row...
    
  skip:
    // We'll tristate the SPI pin again
    DISABLE_SPI;

    // Get the current state of the control lines and also remember the
    // previous states so that we can detect rising or falling edges    

    READ(output_enable, PIND, OE);
    READ(output_request, PIND, OR);

    // When OE is low the output is always enabled
    if(LOW(output_enable)) {
      enabled = true;
    }

    // When OE goes high again, output is disabled immediately
    else if(RISING(output_enable)) {
      enabled = false;
    }    

    // When OR is low, output is enabled and timeout starts
    else if(LOW(output_request)) {
      enabled = true;
      timeout = config->timeout;
    }

    // When OR is high and timeout is reached, output is disabled
    else if(HIGH(output_request) && !timeout) {
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


