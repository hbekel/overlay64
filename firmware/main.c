//------------------------------------------------------------------------------
// Overlay64 -- Video Overlay Driver -- Atmega1284 @ 20MHz
//------------------------------------------------------------------------------

#define F_CPU 20000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "main.h"
#include "font.h"
#include "eeprom.h"
#include "config.h"

#define OE   (1<<PD6)  // Output Enable (act. low)
#define OR   (1<<PD7)  // Output Request (act. low) (output for timeout/50 sec)
#define MOSI (1<<PB5)  // SPI output pin (where the actual bitmap data is send)
#define SS   (1<<PB4)  // SPI slave select (must be pulled up in master mode)

#define ENABLE_SPI  DDRB |= MOSI  // Enable the SPI output pin
#define DISABLE_SPI DDRB &= ~MOSI // Disable (tristate) the SPI output pin

volatile uint8_t output_enable[2]  = { true, true }; // state/edge for OE
volatile uint8_t output_request[2] = { true, true }; // state/edge for OR

volatile uint16_t scanline; // current scanline of the whole video frame
volatile uint8_t enabled;   // whether or not the display is currently enabled
volatile uint8_t timeout;   // timeout counter until disabling display 

volatile Config* config;    // Configuration is read from eeprom

//------------------------------------------------------------------------------

static void setup() {

  // Create config and assign ports
  config = Config_new_with_ports(&PINA, &PINC);

  // Read config from eeprom
  Config_read(config, &eeprom);
  
  // Setup INT1, INT2 and PCINT8 pins
  DDRB &= ~(1<<PB2);
  PORTB |= (1<<PB2);

  DDRD &= ~(1<<PD3);
  PORTD |= (1<<PD3); 

  DDRB &= ~(1<<PB0);
  PORTB |= (1<<PB0);
  
  // Setup Interrupts
  EICRA = (1<<ISC11) | (1<<ISC21); // Sense INT1 and INT2 on falling edge
  EIMSK = (1<<INT1);               // Enable interrupt for INT1 (vsync)

  // Setupt Pin Change Interrupt PCINT8 (hsync)
  PCICR = (1<<PCIE1);
  PCMSK1 = (1<<PCINT8);

  // Turn off ADC  
  ADCSRA &= ~(1<<ADEN); 

  // Turn off JTAG
  MCUCR |= (1<<JTD);
  
  // Setup SPI 
  DDRB = SS | MOSI;                // /SS and MOSI as outputs
  SPCR =
    (1<<SPE) | (1<<MSTR) |         // Enable SPI as Master
    (1<<CPHA) | (1<<CPOL);         // Setup with falling edge of SCK
  SPSR = (1<<SPI2X);               // Double speed
  
  // Setup OE (PD6) and OR (PD7) as inputs with pullups
  DDRD  &= ~OE; PORTD |= OE;
  DDRD  &= ~OR; PORTD |= OR;

  // Setup Input ports
  DDRA = 0x00;
  PORTA = 0xff;

  DDRC = 0x00;
  PORTC = 0xff;
  
  // Enable Interrupts
  sei();   
}

//------------------------------------------------------------------------------

ISR(INT1_vect) { // VSYNC (each frame)...

  // Decrease timeout counter
  if(timeout > 0) timeout--;
   
  // Reset scanline counter
  scanline = 0;
}

//------------------------------------------------------------------------------

ISR(PCINT1_vect) { // HSYNC (each line)...

  // Return immediately unless pin has changed to low
  if(PINB & (1<<PB0)) return;

  // When we enter here, there's a jitter of 1-3 cycles due to code
  // executed in the mainloop. Therefore we'll use the BACK PORCH
  // signal from the LM1881, which is triggered 8us after HSYNC, to
  // trigger another interrupt while only executing NOPS. This way the
  // next interrupt gets triggered after a fixed number of cycles, and
  // the display is stable.
  
  // Disable PCINT1 (HSYNC), enable INT2 (BACK PORCH)
  PCICR = 0;
  EIMSK |= (1<<INT2);
  
  // Enable interrupt handling
  sei();

  // Enter INT2_vect somewhere along these nops...
  ONEHUNDRED_AND_TEN_NOPS();

  // We're back from INT2_vect, at the end of the line
  
  // Disable interrupt handling again
  cli();
  
  // Disable INT2 (BACK PORCH), enable PCINT1 again (HSYNC)
  EIMSK &= ~(1<<INT2);
  PCICR = (1<<PCIE1);
}

//------------------------------------------------------------------------------

ISR(INT2_vect) { // BACK PORCH (8us after HSYNC)
  
  uint8_t* row;   // Character data for the current row
  uint8_t line;   // Logical line of the visible screen
  uint8_t column; // Current character column in the current row 
  uint8_t byte;   // Current byte offset into the character bitmap
  
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
    
    // ...and wait until the visible area is reached
    FORTYTWO_NOPS();

    // Bitbang font data for each column via SPI
    for(column=0; column<SCREEN_COLUMNS; column++) {
      SPDR = font[row[column]*CHAR_HEIGHT+byte];
      TWO_NOPS();
    }
  }  
  else {
    // The display is not enabled or we're outside of the logical screen
    // or there is nothing to display on this row...
    
  skip:
    // We'll tristate the SPI pin again
    DISABLE_SPI;
  }
}

//------------------------------------------------------------------------------

int main(void) {

  setup();

  while(1) {
    
    // Sample input lines and update screen according to user config
    Config_apply(config);

    // Get the current state of the control lines and also remember the
    // previous states so that we can detect rising or falling edges    

    READ(output_enable, PIND, OE);
    READ(output_request, PIND, OR);

    // When OE is low, the output is always enabled
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
  
  return 0;    
}

//------------------------------------------------------------------------------


