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
const char *ws = " \t";

static int fputcc(int ch, FILE* fp) {
  fputc(ch, fp);
  written++;
  return ch;
}

//-----------------------------------------------------------------------------
// Utitlity functions for parsing
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

static bool parseInt(char* word, int base, uint8_t *value) {
  char *failed;
  *value = strtol(word, &failed, base);
  return (word != failed);
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

static bool parseMode(char* word, uint8_t *mode) {
  return
    ((strncmp(word, "manual", 6) == 0) && (*mode = MODE_MANUAL)) ||
    ((strncmp(word, "notify", 6) == 0) && (*mode = MODE_NOTIFY)) ||
    ((strncmp(word, "always", 6) == 0) && (*mode = MODE_ALWAYS));
}

//-----------------------------------------------------------------------------

static bool string_is_empty(char *str) {
  for(int i=0; i<strlen(str); i++) {
    if(str[i] != ' ') return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

static bool isSymbolName(char *name) {
  return strspn(name, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_")
    == strlen(name);
}

//-----------------------------------------------------------------------------
// Functions for parsing datatstructures from text format
//-----------------------------------------------------------------------------

bool Config_parse(volatile Config* self, FILE* in) {  

  bool result = false;
  char* buffer = (char*) calloc(4096, sizeof(char));
  char *line;
  char *word;
  char *name;
  char *value;
  char *equals;
  char *comment;
  int keyword;
  int trailing = 0; 
  int i = 0;
  uint8_t timeout;
  Screen* screen = NULL;
  StringList* words = StringList_new();
  
  fseek(in, 0, SEEK_SET);

  int pos = 0;
  while(fgets(buffer, 4095, in) != NULL) {
    line = buffer;
    pos++;

    // skip leading whitespace
    line += strspn(line, ws);

    // skip empty lines and comments
    if(line[0] == '\n' || line[0] == '\r' || line[0] == '#') continue;

    // remove comment at the end of the line
    if((comment = strstr(line, "#")) != NULL) {
      int quotes = 0;
      for(int i=0; i<comment-line; i++) {
        if(line[i] == '"') quotes++;        
      }
      if(quotes % 2 == 0) {
        comment[0] = '\0';
      }
    }

    // discard newlines
    if(line[strlen(line)-1] == '\n') {
      line[strlen(line)-1] = '\0';
    }

    if(line[strlen(line)-1] == '\r') {
      line[strlen(line)-1] = '\0';
    }

    // check if this command is a definition
    if((strstr(line, "=")) != NULL) {
      name = strdup(line);
      equals = strstr(name, "=");
      equals[0] = '\0';

      if((trailing = strcspn(name, ws)) > 0) {
        name[trailing] = '\0';
      }
      
      if(!isSymbolName(name) || parseKeyword(name, &keyword)) {
        goto not_a_symbol;
      }
      
      value = equals+1;
      value = trim(value);
      
      if(StringList_has_definition(name)) {
        fprintf(stderr, "error: line %d: '%s': symbol already defined\n", pos, name);
        goto done;
      }
      StringList_add_definition(name, value);
    }
    else {
      StringList_append_quoted(words, line, "\n\t ");
    }
  }

 not_a_symbol:
  
  while(i<words->size) {
    word = StringList_get(words, i);
    
    if(parseKeyword(word, &keyword)) {
      i++;

      if(keyword == CONTROL) {
        if(!Control_parse(Config_add_control(self, Control_new()), words, &i))
          goto done;
      }
      
      if(keyword == SCREEN) {
        screen = Screen_new();
        if(!Screen_parse(Config_add_screen(self, screen), words, &i))
          goto done;
      }
            
      else if(keyword == SAMPLE) {
        if(screen == NULL) {
          fprintf(stderr, "error: line %d: sample specified before any screens\n", pos);
          goto done;
        }
        if(!Sample_parse(Screen_add_sample(screen, Sample_new(screen)), words, &i))
          goto done;
      }
      else if(keyword == WRITE || keyword == CLEAR) {
        fprintf(stderr, "error: line %d: command specified before any screens\n", pos);
        goto done;     
      }
      else if(keyword == TIMEOUT) {
        if(parseInt(StringList_get(words, i), 0,  &timeout)) {
          self->timeout = timeout;
        }
        i++;
      }
    }
    else {
      result = false;
      fprintf(stderr, "error: line %d: '%s': unknown keyword\n", pos, word);
      goto done;
    }
  }
  Config_assign_controls_to_screens(self);
  Config_allocate_rows(self);
  result = true;
  
 done:
  return result;
}

//-----------------------------------------------------------------------------

bool Control_parse(Control* self, StringList* words, int *i) {
  uint8_t pin;
  uint8_t mode;
  uint8_t index;
  
  if(!parseInt(StringList_get(words, *i), 0, &pin)) {
    fprintf(stderr, "error: control: no control pin specified\n");
    return false;
  }
  self->pin = config->pins[pin];
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

//-----------------------------------------------------------------------------

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
      command = Command_new(self);
      Command_parse(command, keyword, words, i);     
      CommandList_add_command(self->commands, command);
    }
    else {
      break;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------

bool Sample_parse(Sample* self, StringList* words, int *i) {

  uint8_t pin;  
  
  while(parseInt(StringList_get(words, *i), 0, &pin)) {
    Sample_add_pin(self, config->pins[pin]);
    (*i)++;
  }

  for(int k=0; k<(1<<self->num_pins); k++) {
    Sample_add_commands(self, CommandList_new(self->screen));
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
      command = Command_new(self->screen);
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

//-----------------------------------------------------------------------------

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
    if(string_is_empty(string)) {
      self->action = ACTION_CLEAR;
      self->len = strlen(string);
    }
    else {
      if(Config_has_string(config, string, &index)) {
        Command_set_string(self, config->strings[index]);
      }
      else {
        Command_set_string(self, Config_add_string(config, string));
      }
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

//-----------------------------------------------------------------------------
// Function for writing out datastructures in text format
//-----------------------------------------------------------------------------

void Config_print(volatile Config* self, FILE* out) {

  fprintf(out, "timeout %d\n", self->timeout);

  for(int i=0; i<self->num_controls; i++) {
    Control_print(self->controls[i], out);
  }

  for(int i=0; i<self->num_screens; i++) {
    Screen_print(self->screens[i], out);
  }
}

//-----------------------------------------------------------------------------

uint8_t Config_index_of_pin(volatile Config* self, Pin* pin) {
  for(int i=0; i<NUM_PINS; i++) {
    if(self->pins[i] == pin) {
      return i;
    }
  }
  return 0xff;
}

//-----------------------------------------------------------------------------

uint8_t Config_index_of_screen(volatile Config* self, Screen* screen) {
  for(int i=0; i<self->num_screens; i++) {
    if(self->screens[i] == screen) {
      return i;
    }
  }
  return 0xff;
}

//-----------------------------------------------------------------------------

uint8_t Config_index_of_string(volatile Config* self, char* string) {
  for(int i=0; i<self->num_strings; i++) {
    if(self->strings[i] == string) {
      return i;
    }
  }
  return 0xff;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void Screen_print(Screen* self, FILE* out) {
  fprintf(out, "screen ");

  if(self->mode == MODE_MANUAL) {
    fprintf(out, "manual ");
  }

  if(self->mode == MODE_NOTIFY) {
    fprintf(out, "notify ");
  }

  if(self->mode == MODE_ALWAYS) {
    fprintf(out, "always ");
  }
  
  fprintf(out, "\n");

  CommandList_print(self->commands, out);
  
  for(int i=0; i<self->num_samples; i++) {
    Sample_print(self->samples[i], out);
  }
}

//-----------------------------------------------------------------------------

static void binary(uint8_t value, char **result) {
  uint8_t pos = 0;
  
  for(int i=7; i>=0; i--) {
    (*result)[pos++] = (value & (1<<i)) ? '1' : '0';
  }
}

//-----------------------------------------------------------------------------

void Sample_print(Sample* self, FILE* out) {

  fprintf(out, "sample ");

  for(int i=0; i<self->num_pins; i++) {
    Pin_print(self->pins[i], out);
  }
  
  fprintf(out, "\n");

  char *condition = (char*) calloc(9, sizeof(char));
  
  for(int i=0; i<self->num_command_lists; i++) {
    binary(i, &condition);
    if(self->command_lists[i]->num_commands) {
      fprintf(out, "when %s\n", condition);
      CommandList_print(self->command_lists[i], out);
    }
  }
  
  free(condition);
}

//-----------------------------------------------------------------------------

void CommandList_print(CommandList *self, FILE* out) {
  for(int i=0; i<self->num_commands; i++) {
    Command_print(self->commands[i], out);
  }
}

//-----------------------------------------------------------------------------

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

  if(self->action == ACTION_CLEAR) {
    fprintf(out, "clear %d %d %d\n", self->row, self->col, self->len);
  }
  
  if(self->action == ACTION_WRITE) {
    fprintf(out, "write ");

    char *escaped = (char*) calloc(strlen(self->string)*2+1, sizeof(char));
    escape(self->string, &escaped);
    
    fprintf(out, "%d %d \"%s\"\n", self->row, self->col, escaped);
    
    free(escaped);
  }
}

//-----------------------------------------------------------------------------
   
void Pin_print(Pin* self, FILE* out) {
  fprintf(out, "%d ", Config_index_of_pin(config, self));
}

//-----------------------------------------------------------------------------
// Functions to write datastructures in binary format
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void Control_write(Control* self, FILE* out) {
  Pin_write(self->pin, out);
  fputcc(self->mode, out);

  fputcc(self->num_screens, out);  
  for(uint8_t i=0; i<self->num_screens; i++) {
    fputcc(self->screens[i], out);
  }         
}

//-----------------------------------------------------------------------------

void Screen_write(Screen* self, FILE* out) {
  fputcc(self->mode, out);

  CommandList_write(self->commands, out);

  fputcc(self->num_samples, out);
  for(uint8_t i=0; i<self->num_samples; i++) {
    Sample_write(self->samples[i], out);
  }
}

//-----------------------------------------------------------------------------

void Sample_write(Sample* self, FILE* out) {

  fputcc(self->num_pins, out);
  for(uint8_t i=0; i<self->num_pins; i++) {
    Pin_write(self->pins[i], out);
  }
  for(uint8_t i=0; i<self->num_command_lists; i++) {
    CommandList_write(self->command_lists[i], out);
  }
}

//-----------------------------------------------------------------------------
  
void Pin_write(Pin* self, FILE* out) {
  fputcc(Config_index_of_pin(config, self), out);
}

//-----------------------------------------------------------------------------

void CommandList_write(CommandList *self, FILE* out) {
  fputcc(self->num_commands, out);
  for(uint8_t i=0; i<self->num_commands; i++) {
    Command_write(self->commands[i], out);
  }
}

//-----------------------------------------------------------------------------

void Command_write(Command *self, FILE* out) {
  fputcc(self->action, out);
  fputcc(self->row, out);  
  fputcc(self->col, out);
  fputcc(self->len, out);
  fputcc(Config_index_of_string(config, self->string), out);
}

//-----------------------------------------------------------------------------
// functions for calculating the required memory footprint
//-----------------------------------------------------------------------------

uint16_t Config_get_footprint(volatile Config* self) {
  uint16_t fp = 0;

  fp += 2;                // the pointer to the config itself
  fp += 3*2;              // the pointers to the ports

  fp += NUM_PINS*2;               // the pointers to the pins
  fp += NUM_PINS*sizeof(Pin);     // the actual pins

  fp += 1; // enabled
  fp += 1; // timeout 
  
  fp += 1;                     // num_strings
  fp += self->num_strings * 2; // the pointers to the strings;

  // the strings themselves
  for(uint8_t i=0; i<self->num_strings; i++) {
    fp += strlen(self->strings[i])+1;
  }

  fp += 1; // num_controls
  fp += self->num_controls*2; // control pointers
  for(uint8_t i=0; i<self->num_controls; i++) {
    fp += Control_get_footprint(self->controls[i]);
  }

  fp += 1; // num_screens;
  fp += self->num_screens*2; // screen pointers
  for(uint8_t i=0; i<self->num_screens; i++) {
    fp += Screen_get_footprint(self->screens[i]);
  }
  
  fp += SCREEN_ROWS * 2;  // the pointers to the rows

  fp += 64*8;  // the font data
  fp += 1+1+2; // global scanline

  return fp;
}

//-----------------------------------------------------------------------------

int Control_get_footprint(Control* self) {
  int fp = 2; // Pin pointer
  fp += 1; // mode;
  fp += 1; // asserted;
  fp += 1; // num_screens;
  fp += self->num_screens; // screen indices;
  return fp;
}

//-----------------------------------------------------------------------------

int Screen_get_footprint(Screen* self) {
  int fp = 1; // mode
  fp += 1; // enabled
  fp += 1; // timeout

  fp += 1; // num_samples;
  fp += self->num_samples*2; // sample pointers
  for(uint8_t i=0; i<self->num_samples; i++) {
    fp += Sample_get_footprint(self->samples[i]);
  }

  fp += 1; // num_controls
  fp += self->num_controls*2; // control pointers

  fp += CommandList_get_footprint(self->commands);
  
  fp += SCREEN_ROWS * 2;  // the pointers to the rows

  // the allocated rows themselves
  for(uint8_t i=0; i<SCREEN_ROWS; i++) {
    fp += (self->rows[i] != NULL) ? SCREEN_COLUMNS : 0;
  }
  return fp;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

uint16_t CommandList_get_footprint(CommandList* self) {
  uint16_t fp = 0;

  fp += CommandList_get_sparse_footprint(self);

  // the commands themselves
  for(uint8_t i=0; i<self->num_commands; i++) {
    fp += Command_get_footprint(self->commands[i]);
  }
  return fp;
}

//-----------------------------------------------------------------------------
 
uint16_t CommandList_get_sparse_footprint(CommandList* self) {
  uint16_t fp = 0;
  fp += 1;                       // num_commands;
  fp += self->num_commands * 2;  // the pointers to the commands
  return fp;
}
 
//-----------------------------------------------------------------------------

uint16_t Command_get_footprint(Command* self) {
  uint16_t fp = 0;
  fp += 1+1+1+2+2; // action, col, row, len, string pointer
  return fp;
}

//-----------------------------------------------------------------------------

