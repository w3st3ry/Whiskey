#ifndef WSKY_GC_H_
# define WSKY_GC_H_

# include <stddef.h>
# include "value.h"
# include "objects/scope.h"

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


char *wsky_strdup(const char *string);
char *wsky_strndup(const char *string, size_t maximum);


void wsky_GC_unmarkAll(void);

void wsky_GC_collect(void);

void wsky_GC_deleteAll(void);

void wsky_GC_autoCollect(void);

void wsky_GC_register(wsky_Object *object);

void wsky_GC_visitObject(void *object);

void wsky_GC_visitValue(wsky_Value v);


#endif /* !WSKY_GC_H_ */
