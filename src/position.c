#include "position.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "gc.h"
#include "objects/program_file.h"

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
  wsky_free(s);
}

char *wsky_Position_toString(const wsky_Position *self) {
  const char *fileName = self->file->name;
  if (self->file->path)
    fileName = self->file->path;
  char *buffer = wsky_safeMalloc(strlen(fileName) + 20);
  sprintf(buffer, "%s:%d:%d:", fileName, self->line, self->column);
  return buffer;
}
