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

