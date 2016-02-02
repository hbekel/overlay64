#ifndef FIRMWARE_CONFIG_H
#define FIRMWARE_CONFIG_H

#include "../config.h"

void Config_apply(volatile Config* self);
void Sample_apply(Sample* self);
uint8_t Pin_read(Pin* self);
void CommandList_execute(CommandList* self);
void Command_execute(Command* self);
void Row_write(uint8_t* row, uint8_t col, char *str);
void Row_clear(uint8_t* row, uint8_t col, uint8_t len);

#endif // FIRMWARE_CONFIG_H
