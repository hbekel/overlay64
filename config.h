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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define CHAR_WIDTH    8
#define CHAR_HEIGHT   8

#define SCREEN_COLUMNS 52
#define SCREEN_ROWS    30

#define SCREEN_TOP    46
#define SCREEN_BOTTOM SCREEN_TOP + CHAR_HEIGHT * SCREEN_ROWS

#define MODE_MANUAL 1
#define MODE_NOTIFY 2
#define MODE_ALWAYS 3

#define NUM_PINS 24

#define ACTION_NONE  0x00
#define ACTION_WRITE 0x01
#define ACTION_CLEAR 0x02

extern uint8_t CONFIG_MAGIC[2];

typedef struct {
  uint8_t volatile *port; // pointer into Config->ports
  uint8_t pos;  
  uint8_t edge[2];
} Pin;

typedef struct {
  Pin* pin;

  uint8_t mode;
  bool asserted;
  
  uint8_t *screens; // index into config->screens
  uint8_t num_screens;

} Control;

typedef struct {
  void *screen;
  uint8_t action;
  uint8_t row;
  uint8_t col;
  uint16_t len;
  char* string; // pointer into config->strings
} Command;

typedef struct {
  void *screen;
  Command **commands;
  uint8_t num_commands;
} CommandList;

typedef struct {
  void *screen; 
  Pin **pins; // pointers into Config->pins
  uint8_t num_pins;
  uint8_t value;

  CommandList *command_list; // immediate commands
  
  CommandList **command_lists; // one command list for each state
  uint8_t num_command_lists;
} Sample;

typedef struct {
  uint8_t mode;
  bool enabled;
  uint8_t timeout;

  Sample **samples;
  uint8_t num_samples;
 
  Control **controls;
  uint8_t num_controls;  
  
  CommandList* commands;

  uint8_t **rows;
  
} Screen;

typedef struct {
  uint8_t volatile *ports[4]; // the actual ports to use
  Pin *pins[NUM_PINS];        // the available pins

  bool enabled;
  uint8_t timeout;

  char **strings;
  uint8_t num_strings;
  
  Control **controls;
  uint8_t num_controls;

  Screen **screens;
  uint8_t num_screens;
  
  uint8_t **rows;
  
} Config;

volatile Config* config;

volatile Config *Config_new(void);
volatile Config* Config_new_with_ports(uint8_t volatile *a,
                                       uint8_t volatile *b,
                                       uint8_t volatile *c,
                                       uint8_t volatile *d);

Control* Config_add_control(volatile Config *self, Control* control);
Screen* Config_add_screen(volatile Config *self, Screen* screen);
bool Config_has_string(volatile Config *self, char* string, uint8_t *index);
char *Config_add_string(volatile Config *self, char* string);
bool Config_read(volatile Config *self, FILE *in);
void Config_each_command(volatile Config* self,
                         void (*callback)(Screen* screen, Command* command));
void Config_allocate_row_for_command(Screen* screen, Command *command);
void Config_allocate_rows(volatile Config *self);
void Config_assign_controls_to_screens(volatile Config* self);
bool Config_install_fallback(volatile Config* self);
void Config_free(volatile Config* self);

Control* Control_new(void);
void Control_add_screen(Control* self, uint8_t index);
void Control_read(Control* self, FILE* in);
void Control_free(Control* self);

Screen* Screen_new(void);
Screen* Screen_add_control(Screen *self, Control* control);
Sample* Screen_add_sample(Screen *self, Sample* sample);
void Screen_read(Screen* self, FILE* in);
void Screen_free(Screen* self);

Sample* Sample_new(Screen* screen);
Pin* Sample_add_pin(Sample* self, Pin* pin);
void Sample_read(Sample* self, FILE* in);
CommandList* Sample_add_commands(Sample* self, CommandList* commands);
void Sample_free(Sample* self);

Command* Command_new(Screen *screen);
void Command_set_string(Command* self, char *string);
bool Command_equals(Command* self, Command* command);
void Command_read(Command* self, FILE* in);
void Command_free(Command* self);

CommandList* CommandList_new(Screen* screen);
Command* CommandList_add_command(CommandList *self, Command* command);
void CommandList_read(CommandList *self, FILE *in);
void CommandList_free(CommandList* self);

Pin *Pin_new(volatile Config* config, uint8_t port, uint8_t pos);
void Pin_free(Pin *self);

#endif // CONFIG_H
