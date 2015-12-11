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

void wsky_SyntaxError_print(const wsky_SyntaxError *this, FILE *output) {
  fprintf(output, "%s", this->message);
}
