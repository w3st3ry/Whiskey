#ifndef GC_H_
# define GC_H_

#include "value.h"

void wsky_GC_increfObject(void *object);
void wsky_GC_decrefObject(void *object);
void wsky_GC_xIncrefObject(void *object);
void wsky_GC_xDecrefObject(void *object);

void wsky_GC_increfValue(wsky_Value v);
void wsky_GC_decrefValue(wsky_Value v);


/* Error if `object` is `NULL` */
/*# define wsky_INCREF(object) ((object)->gcReferenceCount++)*/
# define wsky_INCREF(object) (wsky_GC_increfObject(object))
# define wsky_DECREF(object) (wsky_GC_decrefObject(object))

/* Does nothing if `object` is `NULL` */
# define wsky_XINCREF(object) (wsky_GC_xIncrefObject(object))
# define wsky_XDECREF(object) (wsky_GC_xDecrefObject(object))

/* Error if `object` is `NULL` */
# define wsky_Value_INCREF(v) (wsky_GC_increfValue(v))
# define wsky_Value_DECREF(object) (wsky_GC_decrefValue(v))


#endif /* !GC_H_ */
