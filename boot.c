#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "usb.h"
#include "protocol.h"

int boot() {
  printf("Entering bootloader...\n");  
  usb_send("/dev/overlay64", OVERLAY64_BOOT, 0, (uint8_t*)NULL, 0);
  return true;
}

