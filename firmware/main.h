#ifndef MAIN_H
#define MAIN_H

#include <avr/cpufunc.h>

#define TICKS_PER_USEC F_CPU/1000000UL
#define US(n) n*(TICKS_PER_USEC)

#define READ(reg, pins, pin) reg[0] = reg[1]; reg[1] = (pins & pin) ? 1 : 0
#define LOW(reg) (!reg[1])
#define HIGH(reg) (reg[1])
#define RISING(reg) (!reg[0] && reg[1])
#define FALLING(reg) (reg[0] && !reg[1])

#endif // MAIN_H
