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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

#include "target.h"

#if windows
  #include <io.h>
  #include <fcntl.h>
#endif

#if cygwin
  int _fileno(FILE*);
#endif

#include "parser.h"
#include "usb.h"
#include "protocol.h"
#include "intelhex.h"
#include "overlay64.h"

//-----------------------------------------------------------------------------

#if linux
  char* device = "/dev/overlay64";
#else
  char* device = "usb";
#endif

DeviceInfo overlay64;
DeviceInfo usbasp;

extern uint16_t written;
extern volatile Config *config;

//-----------------------------------------------------------------------------

int main(int argc, char **argv) {

  int result = EXIT_SUCCESS;

  struct option options[] = {
    { "help",     no_argument,       0, 'h' },
    { "version",  no_argument,       0, 'v' },
    { 0, 0, 0, 0 },
  };
  int option, option_index;

  while(1) {
    option = getopt_long(argc, argv, "hv", options, &option_index);

    if(option == -1)
      break;

    switch (option) {

    case 'h':
      usage();
      goto done;
      break;

    case 'v':
      version();
      goto done;
      break;

    case '?':
    case ':':
      goto done;
    }
  }

  argc -= optind;
  argv += optind;

  prepare_devices();

  if(argc == 1) {

    if(is_file(argv[0])) {
      result = configure(argc, argv);
    }
    else if(strncmp(argv[0], "boot", 1) == 0) {
      result = boot();
    }
    else if(strncmp(argv[0], "reset", 1) == 0) {
      result = reset();
    }
    else if(strncmp(argv[0], "identify", 2) == 0) {
      result = identify();
    }
    else goto usage;
  }

  else if(argc == 2) {

    if(strncmp(argv[0], "configure", 4) == 0) {
      result = configure(--argc, ++argv);
    }
    else if(strcmp(argv[0], "font-update") == 0) {
      result = font_update(argv[1]);
    }
    else goto usage;
  }

  else if(argc == 3) {
    if(strncmp(argv[0], "convert", 4) == 0) {
      result = convert(--argc, ++argv);
    }
    else if(strcmp(argv[0], "update") == 0) {
      result = update(--argc, ++argv);
    }
    else if(strcmp(argv[0], "font-convert") == 0) {
      result = font_convert(argv[1], argv[2]);
    }
    else goto usage;
  }

  else {
  usage:
    usage();
    complain();
    result = false;
  }

 done:
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

//-----------------------------------------------------------------------------

bool convert(int argc, char **argv) {

  bool result = false;

  FILE *in  = stdin;
  FILE *out = stdout;
  Format output_format = BINARY;

  config = Config_new();

  if(argc >= 1 && (strncmp(argv[0], "-", 1) != 0)) {

    if((in = fopen(argv[0], "rb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
      goto done;
    }
  }

  if(argc >= 2 && (strncmp(argv[1], "-", 1) != 0)) {

    if((out = fopen(argv[1], "wb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
      goto done;
    }
  }

  if(in == stdin) {
    fprintf(stderr, "Reading from stdin...\n");
#if windows
    setmode(_fileno(stdin), O_BINARY);
#endif
  }

  if(out == stdout) {
    fprintf(stderr, "Writing to stdout...\n");
#if windows
    setmode(_fileno(stdout), O_BINARY);
#endif
  }

  if((Config_read(config, in)  && (output_format = CONFIG)) ||
     (Config_parse(config, in) && (output_format = BINARY))) {

    output_format == BINARY ?
      Config_write(config, out) :
      Config_print(config, out);

    footprint(config);

    result = true;
  }

 done:
  Config_free(config);
  return result;
}

//-----------------------------------------------------------------------------

bool configure(int argc, char **argv) {

  bool result = false;

  FILE *in  = stdin;
  FILE *out = NULL;
  uint8_t *data = NULL;
  uint16_t size = 0;

  config = Config_new();

  if(argc >= 1 && (strncmp(argv[0], "-", 1) != 0)) {

    if((in = fopen(argv[0], "rb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
      goto done;
    }
  }

  if(in == stdin) {
    fprintf(stderr, "Reading from stdin...\n");
#if windows
    setmode(_fileno(stdin), O_BINARY);
#endif
  }

  fprintf(stderr, "Reading %s...\n", argv[0]);

  if(Config_read(config, in) || Config_parse(config, in)) {

    data = (uint8_t*) calloc(4096, sizeof(char));

    if((out = fmemopen(data, 4096, "wb")) == NULL) {
      fprintf(stderr, "error: %s\n", strerror(errno));
      goto done;
    }

    Config_write(config, out);
    size = ftell(out);
    fmemupdate(out, data, size);
    fclose(out);

    footprint(config);

    if(usb_ping(&usbasp)) {
      reset();
    }

    fprintf(stderr, "Flashing configuration: %d bytes...", size);
    fflush(stderr);

    int tries = 5;
    bool quiet = usb_quiet;

    usb_quiet = true;

    while(tries--) {
      if((result = usb_send(&overlay64, OVERLAY64_FLASH, 0, 0, data, size) == size)) {
        break;
      }
    }

    usb_quiet = quiet;

    fprintf(stderr, result ? "ok\n" : "failed!\n");

    if(result) {
      result = expect(&overlay64, "Resetting device");
    }
  }

 done:
  Config_free(config);
  fclose(in);

  if(data != NULL) free(data);
  return result;
}

//-----------------------------------------------------------------------------

static bool ends_with(const char *str, const char *end) {
  if(str == NULL || end == NULL) return false;
  if(strlen(end) > strlen(str)) return false;
  return strncasecmp(str+strlen(str)-strlen(end), end, strlen(end)) == 0;
}

//-----------------------------------------------------------------------------

bool update(int argc, char **argv) {
  bool result = false;

  char *filename = argv[0];
  unsigned int address = 0;
  uint8_t *data = (uint8_t *) calloc(1, sizeof(uint8_t));
  int size = 0;

  if(!argc) {
    usage();
    goto done;
  }

  if(!(result = read_file(filename, &data, &size))) {
    goto done;
  }

  if(ends_with(filename, ".hex")) {
    fprintf(stderr, "Trying to parse Intel HEX format..."); fflush(stderr);

    data = readhex(data, &size, &address);

    if(!data) {
      fprintf(stderr, "FAILED!\n");
      errno = EINVAL;
      goto error;
    }
    fprintf(stderr, "OK\nParsed %d bytes starting at 0x%02X\n", size, address);
  }
  else if(!ends_with(filename, ".bin")) {
    fprintf(stderr, "error: please supply firmware as a .bin or .hex file\n");
    goto done;
  }

  if(argc == 2) {
    uint8_t erased[2] = { 0x00, 0x00 };

    if(!usb_ping(&overlay64)) {
      fprintf(stderr, "error: could not connect to overlay64\n");
      goto done;
    }

    fprintf(stderr, "Deactivating existing configuration...");
    fflush(stderr);

    result = usb_send(&overlay64, OVERLAY64_FLASH, 0, 0, erased, 2) == 2;
    fprintf(stderr, result ? "ok\n" : "failed!\n");
    if(!result) goto done;

    expect(&overlay64, "Waiting for overlay64 to reboot");
  }

  if((result = program(USBASP_WRITEFLASH, data, size, address))) {

    if(argc == 2) {
      argc--; argv++;
      result = configure(argc, argv);
    }
  }

 done:
  free(data);
  return result;

 error:
  fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
  goto done;
}

//-----------------------------------------------------------------------------

bool font_update(char* filename) {
  bool result = false;

  uint8_t *data = (uint8_t *) calloc(1, sizeof(uint8_t));
  int size = 0;

  if(read_file(filename, &data, &size)) {
    result = program(USBASP_WRITEFLASH, data, 96*8, OFFSET);
  }

  free(data);
  return result;
}

//-----------------------------------------------------------------------------

bool font_convert(char* infile, char* outfile) {
  bool result = false;
  int size = 0;

  uint8_t *data_in = (uint8_t*) calloc(1, sizeof(uint8_t));
  uint8_t *data_out = (uint8_t*) calloc(1, sizeof(uint8_t));

  if((result = read_file(infile, &data_in, &size))) {
    if(size < (256+32)*8) {
      fprintf(stderr, "error: input file must be at least %d bytes\n", (256+32)*8);
      goto done;
    }
    data_out = (uint8_t*) realloc(data_out, size);
    memcpy(data_out, data_in, size);

    memcpy(data_out, data_in + 32*8, 32*8);
    memcpy(data_out + 32*8, data_in, 32*8);
    memcpy(data_out + 64*8, data_in + 256*8, 32*8);

    memcpy(data_out + 60*8, backspace, 8);
    memcpy(data_out + 62*8, circumflex, 8);
    memcpy(data_out + 63*8, underscore, 8);
    memcpy(data_out + 64*8, backtick, 8);
    memcpy(data_out + 91*8, left_curly, 8);
    memcpy(data_out + 92*8, pipe_symbol, 8);
    memcpy(data_out + 93*8, right_curly, 8);
    memcpy(data_out + 94*8, tilde, 8);
    memcpy(data_out + 95*8, del, 8);

    result = write_file(outfile, data_out, 96*8);
  }

 done:
  free(data_in);
  free(data_out);
  return result;
}

//-----------------------------------------------------------------------------

bool program(int command, uint8_t *data, int size, unsigned int address)  {

  const char *type = (command == USBASP_WRITEEEPROM) ?
    "configuration" :
    (address == OFFSET) ? "font" : "application";

  if(boot()) {
    usb_control(&usbasp, USBASP_CONNECT);

    for(uint32_t i=0; i<size+64; i+=64) {
      usb_send(&usbasp, command,
               (uint16_t) (address & 0xffff), (uint16_t) (address>>16),
               data+i, 64);
      address+=64;
      fprintf(stderr, "\rUpdating %s: %d of %d bytes transferred...",
              type, (i<size) ? i : size , size);
    }
    fprintf(stderr, "OK\n");

    usb_quiet = true;
    usb_control(&usbasp, USBASP_DISCONNECT);
  }
  else {
    failed(&usbasp);
    return false;
  }
  return expect(&overlay64, "Resetting device");
}

//-----------------------------------------------------------------------------

bool boot(void) {
  if(usb_ping(&usbasp)) {
    fprintf(stderr, "Device already in bootloader mode\n");
    return true;
  }

  if(!usb_ping(&overlay64)) {
    failed(&overlay64);
    return false;
  }
  int tries = 5;
  bool quiet = usb_quiet;

  usb_quiet = true;

  while(tries--) {
    if(usb_control(&overlay64, OVERLAY64_BOOT) >= 0) {
      break;
    }
  }

  usb_quiet = quiet;

  return expect(&usbasp, "Entering bootloader");
}

//-----------------------------------------------------------------------------

bool expect(DeviceInfo *device, const char* message) {

  fprintf(stderr, "%s", message); fflush(stderr);

  uint8_t tries = 10;
  usb_quiet = true;

  do {
    sleep(1);
    fprintf(stderr, "."); fflush(stderr);

    if(!--tries) {
      return false;
    }
  } while(!usb_ping(device));

  fprintf(stderr, "\n");
  identify();
  return true;
}

//-----------------------------------------------------------------------------

bool reset(void) {

  if(usb_ping(&overlay64)) {
    usb_control(&overlay64, OVERLAY64_RESET);
  }
  else if(usb_ping(&usbasp)) {
    usb_quiet = true;
    usb_control(&usbasp, USBASP_CONNECT);
    usb_control(&usbasp, USBASP_DISCONNECT);
  }
  else {
    failed(&overlay64);
    failed(&usbasp);
    return false;
  }
  return expect(&overlay64, "Resetting device");
}

//-----------------------------------------------------------------------------

bool identify(void) {
  char id[64];

  if(usb_ping(&overlay64)) {
    if(usb_receive(&overlay64, OVERLAY64_IDENTIFY, 0, 0, (uint8_t*) id, 64) > 0) {
      printf("%s\n", id);
      return true;
    }
  }
  else if(usb_ping(&usbasp)) {
    fprintf(stderr, "USBaspLoader (C)2008 by OBJECTIVE DEVELOPMENT Software GmbH\n");
    return true;
  }
  else {
    failed(&overlay64);
    failed(&usbasp);
  }
  return false;
}

//-----------------------------------------------------------------------------

void prepare_devices(void) {
  strncpy(overlay64.path, device, 4096);
  strncpy(overlay64.role, "Overlay64", 64);
  overlay64.vid = OVERLAY64_VID;
  overlay64.pid = OVERLAY64_PID;

#if linux
  strncpy(usbasp.path, "/dev/usbasp", 4096);
#else
  strncpy(usbasp.path, "usb", 4096);
#endif
  strncpy(usbasp.role, "Bootloader", 64);
  usbasp.vid = USBASP_VID;
  usbasp.pid = USBASP_PID;
}

//-----------------------------------------------------------------------------

bool is_file(const char* path) {
  FILE *in = NULL;

  if(strlen(path) == 1 && path[0] == '-') {
    return true;
  }

  if((in = fopen(path, "rb")) != NULL) {
    fclose(in);
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------

bool read_file(char* filename, uint8_t **data, int *size) {

  bool result = false;
  FILE *in = NULL;

  struct stat st;

  if((in = fopen(filename, "rb")) == NULL) {
    goto error;
  }

  if(fstat(fileno(in), &st) == -1) {
    goto error;
  }

  (*size) = st.st_size;
  (*data) = realloc((*data), (*size));

  if(fread((*data), sizeof(uint8_t), (*size), in) != (*size)) {
    goto error;
  }
  fclose(in);

  result = true;

 done:
  return result;

 error:
  fprintf(stderr, "%s: %s\n", filename, strerror(errno));
  goto done;
}

//-----------------------------------------------------------------------------

bool write_file(char* filename, uint8_t *data, int size) {
  bool result = false;
  FILE* f = NULL;

  if(!(f = fopen(filename, "wb"))) {
    goto error;
  }

  result = fwrite(data, sizeof(uint8_t), size, f) == size;

 done:
  if(f) fclose(f);
  return result;

 error:
  fprintf(stderr, "%s: %s\n", filename, strerror(errno));
  goto done;
}

//-----------------------------------------------------------------------------

#if (defined(WIN32) && !defined(__CYGWIN__)) || defined(__APPLE__)
FILE* fmemopen(void *__restrict buf, size_t size, const char *__restrict mode) {

  FILE* result;
  char file[4096] = "overlay64-XXXXXX";

#if defined(WIN32)
  char path[4096];
  if(!GetTempPath(4096, path)) return NULL;
  if(!GetTempFileName(path, "key", 0, file)) return NULL;
  result = fopen(file, "wbD+");
#else
  int fd = mkstemp(file);
  result = fdopen(fd, "w+");
#endif

  fwrite(buf, sizeof(char), size, result);
  fseek(result, 0, SEEK_SET);

  return result;
}
#endif

//-----------------------------------------------------------------------------

void fmemupdate(FILE *fp, void *buf,  uint16_t size) {
#if defined(WIN32) && !defined(__CYGWIN__) || defined(__APPLE__)
  fseek(fp, 0, SEEK_SET);
  fread(buf, sizeof(uint8_t), size, fp);
#endif
}

//-----------------------------------------------------------------------------

void footprint(volatile Config* config) {

  uint16_t footprint = Config_get_footprint(config);

  fprintf(stderr, "SRAM:\t%5d of 16384 bytes used (%5d bytes free)\n",
          footprint, 16384-footprint);

  fprintf(stderr, "EEPROM:\t%5d of  4096 bytes used (%5d bytes free)\n",
          written, 4096-written);
}

//-----------------------------------------------------------------------------

void version(void) {
  printf("overlay64 v%.1f\n", VERSION);
  printf("Copyright (C) 2016 Henning Bekel.\n");
  printf("License GPLv3: GNU GPL version 3 <http://gnu.org/licenses/gpl.html>.\n");
  printf("This is free software: you are free to change and redistribute it.\n");
  printf("There is NO WARRANTY, to the extent permitted by law.\n");
}

//-----------------------------------------------------------------------------

void usage(void) {
  version();
  printf("\n");
  printf("Usage:\n");
  printf("      overlay64 <options>\n");
  printf("      overlay64 [configure] <infile|->\n");
  printf("      overlay64 convert [<infile>|-] [<outfile>|-]\n");
  printf("      overlay64 update <firmware> [<config>]\n");
  printf("      overlay64 font-convert <infile> <outfile>\n");
  printf("      overlay64 font-update <infile>\n");
  printf("      overlay64 identify\n");
  printf("      overlay64 boot\n");
  printf("      overlay64 reset\n");
  printf("\n");
  printf("  Options:\n");
  printf("      -v, --version : print version information\n");
  printf("      -h, --help    : print this help text\n");
  printf("\n");
  printf("  Commands:\n");
  printf("      configure    : read/parse configuration and flash to eeprom\n");
  printf("      convert      : convert configuration to/from binary/text format\n");
  printf("      update       : update firmware from Intel HEX file\n");
  printf("      font-convert : convert C64 charset to overlay64 font file\n");
  printf("      font-update  : install font from overlay64 font file\n");
  printf("      identify     : report firmware version and build date\n");
  printf("      boot         : make device enter bootloader mode\n");
  printf("      reset        : reset device (leave bootloader/restart application)\n");
  printf("\n");
  printf("  Files:\n");
  printf("      <infile>   : input file, format is autodetected\n");
  printf("      <outfile>  : output file, format determined by extension\n");
  printf("      <firmware> : firmware in binary or Intel HEX format (.bin or .hex)\n");
  printf("\n");
  printf("      *.conf : plain text config file format\n");
  printf("      *.bin  : binary file format (default)\n");
  printf("      *.hex  : Intel HEX file format\n");
  printf("      -      : standard input/output\n");
  printf("\n");
}

//-----------------------------------------------------------------------------

void complain(void) {
#if defined(WIN32) && !defined(__CYGWIN__)
  if(!(!GetConsoleTitle(NULL, 0) && GetLastError() == ERROR_SUCCESS)) {
    printf("\n!THIS IS A COMMANDLINE APPLICTION, PLEASE RUN "
           "IT FROM A COMMAND PROMPT INSTEAD!\n\n");
    system("pause");
  }
#endif
}

//-----------------------------------------------------------------------------

void failed(DeviceInfo *device) {
  fprintf(stderr, "error: failed to open %s device \"%s\" (%04X:%04X)\n",
          device->role, device->path, device->vid, device->pid);
}

//-----------------------------------------------------------------------------
