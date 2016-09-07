#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "strings.h"
#include "config.h"
#include "parser.h"

#define TIMEOUT 0x01
#define SAMPLE  0x02
#define WHEN    0x03
#define WRITE   0x04
#define CLEAR   0x05
#define SCREEN  0x07
#define CONTROL 0x08

volatile Config* config;
uint16_t written = 0;

static int fputcc(int ch, FILE* fp) {
  fputc(ch, fp);
  written++;
  return ch;
}

//------------------------------------------------------------------------------
// Utitlity functions for parsing
//------------------------------------------------------------------------------

static bool parseKeyword(char* word, int *keyword) {
  return
    ((strncmp(word, "sample",   6) == 0) && (*keyword = SAMPLE)) ||
    ((strncmp(word, "when",     4) == 0) && (*keyword = WHEN)) ||
    ((strncmp(word, "write",    5) == 0) && (*keyword = WRITE)) ||
    ((strncmp(word, "clear",    5) == 0) && (*keyword = CLEAR)) ||
    ((strncmp(word, "timeout",  7) == 0) && (*keyword = TIMEOUT)) ||
    ((strncmp(word, "screen",   6) == 0) && (*keyword = SCREEN)) ||
    ((strncmp(word, "control",  7) == 0) && (*keyword = CONTROL));
}

//------------------------------------------------------------------------------

static bool parseInt(char* word, int base, uint8_t *value) {
  char *failed;
  *value = strtol(word, &failed, base);
  return (word != failed);
}

//------------------------------------------------------------------------------

