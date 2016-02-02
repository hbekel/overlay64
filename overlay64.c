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
    fprintf(stderr, "EEPROM:\t%4d bytes\n", written);
  }
  else {  
    return EXIT_FAILURE;
  }

  fprintf(stderr, "SRAM:\t%4d bytes\n",
          Config_get_footprint(config));
  
  Config_free(config);  
  return EXIT_SUCCESS;
}

