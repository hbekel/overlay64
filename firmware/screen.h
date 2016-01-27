#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define CHAR_WIDTH    8
#define CHAR_HEIGHT   8

#define SCREEN_WIDTH  4
#define SCREEN_HEIGHT 4
#define SCREEN_TOP    32
#define SCREEN_BOTTOM SCREEN_TOP + CHAR_HEIGHT * SCREEN_HEIGHT

uint8_t screen[SCREEN_WIDTH*SCREEN_HEIGHT];

void write(uint8_t *screen, uint8_t row, uint8_t col, char* str);
void clear(uint8_t *screen, uint8_t row, uint8_t col, uint8_t len);

#endif // SCREEN_H

