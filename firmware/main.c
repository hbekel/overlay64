//------------------------------------------------------------------------------
// Overlay64 -- Video Overlay Driver -- Atmega1284 @ 20MHz
//------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "main.h"
#include "font.h"
#include "eeprom.h"
#include "config.h"
#include "usbdrv/usbdrv.h"
#include "../protocol.h"

#define ATTR_NO_INIT __attribute__ ((section(".noinit")))
#define ATTR_INIT_SECTION_3 __attribute__ ((used, naked, section(".init3")))

#define MAGIC 0xDEADBEEF
#define BOOTLOADER (0x20000 - 0x1000)
uint32_t BootKey ATTR_NO_INIT;
void CheckBootloader(void) ATTR_INIT_SECTION_3;

#define MOSI (1<<PB5)  // SPI output pin (where the actual bitmap data is send)
#define SS   (1<<PB4)  // SPI slave select (must be pulled up in master mode)

#define ENABLE_SPI  DDRB |= MOSI  // Enable the SPI output pin
#define DISABLE_SPI DDRB &= ~MOSI // Disable (tristate) the SPI output pin

volatile uint16_t scanline; // current scanline of the whole video frame

volatile Config* config;    // Configuration is read from eeprom

//------------------------------------------------------------------------------

static void setup() {

  wdt_disable();
  
  // Create config and assign ports
  config = Config_new_with_ports(&PINA, &PINB, &PINC, &PIND);

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

  // Setup USB
  usbInit();

  usbDeviceDisconnect();
  
  _delay_ms(500);

  usbDeviceConnect();

  _delay_ms(1000);

  // Setup input and control lines
  Config_setup(config);
  
  // Enable Interrupts
  sei();   
}

//------------------------------------------------------------------------------

ISR(INT1_vect) { // VSYNC (each frame)...

  // Decrease timeout counters for all screens
  Config_tick(config);
   
  // Reset scanline counter
  scanline = 0;
}

//------------------------------------------------------------------------------

ISR(PCINT1_vect) { // HSYNC (each line)...

  // Return immediately unless pin has changed to low
  if(PINB & (1<<PB0)) return;

  // When we enter here, there's a jitter of 1-3 cycles due to code
  // executed in the mainloop. Therefore we'll use the BACK PORCH
  // signal from the LM1881, which is triggered about 8us after HSYNC,
  // to trigger another interrupt while only executing NOPS. This way
  // the next interrupt gets triggered after a fixed number of cycles,
  // and the display is stable.
  
  // Disable PCINT1 (HSYNC), disable INT0 (USB), enable INT2 (BACK PORCH)
  PCICR = 0;
  EIMSK |= (1<<INT2);
  EIMSK &= ~(1<<INT0);
  
  // Enable interrupt handling
  sei();

  // Enter INT2_vect somewhere along these nops...
  ONEHUNDRED_AND_TEN_NOPS();

  // Now we're back from INT2_vect, at the end of the line
  
  // Disable interrupt handling again
  cli();
  
  // Disable INT2 (BACK PORCH), enable PCINT1 (HSYNC) and INT0 (USB) again
  EIMSK &= ~(1<<INT2);
  EIMSK |= (1<<INT0);
  PCICR = (1<<PCIE1);
}

//------------------------------------------------------------------------------

ISR(INT2_vect) { // BACK PORCH (8us after HSYNC)
  
  uint8_t* row;   // Character data for the current row
  uint8_t line;   // Logical line of the visible screen
  uint8_t column; // Current character column in the current row 
  uint8_t byte;   // Current byte offset into the character bitmap
  
  scanline++;

  if(config->enabled && (scanline >= SCREEN_TOP && scanline < SCREEN_BOTTOM)) {

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

void CheckBootloader(void) {
  if((MCUSR & (1 << WDRF)) && (BootKey == MAGIC)) {
    BootKey = 0;
    ((void (*)(void))BOOTLOADER)();
  }
}

//------------------------------------------------------------------------------

void EnterBootloader(void) {
  cli();
  BootKey = MAGIC;
  wdt_enable(WDTO_250MS);
  for(;;);
}

//------------------------------------------------------------------------------

USB_PUBLIC usbMsgLen_t usbFunctionSetup(uint8_t data[8]) {

  usbRequest_t *request = (void*) data;

  switch(request->bRequest) {
    
  case OVERLAY64_BOOT:
    EnterBootloader();
    break;
    
  default:
    break;
  }
  return 0;
}

//------------------------------------------------------------------------------

int main(void) {

  setup();

  while(1) {

    // Poll for USB messages
    usbPoll();
    
    // Sample input and control lines and update screen according to user config
    Config_apply(config);
  }
  
  return 0;    
}

//------------------------------------------------------------------------------


