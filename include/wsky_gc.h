#ifndef WSKY_GC_H_
# define WSKY_GC_H_

# include <stddef.h>
# include "value.h"

# define wsky_MALLOC(size) malloc(size)
# define wsky_MALLOC_ATOMIC(size) malloc(size)
# define wsky_REALLOC(data, size) realloc(data, size)
# define wsky_FREE(data) free(data)

# define wsky_STRDUP(string) strdup(string)
# define wsky_STRNDUP(string, length) strndup(string, length)


void wsky_GC_increfObject(void *object);
void wsky_GC_decrefObject(void *object);
void wsky_GC_xIncrefObject(void *object);
void wsky_GC_xDecrefObject(void *object);

void wsky_GC_increfValue(wsky_Value v);
void wsky_GC_decrefValue(wsky_Value v);


/* Error if `object` is `NULL` */
# define wsky_INCREF(object) (wsky_GC_increfObject(object))
# define wsky_DECREF(object) (wsky_GC_decrefObject(object))

/* Does nothing if `object` is `NULL` */
# define wsky_XINCREF(object) (wsky_GC_xIncrefObject(object))
# define wsky_XDECREF(object) (wsky_GC_xDecrefObject(object))

/* Error if `object` is `NULL` */
# define wsky_Value_INCREF(v) (wsky_GC_increfValue(v))
# define wsky_Value_DECREF(v) (wsky_GC_decrefValue(v))


#endif /* !WSKY_GC_H_ */
