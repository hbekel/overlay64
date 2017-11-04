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

#ifndef FIRMWARE_CONFIG_H
#define FIRMWARE_CONFIG_H

#include "../config.h"

void Config_setup(volatile Config* self);
void Config_setup_pins(volatile Config* self);
void Config_sample_pins(volatile Config* self);
void Config_tick(volatile Config* self);
void Config_apply(volatile Config* self);
void Control_sample(Control* self);
void Screen_sample(Screen* self);
bool Screen_has_effect(Screen* self);
void Screen_notify(Screen* self);
void Screen_write(Screen* self);
void Screen_link(Screen* self);
void Screen_unlink(Screen* self);
void Sample_sample(Sample* self, Screen* screen);
bool Sample_has_effect(Sample* self);
void Pin_setup(Pin* self);
uint8_t Pin_sample(Pin* self);
uint8_t Pin_state(Pin *self);
bool Pin_is_high(Pin* self);
bool Pin_is_low(Pin* self);
bool Pin_is_rising(Pin* self); 
bool Pin_is_falling(Pin* self); 
bool Pin_has_changed(Pin* self);
void CommandList_execute(CommandList* self);
void CommandList_execute_with_value(CommandList* self, uint8_t value);
void Command_execute(Command* self);
void Command_execute_with_value(Command* self, uint8_t value);
void Row_write(uint8_t* row, uint8_t col, char *str);
void Row_printf(uint8_t* row, uint8_t col, char *str, uint8_t value);
void Row_clear(uint8_t* row, uint8_t col, uint8_t len);

#endif // FIRMWARE_CONFIG_H
