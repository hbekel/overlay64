#define F_CPU 16000000UL
#define TICKS_PER_USEC F_CPU/1000000UL

#define Wait() while(!(SPSR & (1<<SPIF)))
#define US(a) a*(TICKS_PER_USEC)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

// LM1881 CSYNC -> INT0 (Pin 4)
// LM1881 VSYNC -> INT1 (Pin 5)
// Atmega OUT   -> MOSI (Pin 17)

static volatile uint16_t line=0;
uint8_t test[3*7] = {
  0b11111110, 0b11111110, 0b11000110,
  0b11000000, 0b11000000, 0b11000110,
  0b11000000, 0b11000000, 0b11000110,
  0b11000000, 0b11111110, 0b11111110,
  0b11000000, 0b11000110, 0b00000110,   
  0b11000000, 0b11000110, 0b00000110,
  0b11111110, 0b11111110, 0b00000110,  
};

ISR(INT1_vect) {
  //wait 180us for the vertical blanking period to end
  TCNT1 = 0;
  while(TCNT1<US(180));
  line=0;
}

ISR(INT0_vect) {
  uint16_t tmp;
  line++;
  
  TCNT1=0;  

  if(line >= 34 && line <= 40) {

    while(TCNT1<US(9));   

    tmp = (line-34)*3;
    SPDR = test[tmp+0]; _delay_us(2);
    SPDR = test[tmp+1]; _delay_us(2);
    SPDR = test[tmp+2]; _delay_us(2);
    return;
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


