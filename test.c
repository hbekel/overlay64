/*
overlay64 -- video overlay module
Copyright (C) 2016 Henning Bekel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "parser.h"
#include "firmware/config.h"
#include "firmware/font.h"

extern volatile Config* config;

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