static bool parseString(StringList* words, int *i, char **str) {
  if((*i) < words->size) {
    (*str) = words->strings[(*i)];
    if((*str)[0] == '"') {
      (*str)++;
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

static bool parseMode(char* word, uint8_t *mode) {
  return
    ((strncmp(word, "manual", 6) == 0) && (*mode = MODE_MANUAL)) ||
    ((strncmp(word, "notify", 6) == 0) && (*mode = MODE_NOTIFY));
}

//------------------------------------------------------------------------------
// Functions for parsing datatstructures from text format
//------------------------------------------------------------------------------

bool Config_parse(volatile Config* self, FILE* in) {  
  bool result = true;
  char buf[0x10000];
  char *word;
  int keyword;
  uint8_t timeout;
  Screen* screen = NULL;
  
  fread(buf, sizeof(char), sizeof(buf), in);
  
  StringList* words = StringList_new();
  StringList_append_quoted(words, buf, "\n\t ");
  int i = 0;

  while(i<words->size) {
    word = StringList_get(words, i);
    
    if(parseKeyword(word, &keyword)) {
      i++;

      if(keyword == CONTROL) {
        if(screen != NULL) {
          fprintf(stderr, "error: control specified after first screen\n");
          break;
        }
        result = Control_parse(Config_add_control(self, Control_new()), words, &i);
        if(!result) break;
      }
      
      if(keyword == SCREEN) {
        screen = Screen_new();
        result = Screen_parse(Config_add_screen(self, screen), words, &i);        
        if(!result) break;
      }
            
      else if(keyword == SAMPLE) {
        if(screen == NULL) {
          fprintf(stderr, "error: sample specified before any screens\n");
          break;
        }
        result = Sample_parse(Screen_add_sample(screen, Sample_new()), words, &i);
        if(!result) break;
      }
      else if(keyword == WRITE || keyword == CLEAR) {
        fprintf(stderr, "error: command specified before any screens\n");
        break;          
      }
      else if(keyword == TIMEOUT) {
        if(parseInt(StringList_get(words, i), 0,  &timeout)) {
          self->timeout = timeout;
        }
        i++;
      }
    }
  }
  Config_assign_controls_to_screens(self);
  Config_allocate_rows(self);
  return result;
}

//------------------------------------------------------------------------------

bool Control_parse(Control* self, StringList* words, int *i) {
  uint8_t pin;
  uint8_t mode;
  uint8_t index;
  
  if(!parseInt(StringList_get(words, *i), 0, &pin)) {
    fprintf(stderr, "error: control: no control pin specified\n");
    return false;
  }
  self->pin = config->control[pin];
  (*i)++;

  if(parseMode(StringList_get(words, *i), &mode)) {
     self->mode = mode;
     (*i)++;
  }
  while((*i)<words->size && parseInt(StringList_get(words, *i), 0, &index)) {
    Control_add_screen(self, index);
    (*i)++;
  }
  return true;
}

//------------------------------------------------------------------------------

bool Screen_parse(Screen* self, StringList* words, int *i) {
  uint8_t mode;
  int keyword;
  Command* command;

  if(parseMode(StringList_get(words, *i), &mode)) {
     self->mode = mode;
     (*i)++;
  }

  while((*i)<words->size && parseKeyword(StringList_get(words, *i), &keyword)) {
    if(keyword == WRITE || keyword == CLEAR) {
      (*i)++;
      command = Command_new();
      Command_parse(command, keyword, words, i);     
      CommandList_add_command(self->commands, command);
    }
    else {
      break;
    }
  }
  return true;
}

//------------------------------------------------------------------------------

bool Sample_parse(Sample* self, StringList* words, int *i) {

  uint8_t pin;  
  
  while(parseInt(StringList_get(words, *i), 0, &pin)) {
    Sample_add_pin(self, config->input[pin]);
    (*i)++;
  }

  for(int k=0; k<(1<<self->num_pins); k++) {
    Sample_add_commands(self, CommandList_new());
  }

  int keyword;
  Command *command;
  CommandList *commands = self->command_lists[0];
  uint8_t index;
  
  while((*i)<words->size && parseKeyword(StringList_get(words, *i), &keyword)) {

    if(keyword == WHEN) {
      (*i)++;

      if(!parseInt(StringList_get(words, *i), 2, &index)) {
        fprintf(stderr, "WHEN without condition\n");
        goto error;
      }
      (*i)++;

      if(index >= self->num_command_lists) {
        fprintf(stderr, "condition out of range\n");
        goto error;
      }      
      commands = self->command_lists[index];
    }
    else if(keyword == WRITE || keyword == CLEAR) {
      (*i)++;
      command = Command_new();
      Command_parse(command, keyword, words, i);
      CommandList_add_command(commands, command);
    }
    else {
      goto done;
    }
  }
  done:
    return true;

  error:
    return false;
}

//------------------------------------------------------------------------------

bool Command_parse(Command *self, int keyword, StringList* words, int *i) {

  uint8_t value;
  char *string = (char*) calloc(4096, sizeof(char));
  char *ptr = string;
  
  uint8_t index;
  
  self->action = (keyword == WRITE) ?
    ACTION_WRITE : ((keyword == CLEAR) ? ACTION_CLEAR : ACTION_NONE);  
  
  if(parseInt(StringList_get(words, *i), 0, &value)) {
    self->row = value;
    (*i)++;
  }
  
  if(parseInt(StringList_get(words, *i), 0, &value)) {
    self->col = value;
    (*i)++;
  }
  if(parseString(words, i, &string)) {    
    if(Config_has_string(config, string, &index)) {
      Command_set_string(self, config->strings[index]);
    }
    else {
      Command_set_string(self, Config_add_string(config, string));
    }
    (*i)++;
  }
  else if(parseInt(StringList_get(words, *i), 0, &value)) {
    self->len = value;
    (*i)++;
  }
  free(ptr);
  return true;
}

//------------------------------------------------------------------------------
// Function for writing out datastructures in text format
//------------------------------------------------------------------------------

void Config_print(volatile Config* self, FILE* out) {

  fprintf(out, "timeout %d\n", self->timeout);

  for(int i=0; i<self->num_controls; i++) {
    Control_print(self->controls[i], out);
  }

  for(int i=0; i<self->num_screens; i++) {
    Screen_print(self->screens[i], out);
  }
}

//------------------------------------------------------------------------------

uint8_t Config_index_of_input(volatile Config* self, Pin* pin) {
  for(int i=0; i<INPUT_PINS; i++) {
    if(self->input[i] == pin) {
      return i;
    }
  }
  return 0xff;
}

//------------------------------------------------------------------------------

uint8_t Config_index_of_control(volatile Config* self, Pin* pin) {
  for(int i=0; i<CONTROL_PINS; i++) {
    if(self->control[i] == pin) {
      return i;
    }
  }
  return 0xff;
}

//------------------------------------------------------------------------------

uint8_t Config_index_of_screen(volatile Config* self, Screen* screen) {
  for(int i=0; i<self->num_screens; i++) {
    if(self->screens[i] == screen) {
      return i;
    }
  }
  return 0xff;
}

//------------------------------------------------------------------------------

uint8_t Config_index_of_pin(volatile Config* self, Pin* pin) {
  uint8_t index  = Config_index_of_input(self, pin);
  if(index != 0xff) {
    return index;
  }
  return Config_index_of_control(self, pin);
}

//------------------------------------------------------------------------------

uint8_t Config_index_of_string(volatile Config* self, char* string) {
  for(int i=0; i<self->num_strings; i++) {
    if(self->strings[i] == string) {
      return i;
    }
  }
  return 0xff;
}

//------------------------------------------------------------------------------

void Control_print(Control* self, FILE* out) {
  fprintf(out, "control ");
  Pin_print(self->pin, out);

  if(self->mode == MODE_MANUAL) {
    fprintf(out, "manual ");
  }

  if(self->mode == MODE_NOTIFY) {
    fprintf(out, "notify ");
  }

  for(int i=0; i<self->num_screens; i++) {
    fprintf(out, "%d ", self->screens[i]);
  }
  
  fprintf(out, "\n");
}

//------------------------------------------------------------------------------

void Screen_print(Screen* self, FILE* out) {
  fprintf(out, "screen ");

  if(self->mode == MODE_MANUAL) {
    fprintf(out, "manual ");
  }

  if(self->mode == MODE_NOTIFY) {
    fprintf(out, "notify ");
  }

  fprintf(out, "\n");

  CommandList_print(self->commands, out);
  
  for(int i=0; i<self->num_samples; i++) {
    Sample_print(self->samples[i], out);
  }
}

//------------------------------------------------------------------------------

static void binary(uint8_t value, char **result) {
  uint8_t pos = 0;
  
  for(int i=7; i>=0; i--) {
    (*result)[pos++] = (value & (1<<i)) ? '1' : '0';
  }
}

//------------------------------------------------------------------------------

void Sample_print(Sample* self, FILE* out) {

  fprintf(out, "sample ");

  for(int i=0; i<self->num_pins; i++) {
    Pin_print(self->pins[i], out);
  }
  
  fprintf(out, "\n");

  char *condition = (char*) calloc(9, sizeof(char));
  
  for(int i=0; i<self->num_command_lists; i++) {
    binary(i, &condition);
    fprintf(out, "when %s\n", condition);
    CommandList_print(self->command_lists[i], out);
  }
  
  free(condition);
}

//------------------------------------------------------------------------------

void CommandList_print(CommandList *self, FILE* out) {
  for(int i=0; i<self->num_commands; i++) {
    Command_print(self->commands[i], out);
  }
}

//------------------------------------------------------------------------------

static void escape(char *input, char **output) {
  char c;
  int pos = 0;
  
  for(int i=0; i<strlen(input); i++) {
    c = input[i];

    if(c == '"' || c == '\\') {
      (*output)[pos++] = '\\';
    }
    (*output)[pos++] = c;
  }
}

void Command_print(Command *self, FILE* out) {

  if(self->action == ACTION_WRITE) {
    fprintf(out, "write ");
  }

  if(self->action == ACTION_CLEAR) {
    fprintf(out, "clear ");
  }

  char *escaped = (char*) calloc(strlen(self->string)*2+1, sizeof(char));
  escape(self->string, &escaped);
                                 
  fprintf(out, "%d %d \"%s\"\n", self->row, self->col, escaped);

  free(escaped);
}

//------------------------------------------------------------------------------
   
void Pin_print(Pin* self, FILE* out) {
  fprintf(out, "%d ", Config_index_of_pin(config, self));
}

//------------------------------------------------------------------------------
// Functions to write datastructures in binary format
//------------------------------------------------------------------------------

static void Config_write_magic(FILE* out) {
  fputcc(CONFIG_MAGIC[0], out);
  fputcc(CONFIG_MAGIC[1], out);
}

static void Config_write_timeout(volatile Config* self, FILE* out) {
  fputcc(self->timeout, out);
}

static void Config_write_strings(volatile Config* self, FILE* out) {
  fputcc(self->num_strings, out);
  for(uint8_t i=0; i<self->num_strings; i++) {
    fputcc(strlen(self->strings[i]), out);
    fputs(self->strings[i], out);
    written += strlen(self->strings[i]);
  }
}

static void Config_write_controls(volatile Config* self, FILE* out) {
 fputcc(self->num_controls, out);
  for(uint8_t i=0; i<self->num_controls; i++) {
    Control_write(self->controls[i], out);
  }
}

static void Config_write_screens(volatile Config* self, FILE* out) {
 fputcc(self->num_screens, out);
  for(uint8_t i=0; i<self->num_screens; i++) {
    Screen_write(self->screens[i], out);
  }
}

void Config_write(volatile Config* self, FILE* out) {
  Config_write_magic(out);
  Config_write_timeout(self, out);
  Config_write_strings(self, out);
  Config_write_controls(self, out);
  Config_write_screens(self, out);
}

//------------------------------------------------------------------------------

void Control_write(Control* self, FILE* out) {
  Pin_write(self->pin, out);
  fputcc(self->mode, out);

  fputcc(self->num_screens, out);  
  for(uint8_t i=0; i<self->num_screens; i++) {
    fputcc(self->screens[i], out);
  }         
}

//------------------------------------------------------------------------------

void Screen_write(Screen* self, FILE* out) {
  fputcc(self->mode, out);

  CommandList_write(self->commands, out);

  fputcc(self->num_samples, out);
  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample_write(self->samples[i], out);
  }
}

//------------------------------------------------------------------------------

void Sample_write(Sample* self, FILE* out) {

  fputcc(self->num_pins, out);
  for(uint8_t i=0; i<self->num_pins; i++) {
    Pin_write(self->pins[i], out);
  }
  for(uint8_t i=0; i<self->num_command_lists; i++) {
    CommandList_write(self->command_lists[i], out);
  }
}

//------------------------------------------------------------------------------
  
void Pin_write(Pin* self, FILE* out) {
  fputcc(Config_index_of_pin(config, self), out);
}

//------------------------------------------------------------------------------

void CommandList_write(CommandList *self, FILE* out) {
  fputcc(self->num_commands, out);
  for(uint8_t i=0; i<self->num_commands; i++) {
    Command_write(self->commands[i], out);
  }
}

//------------------------------------------------------------------------------

void Command_write(Command *self, FILE* out) {
  fputcc(self->action, out);
  fputcc(self->row, out);  
  fputcc(self->col, out);
  fputcc(self->len, out);
  fputcc(Config_index_of_string(config, self->string), out);    
}

//------------------------------------------------------------------------------
// functions for calculating the required memory footprint
//------------------------------------------------------------------------------

uint16_t Config_get_footprint(volatile Config* self) {
  uint16_t fp = 0;

  fp += 2;                // the pointer to the config itself
  fp += 3*2;              // the pointers to the ports
  fp += INPUT_PINS*2;     // the pointers to the pins
  fp += INPUT_PINS*3;     // the actual pins

  fp += 1;                // timeout 
  
  fp += 1;                     // num_strings
  fp += self->num_strings * 2; // the pointers to the strings;

  // the strings themselves
  for(uint8_t i=0; i<self->num_strings; i++) {
    fp += strlen(self->strings[i])+1;
  }

  fp += SCREEN_ROWS * 2;  // the pointers to the rows

  // the allocated rows themselves
  for(uint8_t i=0; i<SCREEN_ROWS; i++) {
    fp += (self->rows[i] != NULL) ? SCREEN_COLUMNS : 0;
  }

  fp += 64*8;  // the font data
  fp += 1+1+2; // globals scanline, enabled, timeout
  fp += 2*2;   // global pin state arrays output_enable, output_request 

  return fp;
}

//------------------------------------------------------------------------------

uint16_t Sample_get_footprint(Sample* self) {
  uint16_t fp = 0;
  fp += 1; // num_pins
  fp += self->num_pins * 2;

  fp += 1; // num_commands;
  fp += self->num_command_lists * 2; // pointers to the CommandLists

  // the CommandLists themselves
  for(uint8_t i=0; i<self->num_command_lists; i++) {
    fp += CommandList_get_sparse_footprint(self->command_lists[i]);
  }
  
  return fp;
}

//------------------------------------------------------------------------------

uint16_t CommandList_get_footprint(CommandList* self) {
  uint16_t fp = 0;

  fp += CommandList_get_sparse_footprint(self);

  // the commands themselves
  for(uint8_t i=0; i<self->num_commands; i++) {
    fp += Command_get_footprint(self->commands[i]);
  }
  return fp;
}

//------------------------------------------------------------------------------
 
uint16_t CommandList_get_sparse_footprint(CommandList* self) {
  uint16_t fp = 0;
  fp += 1;                       // num_commands;
  fp += self->num_commands * 2;  // the pointers to the commands
  return fp;
}
 
//------------------------------------------------------------------------------

uint16_t Command_get_footprint(Command* self) {
  uint16_t fp = 0;
  fp += 1+1+1+2+2; // action, col, row, len, string pointer
  return fp;
}

//------------------------------------------------------------------------------

