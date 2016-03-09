#ifndef MEMORY_H
# define MEMORY_H

# include <stdlib.h>

/** Like malloc() */
static inline void *wsky_malloc(size_t size) {
  return malloc(size);
}

/** Use wsky_saveMalloc() instead. */
void *wsky__safeMallocImpl(size_t size, const char *file, int line);

/** Like wsky_malloc(), but never returns NULL. */
#define wsky_safeMalloc(size) wsky__safeMallocImpl(size, __FILE__, __LINE__)

/** Like realloc() */
# define wsky_realloc(data, size) realloc(data, size)

/** Like free() */
static inline void wsky_free(void *data) {
  free(data);
}

#endif /* MEMORY_H */
