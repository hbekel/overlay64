#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "intelhex/kk_ihex.h"
#include "intelhex/kk_ihex_read.h"
#include "intelhex/kk_ihex_read.c"

uint8_t* _buffer = NULL;
int _size = 0;

uint8_t* readhex(uint8_t *data, int *size) {
  
  _buffer = (uint8_t*) calloc(0x20000, sizeof(uint8_t));
  _size = 0;
  
  struct ihex_state ihex;
  ihex_begin_read(&ihex);
  ihex_read_bytes(&ihex, (const char*) data, (*size));
  ihex_end_read(&ihex);

  if(data) free(data);
  (*size) = _size;
  return _buffer;
}

ihex_bool_t ihex_data_read (struct ihex_state *ihex,
                            ihex_record_type_t type,
                            ihex_bool_t checksum_error) {

  if(type == IHEX_DATA_RECORD) {
    unsigned long address = (unsigned long) IHEX_LINEAR_ADDRESS(ihex);

    for(int i=0; i<ihex->length; i++) {
      _buffer[address+i] = ihex->data[i];
    }
    _size += ihex->length;    
  } 
  return true;
}
