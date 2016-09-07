#ifndef FIRMWARE_CONFIG_H
#define FIRMWARE_CONFIG_H

#include "../config.h"

void Config_setup(volatile Config* self);
void Config_tick(volatile Config* self);
void Config_apply(volatile Config* self);
void Control_sample(Control* self);
void Screen_sample(Screen* self);
void Screen_notify(Screen* self);
void Screen_write(Screen* self);
void Screen_clear(Screen* self);
void Sample_sample(Sample* self, Screen* screen);
uint8_t Pin_sample(Pin* self);
uint8_t Pin_state(Pin *self);
bool Pin_is_high(Pin* self);
bool Pin_is_low(Pin* self);
bool Pin_is_rising(Pin* self); 
bool Pin_is_falling(Pin* self); 
bool Pin_has_changed(Pin* self);
void CommandList_execute(CommandList* self);
void CommandList_execute_clear(CommandList* self);
void Command_execute(Command* self);
void Command_execute_clear(Command* self);
void Row_write(uint8_t* row, uint8_t col, char *str);
void Row_clear(uint8_t* row, uint8_t col, uint8_t len);

#endif // FIRMWARE_CONFIG_H
