#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "screen.h"
#include "font.h"

#include "screen.c"

void putbyte(uint8_t byte) {
  for(int i=7; i>=0; i--) {
    printf("%s", (byte & (1<<(i))) ? "X" : " ");
  }
}

int main(int argc, char **argv) {

  write(screen, 0, 0, "TEST");
  write(screen, 1, 0, "OSD!");
  write(screen, 2, 0, "DEL!");
  write(screen, 3, 0, "+-*/");
  
  for(int line=0; line<SCREEN_HEIGHT*CHAR_HEIGHT; line++) {

    int row = line/CHAR_HEIGHT;
    int pos = line%CHAR_HEIGHT;    

    putbyte(font[screen[row*SCREEN_WIDTH+0]*CHAR_HEIGHT+pos]);
    putbyte(font[screen[row*SCREEN_WIDTH+1]*CHAR_HEIGHT+pos]);
    putbyte(font[screen[row*SCREEN_WIDTH+2]*CHAR_HEIGHT+pos]);
    putbyte(font[screen[row*SCREEN_WIDTH+3]*CHAR_HEIGHT+pos]);    

    printf("\n");
  }

  printf("Deleting line 3...\n\n");
  clear(screen, 2, 0, 4);
  
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


