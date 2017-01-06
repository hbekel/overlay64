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
#include <string.h>

#include "config.h"

uint8_t CONFIG_MAGIC[2] = { 'O', 'V' };

static uint8_t A = 0;
static uint8_t B = 1;
static uint8_t C = 2;
static uint8_t D = 3;

//------------------------------------------------------------------------------

volatile Config* Config_new(void) {
  return Config_new_with_ports(NULL, NULL, NULL, NULL);
}

//------------------------------------------------------------------------------

volatile Config* Config_new_with_ports(uint8_t volatile *a,
                                       uint8_t volatile *b,
                                       uint8_t volatile *c,
                                       uint8_t volatile *d) {

  uint8_t i = 0;
  
  volatile Config* self = (Config*) calloc(1, sizeof(Config));  
  
  self->ports[0] = a;
  self->ports[1] = b;
  self->ports[2] = c;
  self->ports[3] = d;

  self->enabled = false;
  self->timeout = 2*50; 

  self->controls = (Control**) calloc(1, sizeof(Control**));
  self->num_controls = 0;
  
  i = 0;
  self->pins[i++] = Pin_new(self, A, 0);
  self->pins[i++] = Pin_new(self, A, 1);
  self->pins[i++] = Pin_new(self, A, 2);
  self->pins[i++] = Pin_new(self, A, 3);
  self->pins[i++] = Pin_new(self, A, 4);
  self->pins[i++] = Pin_new(self, A, 5);
  self->pins[i++] = Pin_new(self, A, 6);
  self->pins[i++] = Pin_new(self, A, 7);

  self->pins[i++] = Pin_new(self, C, 7);
  self->pins[i++] = Pin_new(self, C, 6);
  self->pins[i++] = Pin_new(self, C, 5);
  self->pins[i++] = Pin_new(self, C, 4);
  self->pins[i++] = Pin_new(self, C, 3);
  self->pins[i++] = Pin_new(self, C, 2);
  self->pins[i++] = Pin_new(self, C, 1);
  self->pins[i++] = Pin_new(self, C, 0);

  self->pins[i++] = Pin_new(self, D, 6);
  self->pins[i++] = Pin_new(self, D, 7);
  self->pins[i++] = Pin_new(self, B, 1);
  self->pins[i++] = Pin_new(self, B, 3);
  self->pins[i++] = Pin_new(self, B, 6);
  self->pins[i++] = Pin_new(self, B, 7);
  self->pins[i++] = Pin_new(self, D, 4);
  self->pins[i++] = Pin_new(self, D, 5);  
  
  self->strings = (char**) NULL;
  self->num_strings = 0;

  self->rows = (uint8_t**) calloc(SCREEN_ROWS, sizeof(uint8_t*));

  return self;
}

//------------------------------------------------------------------------------

void Config_free(volatile Config* self) {

  for(uint8_t i=0; i<self->num_controls; i++) {
    Control_free(self->controls[i]);
  }
  free(self->controls);

  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen_free(self->screens[i]);
  }
  free(self->screens);

  for(uint8_t i=0; i<sizeof(self->pins)/sizeof(Pin*); i++) {
    Pin_free(self->pins[i]);
  }

  for(uint8_t i=0; i<SCREEN_ROWS; i++) {
    if(self->rows[i] != NULL) {
      free(self->rows[i]);
    }
  }

  free((void*)self);
}

//------------------------------------------------------------------------------

Control* Config_add_control(volatile Config *self, Control* control) {
  self->controls =
    (Control**) realloc(self->controls, (self->num_controls+1)*sizeof(Control**));

  self->controls[self->num_controls] = control;
  self->num_controls++;
  return control;  
}

//------------------------------------------------------------------------------

Screen* Config_add_screen(volatile Config *self, Screen* screen) {
  self->screens =
    (Screen**) realloc(self->screens, (self->num_screens+1)*sizeof(Screen**));

  self->screens[self->num_screens] = screen;
  self->num_screens++;
  return screen;  
}

//------------------------------------------------------------------------------

