#include "syntax_error.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gc.h"


typedef wsky_SyntaxError SyntaxError;
typedef wsky_Position Position;


SyntaxError wsky_SyntaxError_create(const char *message,
                                    Position position) {
  SyntaxError e = {
    .message = wsky_STRDUP(message),
    .position = position,
  };
  return e;
}

SyntaxError *wsky_SyntaxError_new(const char *message,
                                  Position position) {
  SyntaxError *e = wsky_MALLOC(sizeof(SyntaxError));
  *e = wsky_SyntaxError_create(message, position);
  return e;
}

void wsky_SyntaxError_free(SyntaxError *self) {
  wsky_FREE(self->message);
}

void wsky_SyntaxError_delete(SyntaxError *self) {
  wsky_SyntaxError_free(self);
  wsky_FREE(self);
}



char *wsky_SyntaxError_toString(const SyntaxError *self) {
  char *positionString = wsky_Position_toString(&self->position);
  size_t length = strlen(self->message) + strlen(positionString) + 20;
  char *buffer = wsky_MALLOC(length);
  sprintf(buffer, "%s error: %s", positionString, self->message);
  wsky_FREE(positionString);
  return buffer;
}

void wsky_SyntaxError_print(const SyntaxError *self, FILE *output) {
  char *s = wsky_SyntaxError_toString(self);
  fprintf(output, "%s\n", s);
  wsky_FREE(s);
}
