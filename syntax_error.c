#include "syntax_error.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gc.h"


typedef wsky_SyntaxError SyntaxError;
typedef wsky_Position Position;


SyntaxError wsky_SyntaxError_create(const char *message,
                                    Position position) {
  if (!message)
    abort();
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

void wsky_SyntaxError_free(SyntaxError *this) {
  wsky_FREE(this->message);
}

void wsky_SyntaxError_delete(SyntaxError *this) {
  wsky_SyntaxError_free(this);
  wsky_FREE(this);
}



char *wsky_SyntaxError_toString(const SyntaxError *this) {
  char *positionString = wsky_Position_toString(&this->position);
  size_t length = strlen(this->message) + strlen(positionString) + 20;
  char *buffer = wsky_MALLOC(length);
  sprintf(buffer, "%s error: %s", positionString, this->message);
  wsky_FREE(positionString);
  return buffer;
}

void wsky_SyntaxError_print(const SyntaxError *this, FILE *output) {
  char *s = wsky_SyntaxError_toString(this);
  fprintf(output, "%s\n", s);
  wsky_FREE(s);
}
