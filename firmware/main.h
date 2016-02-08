#ifndef MAIN_H
#define MAIN_H

#define TICKS_PER_USEC F_CPU/1000000UL
#define US(n) n*(TICKS_PER_USEC)

#define NOP() do { __asm__ __volatile__ ("nop"); } while (0)
#define NOPS(n) for(uint8_t i=0; i<n; i++) NOP()

#define READ(reg, pins, pin) reg[0] = reg[1]; reg[1] = (pins & pin) ? 1 : 0
#define LOW(reg) (!reg[1])
#define HIGH(reg) (reg[1])
#define RISING(reg) (!reg[0] && reg[1])
#define FALLING(reg) (reg[0] && !reg[1])

#endif // MAIN_H
