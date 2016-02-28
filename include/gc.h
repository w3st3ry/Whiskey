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
# define wsky_free(data) free(data)


void wsky_GC_unmarkAll(void);
void wsky_GC_collect(void);

void wsky_GC_register(wsky_Object *object);

void wsky_GC__visit(void *object);
#define wsky_GC_VISIT(object) wsky_GC__visit(object)

void wsky_GC__visitValue(wsky_Value v);
#define wsky_GC_VISIT_VALUE(object) wsky_GC__visitValue(object)

void wsky_GC_visitBuiltins(void);


#endif /* !WSKY_GC_H_ */
