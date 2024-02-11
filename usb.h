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

#ifndef OVERLAY64_USB_H
#define OVERLAY64_USB_H

#include <libusb-1.0/libusb.h>

#define OVERLAY64_VID 0x1d50
#define OVERLAY64_PID 0x6100

#define USBASP_VID 0x16c0
#define USBASP_PID 0x05dc

extern bool usb_quiet;

typedef struct {
  char path[4096];
  char role[64];
  unsigned int vid;
  unsigned int pid;
  int bus;
  int address;
  char *serial;
} DeviceInfo;

bool usb_ping(DeviceInfo *info);
int usb_control(DeviceInfo *info, uint8_t message);
int usb_send(DeviceInfo *info, uint8_t message, uint16_t value, uint16_t index, uint8_t* buf, uint16_t size);
int usb_receive(DeviceInfo* info, uint8_t message, uint16_t value, uint16_t index, uint8_t* buf, uint16_t size);

#endif // OVERLAY64_USB_H
