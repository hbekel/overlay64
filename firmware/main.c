//-----------------------------------------------------------------------------
// Overlay64 -- Video Overlay Driver -- Atmega1284 @ 20MHz
//-----------------------------------------------------------------------------

/*
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "font.h"
#include "eeprom.h"
#include "config.h"
#include "usbdrv/usbdrv.h"
#include "../protocol.h"

#define MOSI (1<<PB5)  // SPI output pin (where the actual bitmap data is send)
#define SS   (1<<PB4)  // SPI slave select (must be pulled up in master mode)

#define ENABLE_SPI  DDRB |= MOSI  // Enable the SPI output pin
#define DISABLE_SPI DDRB &= ~MOSI // Disable (tristate) the SPI output pin

volatile uint16_t scanline; // current scanline of the whole video frame

volatile Config* config;    // Configuration is read from eeprom

static volatile bool reset = false; // Requests a reset from USB
static volatile char version[64];   // Version string

static volatile uint8_t usbCommand;
static volatile uint16_t usbDataReceived;
static volatile uint16_t usbDataLength;
static volatile uint8_t *usbData;
static volatile uint16_t usbDataPos;

//-----------------------------------------------------------------------------

static void setup() {

  wdt_disable();
  CheckBootloader();
  
  // Initialize Version string and font data
  SetupVersionString();  
  SetupFont();
  
  // Create config and assign ports
  config = Config_new_with_ports(&PINA, &PINB, &PINC, &PIND);

  // Read config from eeprom
  Config_read(config, &eeprom) || Config_install_fallback(config);

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

  // Setup input and control lines
  Config_setup(config);
  
  // Setup USB
  usbInit();

  usbDeviceDisconnect();
  
  _delay_ms(500);

  usbDeviceConnect();
  
  // Enable Interrupts
  sei();   
}

//-----------------------------------------------------------------------------

void DisableDisplay(void) {
  EIMSK &= ~(1<<INT2);
  PCICR = 0; 
}

//-----------------------------------------------------------------------------

ISR(INT1_vect) { // VSYNC (each frame)...

  // Decrease timeout counters for all screens
  Config_tick(config);
   
  // Reset scanline counter
  scanline = 0;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void Reset(void) {
  cli();
  wdt_enable(WDTO_250MS);
  for(;;);
}

//-----------------------------------------------------------------------------

void CheckBootloader(void) {
  DDRD &= ~(1<<PD0);
  PORTD |= (1<<PD0);

  if((PIND & (1<<PD0)) == 0) {
    EnterBootloader();
    Reset();
  }
}

//-----------------------------------------------------------------------------

void EnterBootloader(void) {
  eeprom_update_word((uint16_t *)0x0ffe, (uint16_t) 0xb0b0);
  reset = true;
}

//-----------------------------------------------------------------------------

void SetupVersionString(void) {

  char *v = "overlay64 firmware " xstr(VERSION) " " __DATE__ " " __TIME__;
  int len = strlen(v);

  for(uint8_t i=0; i<64; i++) {
    version[i] = '\0';
  }
  uint8_t o=0;
  
  for(uint8_t i=0; i<strlen(v); i++) {
    if(i>1 && v[i-1] == ' ' && v[i] == ' ') continue;
    version[o++] = v[i];
  }
}

//-----------------------------------------------------------------------------

USB_PUBLIC usbMsgLen_t usbFunctionSetup(uint8_t data[8]) {

  usbRequest_t *usbRequest = (void*) data;

  switch(usbRequest->bRequest) {

  case OVERLAY64_FLASH:
    DisableDisplay();
    
    usbCommand = usbRequest->bRequest;
    usbDataLength = usbRequest->wLength.word;
    usbDataReceived = 0;
    
    if(usbData == NULL) {
      usbData = (uint8_t*) calloc(1, sizeof(uint8_t) * usbDataLength);
    }
    
    return USB_NO_MSG;
    break;
    
  case OVERLAY64_BOOT:
    DisableDisplay();
    EnterBootloader();   
    break;

  case OVERLAY64_RESET:
    eeprom_update_word((uint16_t *)0x0ffe, (uint16_t) 0xffff);
    reset = true;
    break;
    
  case OVERLAY64_IDENTIFY:
    usbMsgPtr = (uchar *) version;
    return strlen((const char*)version)+1;
    break;
    
  default:
    break;
  }
  return 0;
}

//-----------------------------------------------------------------------------

USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {

  for(int i=0; usbDataReceived < usbDataLength && i < len; i++, usbDataReceived++) {
    usbData[usbDataReceived] = (uint8_t) data[i];
  }

  if(usbDataReceived < usbDataLength) {
    return 0;
  }
  else {
    if(usbCommand == OVERLAY64_FLASH) {
      FlashConfigurationFromUSBData();
    }
  }
  
  free((void*)usbData);
  usbData = NULL;
  return 1;
}

//-----------------------------------------------------------------------------

void FlashConfigurationFromUSBData(void) {  
  fwrite((void*)usbData, sizeof(uint8_t), usbDataLength, &eeprom);
  eeprom_update_word((uint16_t *)0x0ffe, (uint16_t) 0xffff);
  reset = true;
}

//-----------------------------------------------------------------------------

void SetupFont(void) {
  for(uint16_t i=0; i<96*8; i++) {
    font[i] = pgm_read_byte(&(_font[i]));
  }
}

//-----------------------------------------------------------------------------

int main(void) {

  setup();

  while(1) {

    // Poll for USB messages
    usbPoll();

    // If reset is requested, allow USB communication to finish, then reset
    if(reset) {
      _delay_ms(250);
      Reset();
    }
    
    // Sample input and control lines and update screen according to user config
    Config_apply(config);
  }
  
  return 0;    
}

//-----------------------------------------------------------------------------


