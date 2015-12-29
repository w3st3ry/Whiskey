#include "position.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "gc.h"

bool wsky_Position_equals(const wsky_Position *a, const wsky_Position *b) {
  assert(a->file == b->file);
  if (a->index == b->index) {
    assert(a->line == b->line);
    assert(a->column == b->column);
    return true;
  }
  return false;
}



void wsky_Position_print(const wsky_Position *self, FILE *output) {
  char *s = wsky_Position_toString(self);
  fprintf(output, "%s", s);
  wsky_FREE(s);
}

char *wsky_Position_toString(const wsky_Position *self) {
  const char *file = "<unknown file>";
  if (self->file)
    file = self->file->path;
  char *buffer = wsky_MALLOC(strlen(file) + 20);
  sprintf(buffer, "%s:%d:%d:", file, self->line, self->column);
  return buffer;
}
