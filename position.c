#include "position.h"

#include <string.h>
#include <stdlib.h>

bool wsky_Position_equals(const wsky_Position *a, const wsky_Position *b) {
  if (a->file != b->file)
    abort();
  if (a->index == b->index) {
    if ((a->line != b->line) || (a->column != b->column)) {
      abort();
    }
    return true;
  }
  return false;
}



void wsky_Position_print(const wsky_Position *this, FILE *output) {
  char *s = wsky_Position_toString(this);
  fprintf(output, "%s", s);
  free(s);
}

char *wsky_Position_toString(const wsky_Position *this) {
  const char *file = "<unknown file>";
  if (this->file)
    file = this->file->path;
  char *buffer = malloc(strlen(file) + 20);
  sprintf(buffer, "%s:%d:%d:", file, this->line, this->column);
  return buffer;
}
