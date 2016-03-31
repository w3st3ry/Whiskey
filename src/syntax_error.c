#include <string.h>
#include "whiskey_private.h"


SyntaxError wsky_SyntaxError_create(const char *message,
                                    Position position) {
  SyntaxError e = {
    .message = wsky_strdup(message),
    .position = position,
    .expectedSomething = false,
  };
  return e;
}

SyntaxError *wsky_SyntaxError_new(const char *message,
                                  Position position) {
  SyntaxError *e = wsky_safeMalloc(sizeof(SyntaxError));
  *e = wsky_SyntaxError_create(message, position);
  return e;
}


void wsky_SyntaxError_copy(wsky_SyntaxError *dest,
                           const wsky_SyntaxError *source)
{
  dest->message = wsky_strdup(source->message);
  dest->position = source->position;
  dest->expectedSomething = source->expectedSomething;
}

void wsky_SyntaxError_free(SyntaxError *self) {
  wsky_free(self->message);
}

void wsky_SyntaxError_delete(SyntaxError *self) {
  wsky_SyntaxError_free(self);
  wsky_free(self);
}



char *wsky_SyntaxError_toString(const SyntaxError *self) {
  char *positionString = wsky_Position_toString(&self->position);
  char *buffer = wsky_asprintf("%s error: %s",
                               positionString, self->message);
  wsky_free(positionString);
  return buffer;
}

void wsky_SyntaxError_print(const SyntaxError *self, FILE *output) {
  char *s = wsky_SyntaxError_toString(self);
  fprintf(output, "%s\n", s);
  wsky_free(s);
}
