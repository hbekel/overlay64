#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define CHAR_WIDTH    8
#define CHAR_HEIGHT   8

#define SCREEN_COLUMNS 45
#define SCREEN_ROWS    30

#define SCREEN_TOP    40
#define SCREEN_BOTTOM SCREEN_TOP + CHAR_HEIGHT * SCREEN_ROWS

typedef struct Config Config;

void Write(volatile Config *config, uint8_t row, uint8_t col, char* str);
void Clear(volatile Config *config, uint8_t row, uint8_t col, uint8_t len);

#endif // SCREEN_H

