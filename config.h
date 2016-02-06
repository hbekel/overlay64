#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define CHAR_WIDTH    8
#define CHAR_HEIGHT   8

#define SCREEN_COLUMNS 53
#define SCREEN_ROWS    30

#define SCREEN_TOP    40
#define SCREEN_BOTTOM SCREEN_TOP + CHAR_HEIGHT * SCREEN_ROWS

#define INPUT_PINS 13

#define ACTION_NONE  0x00
#define ACTION_WRITE 0x01
#define ACTION_CLEAR 0x02

extern uint8_t CONFIG_MAGIC[2];

typedef struct {
  uint8_t volatile *port; // pointer into Config->ports
  uint8_t pos; 
} Pin;

typedef struct {
  uint8_t action;
  uint8_t row;
  uint8_t col;
  uint16_t len;
  char* string; // pointer into config->strings
} Command;

typedef struct {
  Command **commands;
  uint8_t num_commands;
} CommandList;

typedef struct {
  Pin **pins; // pointers into Config->pins
  uint8_t num_pins;
  
  CommandList **commands; // one command list for each state
  uint8_t num_commands;
} Sample;

typedef struct {
  uint8_t volatile *ports[3]; // the actual ports to use
  Pin *pins[INPUT_PINS];      // the available pins

  uint8_t timeout;
  
  Sample **samples;
  uint8_t num_samples;

  CommandList* immediateCommands;
  CommandList* commands;
  
  char **strings;
  uint8_t num_strings;

  uint8_t **rows;
  
} Config;

volatile Config* config;

volatile Config *Config_new(void);
volatile Config* Config_new_with_ports(uint8_t volatile *port0,
                              uint8_t volatile *port1,
                              uint8_t volatile *port2);

Sample* Config_add_sample(volatile Config *self, Sample* sample);
bool Config_has_string(volatile Config *self, char* string, uint8_t *index);
char *Config_add_string(volatile Config *self, char* string);
bool Config_has_command(volatile Config *self, Command* command, uint8_t *index);
Command* Config_add_command(volatile Config *self, Command* command);
bool Config_read(volatile Config *self, FILE *in);
void Config_allocate_rows(volatile Config *self);
void Config_free(volatile Config* self);

Sample* Sample_new(void);
Pin* Sample_add_pin(Sample* self, Pin* pin);
void Sample_read(Sample* self, FILE* in);
CommandList* Sample_add_commands(Sample* self, CommandList* commands);
void Sample_free(Sample* self);

Command* Command_new(void);
void Command_set_string(Command* self, char *string);
bool Command_equals(Command* self, Command* command);
void Command_read(Command* self, FILE* in);
void Command_free(Command* self);

CommandList* CommandList_new(void);
Command* CommandList_add_command(CommandList *self, Command* command);
Command* CommandList_add_new_command(CommandList *self, Command* command);
void CommandList_read(CommandList *self, FILE *in);
void CommandList_read_indexed(CommandList *self, FILE* in);
void CommandList_free(CommandList* self);

Pin *Pin_new(volatile Config* config, uint8_t port, uint8_t pos);
void Pin_free(Pin *self);

#endif // CONFIG_H
