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
uint8_t Config_index_of_screen(volatile Config* self, Screen* screen);
  
bool Control_parse(Control* self, StringList* words, int *i);
void Control_print(Control* self, FILE* out);
void Control_write(Control* self, FILE* out);
int Control_get_footprint(Control* self);

bool Screen_parse(Screen* self, StringList* words, int *i);
void Screen_print(Screen* self, FILE* out);
void Screen_write(Screen* self, FILE* out);
int Screen_get_footprint(Screen* self);

bool Sample_parse(Sample* self, StringList* words, int *i);
void Sample_print(Sample* self, FILE* out);
void Sample_write(Sample* self, FILE* out);
uint16_t Sample_get_footprint(Sample* self);

void Pin_print(Pin* self, FILE* out);
void Pin_write(Pin* self, FILE* out);

void CommandList_print(CommandList *self, FILE* out);
void CommandList_write(CommandList *self, FILE* out);
uint16_t CommandList_get_sparse_footprint(CommandList* self);
uint16_t CommandList_get_footprint(CommandList* self);

bool Command_parse(Command *self, int keyword, StringList* words, int *i);
void Command_print(Command *self, FILE* out);
void Command_write(Command *self, FILE* out);
uint16_t Command_get_footprint(Command* self);

#endif // PARSER_H
