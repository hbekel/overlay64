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

#ifndef OVERLAY64_H
#define OVERLAY64_H

typedef enum { BINARY, CONFIG } Format;

#define USBASP_CONNECT     1
#define USBASP_WRITEFLASH  6
#define USBASP_READFLASH   4
#define USBASP_WRITEEEPROM 8
#define USBASP_DISCONNECT  2

int convert(int argc, char** argv);
int configure(int argc, char** argv);
int update(int argc, char** argv);
int program(int command, uint8_t* data, int size, unsigned int address);
int boot(void);
int reset(void);
bool identify(void);

bool wait(DeviceInfo *device, const char* message);
void prepare_devices(void);
bool file(const char* path);

void version(void);
void usage(void);
void failed(DeviceInfo *device);
void complain(void);
void footprint(volatile Config* config);

void fmemupdate(FILE *fp, void *buf,  uint16_t size);

#if defined(WIN32) && !defined(__CYGWIN__)
  FILE* fmemopen(void *__restrict buf, size_t size, const char *__restrict mode);
#endif
  
#endif // OVERLAY64_H
