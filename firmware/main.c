#define F_CPU 20000000UL
#define F_CPU 14745600UL
#define MCU atmega8p
#define Wait() while(!(SPSR & (1<<SPIF)))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//LM1881 CSYNC -> INT0
//LM1881 VSYNC -> INT1

int line=0;
uint8_t test = 0b11100000;

ISR(INT1_vect) {
  TCNT1 = 0; //start the timer

  //wait 180us for the vertical blanking period to end 3640/3676 seems to be good
  while(TCNT1<3640);  
  line=0;
}

ISR(INT0_vect) {
  line++;
  
  if (line > 36 && line < 238) {	
    TCNT1=0;
    while (TCNT1 < 1100);
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


