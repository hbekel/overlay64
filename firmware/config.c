#include "config.h"
#include "string.h"

//------------------------------------------------------------------------------

void Config_apply(volatile Config* self) {

  CommandList_execute(self->immediateCommands);

  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample_apply(self->samples[i]);
  }
}

//------------------------------------------------------------------------------

void Sample_apply(Sample* self) {
  uint8_t value = 0;

  for(uint8_t i=0; i<self->num_pins; i++) {
    value |= (Pin_read(self->pins[i]) << i);
  }
  CommandList_execute(self->commands[value]);
}

//------------------------------------------------------------------------------

uint8_t Pin_read(Pin* self) {
  return ((*(self->port)) & (1<<self->pos)) ? 1 : 0;
}

//------------------------------------------------------------------------------

void CommandList_execute(CommandList* self) {
  for(uint8_t i=0; i<self->num_commands; i++) {
    Command_execute(self->commands[i]);
  }
}

//------------------------------------------------------------------------------

void Command_execute(Command* self) {

  uint8_t* row = config->rows[self->row];
  
  if(self->action == ACTION_WRITE) {
    Row_write(row, self->col, self->string);
  }
  else if(self->action == ACTION_CLEAR) {
    Row_clear(row, self->col, self->len);
  }
}

//------------------------------------------------------------------------------

void Row_write(uint8_t* row, uint8_t col, char *str) {
  uint8_t* dst = row+col;
  uint8_t len = strlen(str);
  
  for(uint8_t i=0; i<len; i++) {
    dst[i] = (uint8_t) str[i]-0x20;
  }
}

//------------------------------------------------------------------------------

void Row_clear(uint8_t* row, uint8_t col, uint8_t len) {
  uint8_t* dst = row+col;
  memset(dst, 0, len);
}

//------------------------------------------------------------------------------
