#include <assert.h>
#include <string.h>
#include "whiskey_private.h"


const Position wsky_Position_UNKNOWN = {
  .file = NULL,
  .line = -1,
  .column = -1,
  .index = -1,
};

bool wsky_Position_isUnknown(const Position *position) {
  return (position->file == NULL &&
          position->index == -1);
}

bool wsky_Position_equals(const Position *a, const Position *b) {
  assert(a->file);
  assert(a->file == b->file);
  if (a->index == b->index) {
    assert(a->line == b->line);
    assert(a->column == b->column);
    return true;
  }
  return false;
}



void wsky_Position_print(const Position *self, FILE *output) {
  char *s = wsky_Position_toString(self);
  fprintf(output, "%s", s);
  wsky_free(s);
}

static char *toString(const char *fileName, int line, int column) {
  char *buffer = wsky_safeMalloc(strlen(fileName) + 40);
  sprintf(buffer, "%s:%d:%d:", fileName, line, column);
  return buffer;
}

char *wsky_Position_toString(const Position *self) {
  assert(self->file);
  const char *fileName = self->file->name;
  if (self->file->absolutePath)
    fileName = self->file->absolutePath;
  return toString(fileName, self->line, self->column);
}
