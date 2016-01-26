#define F_CPU 20000000UL
#define TICKS_PER_USEC F_CPU/1000000UL

#define Wait() while(!(SPSR & (1<<SPIF)))
#define US(a) a*TICKS_PER_USEC

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// LM1881 CSYNC -> INT0 (Pin 4)
// LM1881 VSYNC -> INT1 (Pin 5)
// Atmega OUT   -> MOSI (Pin 17)

int line=0;
uint8_t test = 0b10110001;

ISR(INT1_vect) {
  TCNT1 = 0; //start the timer

  //wait 180us for the vertical blanking period to end
  while(TCNT1<US(180));  
  line=0;
}

ISR(INT0_vect) {
  line++;
  
  if (line > 36 && line < 238) {	
    TCNT1=0;
    
    // horizontal gap before first visible pixel?
    // theoretically this should be sync 4us + back porch 8us = 12us +- x
    // given that the lm1881 triggers CSYNC on the falling edge of the sync
    // x -= some time until we get here (interrupt overhead + code above)
    // x += some time until first visible pixel... include a safety margin
    
    while (TCNT1 < US(12)); 

    // are my pixels square? How long is a single SPI bit pulse?
    // is there a gap between bytes? (overhead of SPI/wait)
    // how many bytes fit on a line?
    // what does low/high result in?
    // do i have enough time for pointing into a character buffer?
    // (including pointer arithmetic?)

    // try four bytes for a start...
    
    SPDR = test; Wait();  
    SPDR = test; Wait();  
    SPDR = test; Wait();  
    SPDR = test; Wait();            
  }
}

int setup() {
  TCCR1B = (1<<CS10) + (1<<WGM12);
  TIMSK1=0;
  OCR1A = 0xffff;
  
  EICRA = (0<<ISC01) + (0<<ISC11); // Set interrupt on falling edge
  EIMSK = (1<<INT0) + (1<<INT1);   // Enable interrupts for int0 and int1

  ADCSRA&=~(1<<ADEN); // Turn off ADC

  //Setup SPI
  SPDR = 0;
  DDRB = (1<<DDB3) | (1<<DDB5);
  SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (0<<CPOL);
  SPSR = (1<<SPI2X);

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


