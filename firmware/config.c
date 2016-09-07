#include <avr/io.h>

#include "config.h"
#include "string.h"

static bool enabled;

//------------------------------------------------------------------------------

void Config_setup(volatile Config* self) {
  // TODO: implement dynamically according to config

#define OE (1<<PD6)
#define OR (1<<PD7)
  
  DDRD  &= ~OE; PORTD |= OE;
  DDRD  &= ~OR; PORTD |= OR;

  DDRA = 0x00;
  PORTA = 0xff;

  DDRC = 0x00;
  PORTC = 0xff;
}

//------------------------------------------------------------------------------

void Config_tick(volatile Config* self) {
  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen *screen = self->screens[i];
    if(screen->timeout > 0) {
      screen->timeout--;
    }
  }
}

//------------------------------------------------------------------------------

void Config_apply(volatile Config* self) {

  enabled = false;

  for(uint8_t i=0; i<CONTROL_PINS; i++) {
    Pin_sample(self->control[i]);
  }
  
  for(uint8_t i=0; i<INPUT_PINS; i++) {
    Pin_sample(self->input[i]);
  }
  
  for(uint8_t i=0; i<self->num_controls; i++) {
    Control* control = self->controls[i];    
    Control_sample(control);
  }
  
  for(uint8_t i=0; i<self->num_screens; i++) {    
    Screen* screen = self->screens[i];    
    Screen_sample(screen);
    
    enabled = enabled || screen->enabled;
  }
  
  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen* screen = self->screens[i];
    if(!screen->enabled) {
      Screen_clear(screen);
    }
  }

  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen* screen = self->screens[i];
    if(screen->enabled) {
      Screen_write(screen);
    }
  }
  self->enabled = enabled;
}

//------------------------------------------------------------------------------

void Control_sample(Control* self) {
  self->asserted =
    (self->mode == MODE_MANUAL && Pin_is_low(self->pin)) ||
    (self->mode == MODE_NOTIFY && Pin_is_rising(self->pin));
}

//------------------------------------------------------------------------------

void Screen_sample(Screen* self) {

  self->enabled = false;

  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample_sample(self->samples[i], self);
  }

  for(uint8_t i=0; i<self->num_controls; i++) {
    Control *control = self->controls[i];
    
    if(control->asserted) {
      if(control->mode == MODE_NOTIFY) {
        self->timeout = config->timeout;
      }
      else {
        self->enabled = self->enabled || control->asserted;
      }
    }
  }
  self->enabled = self->enabled || self->timeout > 0;
}

//------------------------------------------------------------------------------

void Screen_notify(Screen* self) {
  if(self->mode == MODE_NOTIFY) {
    self->timeout = config->timeout;
  }
}

//------------------------------------------------------------------------------

void Screen_write(Screen* self) {
  CommandList_execute(self->commands);

  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample* sample = self->samples[i];
    CommandList *commands = sample->command_lists[sample->value];
    CommandList_execute(commands);
  }
}

//------------------------------------------------------------------------------

void Screen_clear(Screen* self) {
  CommandList_execute_clear(self->commands);

  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample* sample = self->samples[i];

    for(uint8_t k=0; k<sample->num_command_lists; k++) {
      CommandList* commands = sample->command_lists[k];
      CommandList_execute_clear(commands);
    }
  }  
}

//------------------------------------------------------------------------------

void Sample_sample(Sample* self, Screen* screen) {
  Pin *pin;
  self->value = 0;
  
  for(uint8_t i=0; i<self->num_pins; i++) {
    pin = self->pins[i];

    self->value |= (Pin_state(pin) << i);
    
    if(Pin_has_changed(pin)) {
      Screen_notify(screen);
    }
  }
}

//------------------------------------------------------------------------------

uint8_t Pin_sample(Pin* self) {
  self->edge[0] = self->edge[1];
  self->edge[1] = ((*(self->port)) & (1<<self->pos)) ? 1 : 0;
  return self->edge[1];
}

//------------------------------------------------------------------------------

uint8_t Pin_state(Pin *self) {
  return self->edge[1];
}

//------------------------------------------------------------------------------

bool Pin_is_high(Pin* self) {
  return self->edge[1];
}

//------------------------------------------------------------------------------

bool Pin_is_low(Pin* self) {
 return self->edge[1];
}

//------------------------------------------------------------------------------

bool Pin_is_rising(Pin* self) {
  return (self->edge[0] == 0) && (self->edge[1] == 1);
}

//------------------------------------------------------------------------------

bool Pin_is_falling(Pin* self) {
  return (self->edge[0] == 1) && (self->edge[0] == 1);
}

//------------------------------------------------------------------------------

bool Pin_has_changed(Pin* self) {
  return self->edge[0] != self->edge[1];
}

//------------------------------------------------------------------------------

void CommandList_execute(CommandList* self) {
  for(uint8_t i=0; i<self->num_commands; i++) {
    Command_execute(self->commands[i]);
  }
}

//------------------------------------------------------------------------------

void CommandList_execute_clear(CommandList* self) {
  for(uint8_t i=0; i<self->num_commands; i++) {
    Command_execute_clear(self->commands[i]);
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

void Command_execute_clear(Command* self) {

  uint8_t* row = config->rows[self->row];
  
  if(self->action == ACTION_WRITE) {
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
  for(uint8_t i=0; i<len; i++) {
    dst[i] = (uint8_t) 0x00;
  }
}

//------------------------------------------------------------------------------
