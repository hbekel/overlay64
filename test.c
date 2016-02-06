#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "parser.h"
#include "firmware/config.h"
#include "firmware/font.h"

volatile Config* config;

void putbyte(uint8_t byte) {
  for(int i=7; i>=0; i--) {
    printf("%s", (byte & (1<<(i))) ? "X" : " ");
  }
}

int main(int argc, char **argv) {
  config = Config_new();

  Config_parse(config, stdin);

  Config_apply(config);
  
  for(int line=0; line<SCREEN_ROWS*CHAR_HEIGHT; line++) {

    uint8_t* row = config->rows[line/CHAR_HEIGHT];
    uint8_t byte = line%CHAR_HEIGHT;

    if(row == NULL) continue;

    for(int col=0; col<SCREEN_COLUMNS; col++) {
      putbyte(font[row[col]*CHAR_HEIGHT+byte]);
    }
    printf("\n");
  }

  Config_free(config);
    
  return 0;
}

