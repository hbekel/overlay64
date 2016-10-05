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
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "strings.h"
#include "target.h"

#if windows
static char *strtok_r(char *str, const char *delim, char **save)
{
  char *res, *last;

  if( !save )
    return strtok(str, delim);
  if( !str && !(str = *save) )
    return NULL;
  last = str + strlen(str);
  if( (*save = res = strtok(str, delim)) )
    {
      *save += strlen(res);
      if( *save < last )
        (*save)++;
      else
        *save = NULL;
    }
  return res;
}
#endif

Definition** StringList_definitions = NULL;
int StringList_num_definitions = 0;

void StringList_init() {
  if(StringList_definitions == NULL) {
    StringList_definitions = (Definition**) calloc(1, sizeof(Definition*));
  }
}

Definition *Definition_new(const char* name, const char* value) {
  Definition* self = (Definition*) calloc(1, sizeof(Definition));

  self->name = (char*) calloc(strlen(name)+1, sizeof(char));
  strncpy(self->name, name, strlen(name)+1);

  self->value = (char*) calloc(strlen(value)+1, sizeof(char));
  strncpy(self->value, value, strlen(value)+1);

  return self;
}

bool StringList_add_definition(const char* name, const char* value) {

  if(StringList_has_definition(name)) {
    return false;
  }
  Definition* definition = Definition_new(name, value);

  StringList_definitions =
    (Definition**) realloc(StringList_definitions,
                           (StringList_num_definitions+1) * sizeof(Definition*));

  StringList_definitions[StringList_num_definitions] = definition;

  StringList_num_definitions++;
  return true;
}

bool StringList_has_definition(const char* name) {
  return StringList_get_definition(name) != NULL;
}

Definition* StringList_get_definition(const char* name) {
  Definition* definition;
  for(int i=0; i<StringList_num_definitions; i++) {
    definition = StringList_definitions[i];
    if(strcmp(definition->name, name) == 0) {
      return definition;
    }
  }
  return NULL;
}

StringList *StringList_new(void) {
  StringList_init();
  
  StringList *stringlist = (StringList*) calloc(1, sizeof(StringList));
  stringlist->size = 0;
  stringlist->strings = (char**) NULL;  
  return stringlist;
}

void StringList_append(StringList *self, const char *string, const char* delim) {  

  if(StringList_has_definition(string)) {
    StringList_append_tokenized(self, StringList_get_definition(string)->value, delim);
  }
  else {
    self->strings = (char**) realloc(self->strings, (self->size+1) * sizeof(char *));
    self->strings[self->size] = calloc(strlen(string)+1, sizeof(char));
    strncpy(self->strings[self->size], string, strlen(string));
    self->size++;
  }
}

void StringList_append_tokenized(StringList *self, const char* input, const char *delim) {
  char *substring;
  char *saveptr;
  
  char *string = (char*) calloc(strlen(input)+1, sizeof(char));
  strncpy(string, input, strlen(input));
  
  if((substring = strtok_r(string, delim, &saveptr)) != NULL) {
    StringList_append(self, substring, delim);
  
    while((substring = strtok_r(NULL, delim, &saveptr)) != NULL) {
      StringList_append(self, substring, delim);
    } 
  }
  free(string);
}

void StringList_append_quoted(StringList *self, const char* input, const char *delim) {

  bool literal = false;
  bool escaped = false;

  char *token = (char *) calloc(4096, sizeof(char));
  int pos = 0;
  char c;
  
  for(int i=0; i<strlen(input); i++) {
    c = input[i];
    
    if(c == '\\' && !escaped) {
      escaped = true;
      continue;
    }
    
    if(c == '"' && !escaped) {
      literal = !literal;
      if(!literal) {
        StringList_append(self, token, delim);
        token[pos=0] = '\0';
        continue;
      }
    }

    if((!literal) && (strchr(delim, c) != NULL)) {      
      if(strlen(token)) {
        StringList_append(self, token, delim);
        token[pos=0] = '\0';
      }
      continue;
    }
    
    pos = strlen(token);
    token[pos] = c;
    token[pos+1] = '\0';

    if(escaped) {
      escaped = false;
    }
  }
  
  if(strlen(token)) {
    StringList_append(self, token, delim);
    token[pos=0] = '\0';
  }  
  free(token);
}

char* StringList_get(StringList *self, int index) {
  if(index >= 0 && index < self->size) {
    return self->strings[index];
  }
  return NULL;
}

char* StringList_last(StringList *self) {
  return self->strings[self->size-1];
}

void StringList_remove_last(StringList *self) {
  free(self->strings[--self->size]);
}

void StringList_free(StringList *self) {
  for(int i=0; i<self->size; i++) {
    free(self->strings[i]);
  }
  free(self->strings);
  free(self);
}
