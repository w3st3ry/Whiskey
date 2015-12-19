#ifndef WSKY_GC_H_
# define WSKY_GC_H_

# include <gc.h>
# include "value.h"

char *wsky__strdup(const char *string);
char *wsky__strndup(const char *string, size_t n);

# define wsky_MALLOC(size) GC_MALLOC(size)
# define wsky_MALLOC_ATOMIC(size) GC_MALLOC_ATOMIC(size)
# define wsky_REALLOC(data, size) GC_REALLOC(data, size)
//# define wsky_FREE(data) GC_FREE(data)
# define wsky_FREE(data) ((void) data)

# define wsky_STRDUP(string) wsky__strdup(string)
# define wsky_STRNDUP(string, length) wsky__strndup(string, length)


void wsky_GC_increfObject(void *object);
void wsky_GC_decrefObject(void *object);
void wsky_GC_xIncrefObject(void *object);
void wsky_GC_xDecrefObject(void *object);

void wsky_GC_increfValue(wsky_Value v);
void wsky_GC_decrefValue(wsky_Value v);


/* Error if `object` is `NULL` */
//# define wsky_INCREF(object) (wsky_GC_increfObject(object))
//# define wsky_DECREF(object) (wsky_GC_decrefObject(object))

/* Does nothing if `object` is `NULL` */
//# define wsky_XINCREF(object) (wsky_GC_xIncrefObject(object))
//# define wsky_XDECREF(object) (wsky_GC_xDecrefObject(object))

/* Error if `object` is `NULL` */
//# define wsky_Value_INCREF(v) (wsky_GC_increfValue(v))
//# define wsky_Value_DECREF(v) (wsky_GC_decrefValue(v))

# define wsky_INCREF(object) ((void) object)
# define wsky_DECREF(object) ((void) object)

# define wsky_XINCREF(object) ((void) object)
# define wsky_XDECREF(object) ((void) object)

# define wsky_Value_INCREF(v) ((void) v)
# define wsky_Value_DECREF(v) ((void) v)


#endif /* !WSKY_GC_H_ */
