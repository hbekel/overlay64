#include <stdlib.h>
#include <string.h>

#include "config.h"

uint8_t CONFIG_MAGIC[2] = { 'O', 'V' };

static uint8_t A = 0;
static uint8_t C = 1;

//------------------------------------------------------------------------------

volatile Config* Config_new(void) {
  return Config_new_with_ports(NULL, NULL);
}

//------------------------------------------------------------------------------

volatile Config* Config_new_with_ports(uint8_t volatile *port0, uint8_t volatile *port1) {

  volatile Config* self = (Config*) calloc(1, sizeof(Config));

  self->ports[0] = port0;
  self->ports[1] = port1;

  self->timeout = 2*50; 

  self->immediateCommands = CommandList_new();
  self->commands = CommandList_new();
  self->samples = (Sample**) calloc(1, sizeof(Sample**));
  self->sample = 0;
  
  uint8_t i = 0;
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
  
  self->strings = (char**) NULL;
  self->num_strings = 0;

  self->rows = (uint8_t**) calloc(SCREEN_ROWS, sizeof(uint8_t*));

  return self;
}

//------------------------------------------------------------------------------

void Config_free(volatile Config* self) {

  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample_free(self->samples[i]);
  }
  free(self->samples);
  
  for(uint8_t i=0; i<sizeof(self->pins)/sizeof(Pin*); i++) {
    Pin_free(self->pins[i]);
  }
  CommandList_free(self->immediateCommands);
  CommandList_free(self->commands);

  for(uint8_t i=0; i<SCREEN_ROWS; i++) {
    if(self->rows[i] != NULL) {
      free(self->rows[i]);
    }
  }

  free((void*)self);
}

//------------------------------------------------------------------------------

Sample* Config_add_sample(volatile Config *self, Sample* sample) {
  self->samples = (Sample**) realloc(self->samples, (self->num_samples+1)*sizeof(Sample**));
  self->samples[self->num_samples] = sample;
  self->num_samples++;
  return sample;
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

bool Config_has_command(volatile Config *self, Command* command, uint8_t *index) {
  for(uint8_t i=0; i<self->commands->num_commands; i++) {
    if(Command_equals(self->commands->commands[i], command)) {
      *index = i;
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

Command* Config_add_command(volatile Config *self, Command* command) {
  uint8_t index = 0;
  if(Config_has_command(self, command, &index)) {
    return self->commands->commands[index];
  }
  CommandList_add_new_command(self->commands, command);
  return command;
}

//------------------------------------------------------------------------------

void Config_allocate_rows(volatile Config *self) {

  Command *command;
  
  for(uint8_t i=0; i<self->commands->num_commands; i++) {
    command = self->commands->commands[i];
    if(self->rows[command->row] == NULL) {
      self->rows[command->row] = (uint8_t*) calloc(SCREEN_COLUMNS, sizeof(uint8_t));
    }
  }
}

//------------------------------------------------------------------------------

Sample* Sample_new(void) {
  Sample* self = (Sample*) calloc(1, sizeof(Sample));
  self->pins = (Pin**) calloc(1, sizeof(Pin**));
  self->num_pins = 0;
  
  self->commands = (CommandList**) calloc(1, sizeof(CommandList**));
  self->num_commands = 0;

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
  self->commands =
    (CommandList**) realloc(self->commands, (self->num_commands+1)*sizeof(CommandList**));
  self->commands[self->num_commands] = commands;
  self->num_commands++;
  return commands;
}

//------------------------------------------------------------------------------

void Sample_free(Sample* self) {
  for(uint8_t i=0; i<self->num_commands; i++) {
    CommandList_free(self->commands[i]);
  }
  free(self->pins);
  free(self->commands);  
};

//------------------------------------------------------------------------------

Command* Command_new(void) {
  Command* self = (Command*) calloc(1, sizeof(Command));
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

CommandList* CommandList_new(void) {
  CommandList* self = (CommandList*) calloc(1, sizeof(CommandList));
  self->commands = (Command**) calloc(1, sizeof(Command**));
  self->num_commands = 0;
  return self;
}

//------------------------------------------------------------------------------

Command* CommandList_add_new_command(CommandList *self, Command* command) {

  self->commands =
    (Command**) realloc(self->commands, (self->num_commands+1)*sizeof(Command**));

  self->commands[self->num_commands] = command;
  self->num_commands++;
  return command;
}

//------------------------------------------------------------------------------

Command* CommandList_add_command(CommandList *self, Command* command) {

  uint8_t index;
  
  self->commands =
    (Command**) realloc(self->commands, (self->num_commands+1)*sizeof(Command**));

  if(Config_has_command(config, command, &index)) {
    self->commands[self->num_commands] = config->commands->commands[index];

    if(command != self->commands[self->num_commands]) {
      Command_free(command);
    }
    command = config->commands->commands[index];
  }
  else {
    self->commands[self->num_commands] = Config_add_command(config, command);
  }
  
  self->num_commands++;
  return command;
}

//------------------------------------------------------------------------------

void CommandList_free(CommandList* self) {

  if(self == config->commands) {
    for(uint8_t i=0; i<self->num_commands; i++) {
      if(self->commands != NULL) {
        Command_free(self->commands[i]);
      }
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

static void Config_read_commands(volatile Config* self, FILE* in) {
  CommandList_read(self->commands, in);
  CommandList_read_indexed(self->immediateCommands, in);
}

static void Config_read_samples(volatile Config* self, FILE* in) {
  uint8_t len  = fgetc(in);
  for(uint8_t i=0; i<len; i++) {
    Sample_read(Config_add_sample(self, Sample_new()), in);
  }
}

bool Config_read(volatile Config *self, FILE *in) {

  if(Config_peek_magic(in)) {
    Config_read_timeout(self, in);
    Config_read_strings(self, in);
    Config_read_commands(self, in);
    Config_read_samples(self, in);
    Config_allocate_rows(self);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------

void CommandList_read(CommandList *self, FILE* in) {
  uint8_t num_commands = fgetc(in);
  Command *command;
  
  for(uint8_t i=0; i<num_commands; i++) {
    command = Command_new();
    Command_read(command, in);
    CommandList_add_new_command(self, command);
  }
}

//------------------------------------------------------------------------------

void CommandList_read_indexed(CommandList *self, FILE* in) {
  uint8_t num_commands = fgetc(in);

  for(uint8_t i=0; i<num_commands; i++) {
    CommandList_add_command(self, config->commands->commands[fgetc(in)]);
  }
}

//------------------------------------------------------------------------------

void Command_read(Command* self, FILE* in) {
  self->action = fgetc(in);
  self->row = fgetc(in);
  self->col = fgetc(in);
  self->len = fgetc(in);

  if(self->action == ACTION_WRITE) {
    Command_set_string(self, config->strings[fgetc(in)]);
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
    commands = CommandList_new();
    CommandList_read_indexed(commands, in);
    Sample_add_commands(self, commands);
  }
}

//------------------------------------------------------------------------------
