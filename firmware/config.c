#include "config.h"

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

  Row* row = config->rows[self->row];
  
  if(self->action == ACTION_WRITE) {
    Row_write(row, self->col, self->string);
    //Config_row_write(config, self->row, self->col, self->string);
  }
  if(self->action == ACTION_CLEAR) {
    Row_clear(row, self->col, self->len);
    //Config_row_clear(config, self->row, self->col, self->len);
  }
}

//------------------------------------------------------------------------------
