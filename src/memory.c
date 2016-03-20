#include "whiskey.h"

void *wsky__safeMallocImpl(size_t size, const char *file, int line) {
  void *data = wsky_malloc(size);
  if (data)
    return data;
  fprintf(stderr,
          "%s:%d: malloc() returned NULL."
          "You are probably running out of memory\n",
          file, line);
  abort();
}
