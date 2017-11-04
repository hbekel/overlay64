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

#include <avr/io.h>

#include "config.h"
#include "string.h"

static bool enabled;

//-----------------------------------------------------------------------------

void Config_setup(volatile Config* self) {
  Config_setup_pins(self);
  Config_sample_pins(self);
}

//-----------------------------------------------------------------------------

void Config_setup_pins(volatile Config* self) {
  for(uint8_t i=0; i<NUM_PINS; i++) {
    Pin_setup(self->pins[i]);
  }
}

//-----------------------------------------------------------------------------

void Config_sample_pins(volatile Config* self) {
  for(uint8_t i=0; i<NUM_PINS; i++) {
    Pin_sample(self->pins[i]);
  }  
}

//-----------------------------------------------------------------------------

void Config_tick(volatile Config* self) {
  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen *screen = self->screens[i];
    if(screen->timeout > 0) {
      screen->timeout--;
    }
  }
}

//-----------------------------------------------------------------------------

void Config_apply(volatile Config* self) {

  enabled = false;

  // Read the state of input and control pins
  Config_sample_pins(self);

  // Check wether any controls are asserted
  for(uint8_t i=0; i<self->num_controls; i++) {
    Control* control = self->controls[i];    
    Control_sample(control);
  }

  // Determine state of samples and screen state
  for(uint8_t i=0; i<self->num_screens; i++) {    
    Screen* screen = self->screens[i];    
    Screen_sample(screen);
    
    enabled = enabled || screen->enabled;
  }

  // unlink disabled screens from main screen
  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen* screen = self->screens[i];
    if(!screen->enabled) {
      Screen_unlink(screen);
    }
  }

  // write and link enabled screens into main screen
  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen* screen = self->screens[i];
    if(screen->enabled) {
      Screen_write(screen);
      Screen_link(screen);
    }
  }

  // Apply global enabled value only after it has
  // been fully determined
  self->enabled = enabled;
}

//-----------------------------------------------------------------------------

void Control_sample(Control* self) {
  self->asserted =
    (self->mode == MODE_MANUAL && Pin_is_low(self->pin)) ||
    (self->mode == MODE_NOTIFY && Pin_is_rising(self->pin));
}

//-----------------------------------------------------------------------------

void Screen_sample(Screen* self) {

  self->enabled = (self->mode == MODE_ALWAYS);

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
        self->timeout = 0;
        self->enabled = true;
      }
    }
  }
  self->enabled = self->enabled || (self->timeout > 0);  
  self->enabled = self->enabled && Screen_has_effect(self);
}

//-------------------------------------------------------------------------------

bool Screen_has_effect(Screen* self) {
  
  if(self->commands->num_commands) {
    return true;
  }

  for(uint8_t i=0; i<self->num_samples; i++) {
    if(Sample_has_effect(self->samples[i])) {
      return true;
    }
  }  
  return false;
}

//-----------------------------------------------------------------------------

void Screen_notify(Screen* self) {
  if(self->mode == MODE_NOTIFY) {
    self->timeout = config->timeout;
  }
}

//-----------------------------------------------------------------------------

void Screen_write(Screen* self) {
  CommandList_execute(self->commands);

  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample* sample = self->samples[i];
    CommandList_execute_with_value(sample->command_list, sample->value);

    CommandList *commands = sample->command_lists[sample->value];
    CommandList_execute(commands);
  }
}

//-----------------------------------------------------------------------------

void Screen_link(Screen* self) {
  for(uint8_t i=0; i<SCREEN_ROWS; i++) {
    if(self->rows[i] != NULL && config->rows[i] == NULL) {
      config->rows[i] = self->rows[i];
    }
  }
}

//-----------------------------------------------------------------------------