bool Config_has_string(volatile Config *self, char* string, uint8_t *index) {

  for(uint8_t i=0; i<self->num_strings; i++) {    
    if(strcmp(self->strings[i], string) == 0) {
      *index = i;
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

char* Config_add_string(volatile Config *self, char* string) {
  self->strings = (char**) realloc(self->strings, (self->num_strings+1) * sizeof(char *));
  self->strings[self->num_strings] = calloc(strlen(string)+1, sizeof(char));
  strncpy(self->strings[self->num_strings], string, strlen(string));
  self->num_strings++;
  return self->strings[self->num_strings-1];
}

//------------------------------------------------------------------------------

void Config_each_command(volatile Config* self,
                         void (*callback)
                         (Screen* screen, Command* command)) {

  Screen *screen;
  Sample *sample;
  CommandList *commands;
  Command *command;
  
  for(uint8_t i=0; i<self->num_screens; i++) {
    screen = self->screens[i];
    commands = screen->commands;

    for(uint8_t k=0; k<commands->num_commands; k++) {
      command = commands->commands[k];
      callback(screen, command);
    }
    
    for(uint8_t k=0; k<screen->num_samples; k++) {
      sample = screen->samples[k];
      
      for(uint8_t m=0; m<sample->num_command_lists; m++) {
        commands = sample->command_lists[m];
        
        for(uint8_t l=0; l<commands->num_commands; l++) {
          command = commands->commands[l];
          callback(screen, command);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------

void Config_allocate_row_for_command(Screen* screen, Command *command) {  
  if(screen->rows[command->row] == NULL) {
    screen->rows[command->row] = (uint8_t*) calloc(SCREEN_COLUMNS, sizeof(uint8_t));
  }
}

//------------------------------------------------------------------------------

void Config_allocate_rows(volatile Config *self) {
  Config_each_command(self, &Config_allocate_row_for_command);
}

//------------------------------------------------------------------------------

void Config_assign_controls_to_screens(volatile Config* self) {

  for(uint8_t i=0; i<self->num_controls; i++) {
    Control* control = self->controls[i];

    for(uint8_t k=0; k<control->num_screens; k++) {
      Screen* screen = self->screens[control->screens[k]];
      Screen_add_control(screen, control);
    }
  }
}

//------------------------------------------------------------------------------

bool Config_install_fallback(volatile Config* self) {

  Screen* screen = Screen_new();
  screen->mode = MODE_ALWAYS;
  
  Command* command;
  
  Config_add_string(self, "***** OVERLAY64 VERSION 1.0 READY *****");
  Config_add_string(self, "NO USER CONFIGURATION FOUND");
  Config_add_string(self, "(C)2016 HENNING BEKEL");    
  Config_add_string(self, "WWW.HENNING-BEKEL.DE/OVERLAY64");    

  command = Command_new(screen);  
  command->action = ACTION_WRITE;
  command->row = 0;
  command->col = 6;
  command->string = self->strings[0];
  CommandList_add_command(screen->commands, command);

  command = Command_new(screen);  
  command->action = ACTION_WRITE;
  command->row = 1;
  command->col = 12;
  command->string = self->strings[1];
  CommandList_add_command(screen->commands, command);

  command = Command_new(screen);  
  command->action = ACTION_WRITE;
  command->row = 28;
  command->col = 15;
  command->string = self->strings[2];
  CommandList_add_command(screen->commands, command);
  
  command = Command_new(screen);  
  command->action = ACTION_WRITE;
  command->row = 29;
  command->col = 11;
  command->string = self->strings[3];
  CommandList_add_command(screen->commands, command);
  
  Config_add_screen(self, screen);
  Config_allocate_rows(self);

  return true;
}

//------------------------------------------------------------------------------

Control* Control_new(void) {
  Control* self = (Control*) calloc(1, sizeof(Sample));
  self->pin = NULL;
  self->mode = MODE_MANUAL;
  self->asserted = false;
  self->screens = (uint8_t *) calloc(1, sizeof(uint8_t*));
  self->num_screens = 0;
  return self;
}

//------------------------------------------------------------------------------

void Control_free(Control* self) {
  free(self->screens);
  free(self);
}

//------------------------------------------------------------------------------

void Control_add_screen(Control* self, uint8_t index) {
  self->screens = (uint8_t*) realloc(self->screens, (self->num_screens+1)*sizeof(uint8_t));
  self->screens[self->num_screens] = index;
  self->num_screens++;
}

//------------------------------------------------------------------------------

Screen* Screen_new(void) {
  Screen* self = (Screen*) calloc(1, sizeof(Screen));

  self->mode = MODE_MANUAL;
  self->enabled = false;
  self->timeout = 0;

  self->controls = (Control**) calloc(1, sizeof(Control**));
  self->num_controls = 0;
  
  self->samples = (Sample**) calloc(1, sizeof(Sample**));
  self->num_samples = 0;
  
  self->commands = CommandList_new(self);

  self->rows = (uint8_t**) calloc(SCREEN_ROWS, sizeof(uint8_t*));
  
  return self;
}

//------------------------------------------------------------------------------

Screen* Screen_add_control(Screen *self, Control* control) {
  self->controls =
    (Control**) realloc(self->controls, (self->num_controls+1)*sizeof(Control**));

  self->controls[self->num_controls] = control;
  self->num_controls++;
  return self;  
}

//------------------------------------------------------------------------------

Sample* Screen_add_sample(Screen *self, Sample* sample) {
  self->samples = (Sample**) realloc(self->samples, (self->num_samples+1)*sizeof(Sample**));
  self->samples[self->num_samples] = sample;
  self->num_samples++;
  return sample;
}

//------------------------------------------------------------------------------

void Screen_free(Screen* self) {
  free(self->controls);
  
  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample_free(self->samples[i]);
  }
  free(self->samples);  

  CommandList_free(self->commands);
  
  free(self);
}

//------------------------------------------------------------------------------

Sample* Sample_new(Screen *screen) {
  Sample* self = (Sample*) calloc(1, sizeof(Sample));
  self->screen = screen;
  self->pins = (Pin**) calloc(1, sizeof(Pin**));
  self->num_pins = 0;
  self->value = 0;
  
  self->command_lists = (CommandList**) calloc(1, sizeof(CommandList**));
  self->num_command_lists = 0;

  return self;
}

//------------------------------------------------------------------------------

Pin* Sample_add_pin(Sample* self, Pin* pin) {
  self->pins = (Pin**) realloc(self->pins, (self->num_pins+1)*sizeof(Pin**));
  self->pins[self->num_pins] = pin;
  self->num_pins++;
  return pin;
}

//------------------------------------------------------------------------------

CommandList* Sample_add_commands(Sample* self, CommandList* commands) {
  self->command_lists =
    (CommandList**) realloc(self->command_lists,
                            (self->num_command_lists+1)*sizeof(CommandList**));
  self->command_lists[self->num_command_lists] = commands;
  self->num_command_lists++;
  return commands;
}

//------------------------------------------------------------------------------

void Sample_free(Sample* self) {
  for(uint8_t i=0; i<self->num_command_lists; i++) {
    CommandList_free(self->command_lists[i]);
  }
  free(self->pins);
  free(self->command_lists);  
};

//------------------------------------------------------------------------------

Command* Command_new(Screen* screen) {
  Command* self = (Command*) calloc(1, sizeof(Command));
  self->screen = screen;
  self->action = ACTION_NONE;
  self->row = 0;
  self->col = 0;
  self->len = 0;
  self->string = NULL;
  return self;
}

//------------------------------------------------------------------------------

void Command_set_string(Command* self, char *string) {
  self->string = string;
  self->len = strlen(string);
}

//------------------------------------------------------------------------------

bool Command_equals(Command* self, Command* command) {
  return
    (self->action == command->action) &&
    (self->row == command->row) &&
    (self->col == command->col) &&
    (self->len == command->len) &&
    (self->string == command->string);
}

//------------------------------------------------------------------------------

void Command_free(Command* self) {
  free(self);
}

//------------------------------------------------------------------------------

CommandList* CommandList_new(Screen* screen) {
  CommandList* self = (CommandList*) calloc(1, sizeof(CommandList));
  self->screen = screen;
  self->commands = (Command**) calloc(1, sizeof(Command**));
  self->num_commands = 0;
  return self;
}

//------------------------------------------------------------------------------

Command* CommandList_add_command(CommandList *self, Command* command) {

  self->commands =
    (Command**) realloc(self->commands, (self->num_commands+1)*sizeof(Command**));

  self->commands[self->num_commands] = command;
  self->num_commands++;
  return command;
}

//------------------------------------------------------------------------------

void CommandList_free(CommandList* self) {

  for(uint8_t i=0; i<self->num_commands; i++) {
    if(self->commands != NULL) {
      Command_free(self->commands[i]);
    }
  }
  free(self->commands);
  free(self);
}

//------------------------------------------------------------------------------

Pin *Pin_new(volatile Config* c, uint8_t port, uint8_t pos) {
  Pin* self = (Pin*) calloc(1, sizeof(Pin));
  self->port = c->ports[port];
  self->pos = pos;
  self->edge[0] = 1;
  self->edge[1] = 1;
  return self;
}

//------------------------------------------------------------------------------

void Pin_free(Pin* self) {
  free(self);
}

//------------------------------------------------------------------------------
// functions to read datastructures from binary format
//------------------------------------------------------------------------------

static bool Config_peek_magic(FILE* in) {
  char c;
  if(!(((uint8_t)(c = fgetc(in))) == CONFIG_MAGIC[0])) {
    ungetc(c, in);
    return false;
  }

  if(!(((uint8_t)(c = fgetc(in))) == CONFIG_MAGIC[1])) {
    ungetc(c, in);
    return false;
  }
  return true;
}

static void Config_read_timeout(volatile Config* self, FILE* in) {
  self->timeout = fgetc(in);
}

static void Config_read_strings(volatile Config* self, FILE* in) {
  uint8_t num_strings = fgetc(in);  
  uint8_t len;
  char * string;
  
  for(uint8_t i=0; i<num_strings; i++) {
    len = fgetc(in);

    string = (char *) calloc(len+1, sizeof(char));
    fread(string, sizeof(char), len, in);
    
    Config_add_string(self, string);
  }
}

static void Config_read_controls(volatile Config* self, FILE* in) {
  uint8_t len  = fgetc(in);
  for(uint8_t i=0; i<len; i++) {
    Control_read(Config_add_control(self, Control_new()), in);
  }
}

static void Config_read_screens(volatile Config* self, FILE* in) {
  uint8_t len  = fgetc(in);
  for(uint8_t i=0; i<len; i++) {
    Screen_read(Config_add_screen(self, Screen_new()), in);
  }
}

bool Config_read(volatile Config *self, FILE *in) {

  if(Config_peek_magic(in)) {
    Config_read_timeout(self, in);
    Config_read_strings(self, in);
    Config_read_controls(self, in);
    Config_read_screens(self, in);
    Config_assign_controls_to_screens(self);
    Config_allocate_rows(self);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------

void Control_read(Control* self, FILE* in) {
  self->pin = config->pins[fgetc(in)];
  self->mode = fgetc(in);
  uint8_t num_screens = fgetc(in);
  for(uint8_t i=0; i<num_screens; i++) {
    Control_add_screen(self, fgetc(in));
  }
}

//------------------------------------------------------------------------------

void Screen_read(Screen* self, FILE* in) {
  self->mode = fgetc(in);

  CommandList_read(self->commands, in);

  uint8_t num_samples = fgetc(in);
  for(uint8_t i=0; i<num_samples; i++) {
    Sample_read(Screen_add_sample(self, Sample_new(self)), in);
  }
}

//------------------------------------------------------------------------------

void CommandList_read(CommandList *self, FILE* in) {
  uint8_t num_commands = fgetc(in);
  Command *command;
  
  for(uint8_t i=0; i<num_commands; i++) {
    command = Command_new(self->screen);
    Command_read(command, in);
    CommandList_add_command(self, command);
  }
}

//------------------------------------------------------------------------------

void Command_read(Command* self, FILE* in) {
  self->action = fgetc(in);
  self->row = fgetc(in);
  self->col = fgetc(in);
  self->len = fgetc(in);
  uint8_t index = fgetc(in);
  
  if(self->action == ACTION_WRITE) {
    Command_set_string(self, config->strings[index]);
  }
}

//------------------------------------------------------------------------------

void Sample_read(Sample* self, FILE* in) {
  uint8_t num_pins = fgetc(in);
  CommandList* commands;
  for(uint8_t i=0; i<num_pins; i++) {
    Sample_add_pin(self, config->pins[fgetc(in)]);
  }

  uint8_t num_command_lists = 1<<(self->num_pins);
  for(uint8_t i=0; i<num_command_lists; i++) {
    commands = CommandList_new(self->screen);
    CommandList_read(commands, in);
    Sample_add_commands(self, commands);
  }
}

//------------------------------------------------------------------------------
