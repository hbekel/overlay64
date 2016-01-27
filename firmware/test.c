#include <stdint.h>
#include <stdio.h>

#include "screen.h"
#include "font.h"

void putbyte(uint8_t byte) {
  for(int i=7; i>=0; i--) {
    printf("%s", (byte & (1<<(i))) ? "X" : " ");
  }
}

int main(int argc, char **argv) {

  screen[0] = 0x21;
  screen[1] = 0x22;
  screen[2] = 0x23;
  screen[3] = 0x24;

  screen[4] = 0x24;
  screen[5] = 0x23;
  screen[6] = 0x22;
  screen[7] = 0x21;

  screen[8] = 0x21;
  screen[9] = 0x21;
  screen[10] = 0x23;
  screen[11] = 0x23;

  screen[12] = 0x22;
  screen[13] = 0x22;
  screen[14] = 0x24;
  screen[15] = 0x24;
  
  for(int line=0; line<SCREEN_HEIGHT*CHAR_HEIGHT; line++) {

    int row = line/CHAR_HEIGHT;
    int pos = line%CHAR_HEIGHT;    

    putbyte(font[screen[row*SCREEN_WIDTH+0]*CHAR_HEIGHT+pos]);
    putbyte(font[screen[row*SCREEN_WIDTH+1]*CHAR_HEIGHT+pos]);
    putbyte(font[screen[row*SCREEN_WIDTH+2]*CHAR_HEIGHT+pos]);
    putbyte(font[screen[row*SCREEN_WIDTH+3]*CHAR_HEIGHT+pos]);    

    printf("\n");
  }

  for(int line=0; line<SCREEN_HEIGHT*CHAR_HEIGHT; line++) {

    int chr = line/CHAR_HEIGHT*SCREEN_WIDTH;
    int lin = line%CHAR_HEIGHT;    

    putbyte(font[screen[chr++]*CHAR_HEIGHT+lin]);
    putbyte(font[screen[chr++]*CHAR_HEIGHT+lin]);
    putbyte(font[screen[chr++]*CHAR_HEIGHT+lin]);
    putbyte(font[screen[chr++]*CHAR_HEIGHT+lin]);    

    printf("\n");
  }

}


