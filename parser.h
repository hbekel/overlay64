#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>

#include "strings.h"
#include "config.h"

bool Config_parse(volatile Config* self, FILE* in);
void Config_print(volatile Config* self, FILE* out);
void Config_write(volatile Config* self, FILE* out);
uint16_t Config_get_footprint(volatile Config* self);

uint8_t Config_index_of_pin(volatile Config* self, Pin* pin);
uint8_t Config_index_of_string(volatile Config* self, char* string);
uint8_t Config_index_of_command(volatile Config* self, Command* command);

bool Sample_parse(Sample* self, StringList* words, int *i);
void Sample_print(Sample* self, FILE* out);
void Sample_write(Sample* self, FILE* out);
uint16_t Sample_get_footprint(Sample* self);

void Pin_print(Pin* self, FILE* out);
void Pin_write(Pin* self, FILE* out);

void CommandList_print(CommandList *self, FILE* out);
void CommandList_write_indexed(CommandList *self, FILE* out);
void CommandList_write(CommandList *self, FILE* out);
uint16_t CommandList_get_sparse_footprint(CommandList* self);
uint16_t CommandList_get_footprint(CommandList* self);

bool Command_parse(Command *self, int keyword, StringList* words, int *i);
void Command_print(Command *self, FILE* out);
void Command_write(Command *self, FILE* out);
uint16_t Command_get_footprint(Command* self);

#endif // PARSER_H
