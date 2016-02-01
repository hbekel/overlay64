#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "strings.h"

StringList *StringList_new(void) {
  StringList *stringlist = (StringList*) calloc(1, sizeof(StringList));
  stringlist->size = 0;
  stringlist->strings = (char**) NULL;
  return stringlist;
}

void StringList_append(StringList *self, char *string) {
  self->strings = (char**) realloc(self->strings, (self->size+1) * sizeof(char *));
  self->strings[self->size] = calloc(strlen(string)+1, sizeof(char));
  strncpy(self->strings[self->size], string, strlen(string));
  self->size++;
}

void StringList_append_tokenized(StringList *self, const char* input, const char *delim) {
  char *substring;

  char *string = (char*) calloc(strlen(input)+1, sizeof(char));
  strncpy(string, input, strlen(input));
  
  if((substring = strtok(string, delim)) != NULL) {
    StringList_append(self, substring);
  
    while((substring = strtok(NULL, delim)) != NULL) {
      StringList_append(self, substring);
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
        StringList_append(self, token);
        token[pos=0] = '\0';
        continue;
      }
    }

    if((!literal) && (strchr(delim, c) != NULL)) {      
      if(strlen(token)) {
        StringList_append(self, token);
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
    StringList_append(self, token);
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
