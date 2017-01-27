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

typedef enum { BINARY = 1, CONFIG } Format;

#define USBASP_CONNECT     1
#define USBASP_WRITEFLASH  6
#define USBASP_READFLASH   4
#define USBASP_WRITEEEPROM 8
#define USBASP_DISCONNECT  2

bool convert(int argc, char** argv);
bool configure(int argc, char** argv);
bool update(int argc, char** argv);
bool program(int command, uint8_t* data, int size, unsigned int address);
bool font_convert(char *input, char *output);
bool font_update(char *filename);
bool boot(void);
bool reset(void);
bool identify(void);

bool wait(DeviceInfo *device, const char* message);
void prepare_devices(void);
bool is_file(const char* path);
bool read_file(char* filename, uint8_t **data, int *size);
bool write_file(char* filename, uint8_t *data, int size);

void version(void);
void usage(void);
void failed(DeviceInfo *device);
void complain(void);
void footprint(volatile Config* config);

void fmemupdate(FILE *fp, void *buf,  uint16_t size);

#if defined(WIN32) && !defined(__CYGWIN__)
  FILE* fmemopen(void *__restrict buf, size_t size, const char *__restrict mode);
#endif

#if windows
unsigned int sleep(unsigned int sec) { Sleep(sec*1000); return 0; }
#endif

uint8_t backspace[]   = { 0x00, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00 };
uint8_t circumflex[]  = { 0x18, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t underscore[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00 };
uint8_t backtick[]    = { 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t left_curly[]  = { 0x1c, 0x30, 0x30, 0x60, 0x30, 0x30, 0x1c, 0x00 };
uint8_t pipe_symbol[] = { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 };
uint8_t right_curly[] = { 0x38, 0x0c, 0x0c, 0x06, 0x0c, 0x0c, 0x38, 0x00 };
uint8_t tilde[]       = { 0x64, 0x92, 0x92, 0x4c, 0x00, 0x00, 0x00, 0x00 };
uint8_t del[]         = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#endif // OVERLAY64_H
