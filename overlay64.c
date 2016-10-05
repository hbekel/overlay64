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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "target.h"

#if windows  
  #include <io.h>
  #include <fcntl.h>
#endif

#include "parser.h"
#include "boot.h"

extern uint16_t written;

int main(int argc, char **argv) {
  
  if(argc == 2 && (strncmp(argv[1], "boot", 4) == 0)) {
    exit(boot());    
  }
  config = Config_new();
  
#if windows
  setmode(_fileno(stdin), O_BINARY);
  setmode(_fileno(stdout), O_BINARY);  
#endif

  if(Config_read(config, stdin)) {
    Config_print(config, stdout);
  }
  else if(Config_parse(config, stdin)) {
    Config_write(config, stdout);
    fprintf(stderr, "EEPROM:\t%5d of  4096 bytes used (%5d bytes free)\n",
            written, 4096-written);
  }
  else {  
    return EXIT_FAILURE;
  }
  
  uint16_t footprint = Config_get_footprint(config);
  
  fprintf(stderr, "SRAM:\t%5d of 16384 bytes used (%5d bytes free)\n",
          footprint, 16384-footprint);
  
  Config_free(config);  
  return EXIT_SUCCESS;
}