void Screen_unlink(Screen* self) {
  for(uint8_t i=0; i<SCREEN_ROWS; i++) {
    if(self->rows[i] == config->rows[i]) {
      config->rows[i] = NULL;
    }
  }
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

bool Sample_has_effect(Sample* self) {
  
  return self->command_list->num_commands ||
    self->command_lists[self->value]->num_commands;
}

//-----------------------------------------------------------------------------

void Pin_setup(Pin* self) {
  uint8_t mask = (1<<(self->pos));
  
  if(self->port == &PINA) {
    DDRA &= ~mask;
    PORTA |= mask;
  }

  if(self->port == &PINB) {
    DDRB &= ~mask;
    PORTB |= mask;
  }

  if(self->port == &PINC) {
    DDRC &= ~mask;
    PORTC |= mask;
  }

  if(self->port == &PIND) {
    DDRD &= ~mask;
    PORTD |= mask;
  }
}

//-----------------------------------------------------------------------------

uint8_t Pin_sample(Pin* self) {
  self->edge[0] = self->edge[1];
  self->edge[1] = ((*(self->port)) & (1<<self->pos)) ? 1 : 0;
  return self->edge[1];
}

//-----------------------------------------------------------------------------

uint8_t Pin_state(Pin *self) {
  return self->edge[1];
}

//-----------------------------------------------------------------------------

bool Pin_is_high(Pin* self) {
  return self->edge[1] == 1;
}

//-----------------------------------------------------------------------------

bool Pin_is_low(Pin* self) {
 return self->edge[1] == 0;
}

//-----------------------------------------------------------------------------

bool Pin_is_rising(Pin* self) {
  return (self->edge[0] == 0) && (self->edge[1] == 1);
}

//-----------------------------------------------------------------------------

bool Pin_is_falling(Pin* self) {
  return (self->edge[0] == 1) && (self->edge[0] == 1);
}

//-----------------------------------------------------------------------------

bool Pin_has_changed(Pin* self) {
  return self->edge[0] != self->edge[1];
}

//-----------------------------------------------------------------------------

void CommandList_execute(CommandList* self) {
  for(uint8_t i=0; i<self->num_commands; i++) {
    Command_execute(self->commands[i]);
  }
}

//-----------------------------------------------------------------------------

void CommandList_execute_with_value(CommandList* self, uint8_t value) {
  for(uint8_t i=0; i<self->num_commands; i++) {
    Command_execute_with_value(self->commands[i], value);
  }
}

//-----------------------------------------------------------------------------

void Command_execute(Command* self) {

  uint8_t* row = ((Screen *)self->screen)->rows[self->row];
  
  if(self->action == ACTION_WRITE) {
    Row_write(row, self->col, self->string);
  }
  else if(self->action == ACTION_CLEAR) {
    Row_clear(row, self->col, self->len);
  }
}

//-----------------------------------------------------------------------------

void Command_execute_with_value(Command* self, uint8_t value) {

  uint8_t* row = ((Screen *)self->screen)->rows[self->row];
  
  if(self->action == ACTION_WRITE) {

    if(strstr(self->string, "%") == NULL) {
      Row_write(row, self->col, self->string);
    }
    else {
      Row_printf(row, self->col, self->string, value);
    }         
  }
  else if(self->action == ACTION_CLEAR) {
    Row_clear(row, self->col, self->len);
  }
}

//-----------------------------------------------------------------------------

void Row_write(uint8_t* row, uint8_t col, char *str) {
  uint8_t* dst = row+col;
  uint8_t len = strlen(str);
  
  for(uint8_t i=0; i<len && col+i < SCREEN_COLUMNS; i++) {
    dst[i] = (uint8_t) str[i]-0x20;
  }
}

//-----------------------------------------------------------------------------

void Row_printf(uint8_t* row, uint8_t col, char *fmt, uint8_t value) {

  char tmp[SCREEN_COLUMNS+1];

  snprintf(tmp, SCREEN_COLUMNS, fmt,
           value, value, value, value, value, value, value, value,
           value, value, value, value, value, value, value, value);

  char* dst = (char*) (row+col);
  uint8_t len = strlen(tmp);

  for(uint8_t i=0; i<len && col+i < SCREEN_COLUMNS; i++) {
    dst[i] = (uint8_t) tmp[i]-0x20;
  }
}

//-----------------------------------------------------------------------------

void Row_clear(uint8_t* row, uint8_t col, uint8_t len) {
  uint8_t* dst = row+col;
  for(uint8_t i=0; i<len && col+i < SCREEN_COLUMNS; i++) {
    dst[i] = (uint8_t) 0x00;
  }
}

//-----------------------------------------------------------------------------
