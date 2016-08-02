#ifndef MAIN_H
#define MAIN_H

#include <avr/cpufunc.h>

#define TICKS_PER_USEC F_CPU/1000000UL
#define US(n) n*(TICKS_PER_USEC)

#define TWO_NOPS() _NOP(); _NOP()
#define TEN_NOPS() TWO_NOPS(); TWO_NOPS(); TWO_NOPS(); TWO_NOPS(); TWO_NOPS()
#define ONEHUNDRED_NOPS() TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS()
#define FORTYTWO_NOPS() TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TEN_NOPS(); TWO_NOPS()
#define ONEHUNDRED_AND_TEN_NOPS() ONEHUNDRED_NOPS(); TEN_NOPS()

#define READ(reg, pins, pin) reg[0] = reg[1]; reg[1] = (pins & pin) ? 1 : 0
#define LOW(reg) (!reg[1])
#define HIGH(reg) (reg[1])
#define RISING(reg) (!reg[0] && reg[1])
#define FALLING(reg) (reg[0] && !reg[1])

#endif // MAIN_H
