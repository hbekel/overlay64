#include <stdlib.h>

#include "parser.h"

int main(int argc, char **argv) {

  config = Config_new();

  if((Config_read(config, stdin) || Config_parse(config, stdin))) {

    Config_write(config, stdout);

    fprintf(stderr, "memory footprint is %d bytes\n", Config_get_footprint(config));
    
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

