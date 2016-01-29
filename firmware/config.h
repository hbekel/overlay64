#ifndef FIRMWARE_CONFIG_H
#define FIRMWARE_CONFIG_H

#include "../config.h"

void Config_apply(Config* self);
void Sample_apply(Sample* self);
uint8_t Pin_read(Pin* self);
void CommandList_execute(CommandList* self);
void Command_execute(Command* self);

#endif // FIRMWARE_CONFIG_H
