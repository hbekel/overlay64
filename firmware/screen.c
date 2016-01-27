static void write(uint8_t *screen, uint8_t row, uint8_t col, char* str) {

  uint8_t len = strlen(str);
  screen += row * SCREEN_WIDTH + col;
  
  for(uint8_t i=0; i<len; i++) {
    screen[i] = (uint8_t) str[i]-0x20;
  }    
}

static void clear(uint8_t *screen, uint8_t row, uint8_t col, uint8_t len) {
  screen += row * SCREEN_WIDTH + col;
  for(uint8_t i=0; i<len; i++) {
    screen[i] = 0x00;
  }
}
