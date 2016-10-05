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

#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>

typedef struct {
  int size;
  char **strings;
} StringList;

typedef struct {
  char* name;
  char* value;
} Definition;

void StringList_init(void);
bool StringList_add_definition(const char* name, const char* value);
bool StringList_has_definition(const char* name);
Definition* StringList_get_definition(const char* name);

StringList *StringList_new(void);
void StringList_append(StringList *self, const char *string, const char *delim);
void StringList_append_tokenized(StringList *self, const char* input, const char *delim);
void StringList_append_quoted(StringList *self, const char* input, const char *delim);
char* StringList_get(StringList *self, int index);
char* StringList_last(StringList *self);
void StringList_remove_last(StringList *self);
void StringList_free(StringList *self);

#endif // STRINGS_H
