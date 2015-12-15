#include "syntax_error.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

wsky_SyntaxError wsky_SyntaxError_create(const char *message,
                                         wsky_Position position) {
  if (!message)
    abort();
  wsky_SyntaxError e = {
    .message = strdup(message),
    .position = position,
  };
  return e;
}

wsky_SyntaxError *wsky_SyntaxError_new(const char *message,
                                       wsky_Position position) {
  wsky_SyntaxError *e = malloc(sizeof(wsky_SyntaxError));
  *e = wsky_SyntaxError_create(message, position);
  return e;
}

void wsky_SyntaxError_free(wsky_SyntaxError *this) {
  free(this->message);
}

void wsky_SyntaxError_delete(wsky_SyntaxError *this) {
  wsky_SyntaxError_free(this);
  free(this);
}



char *wsky_SyntaxError_toString(const wsky_SyntaxError *this) {
  char *positionString = wsky_Position_toString(&this->position);
  size_t length = strlen(this->message) + strlen(positionString) + 20;
  char *buffer = malloc(length);
  sprintf(buffer, "%s error: %s", positionString, this->message);
  free(positionString);
  return buffer;
}

void wsky_SyntaxError_print(const wsky_SyntaxError *this, FILE *output) {
  char *s = wsky_SyntaxError_toString(this);
  fprintf(output, "%s\n", s);
  free(s);
}
