#include <stdlib.h>

#include "parser.h"

extern uint16_t written;

int main(int argc, char **argv) {

  config = Config_new();

  if(Config_read(config, stdin)) {
    Config_print(config, stdout);
  }
  else if(Config_parse(config, stdin)) {
    Config_write(config, stdout);
    fprintf(stderr, "EEPROM:\t%4d of  512 bytes used (%4d bytes free)\n",
            written, 512-written);
  }
  else {  
    return EXIT_FAILURE;
  }

  uint16_t footprint = Config_get_footprint(config);
  
  fprintf(stderr, "SRAM:\t%4d of 2048 bytes used (%4d bytes free)\n",
          footprint, 2048-footprint);
  
  Config_free(config);  
  return EXIT_SUCCESS;
}

