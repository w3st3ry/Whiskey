#ifndef GC_H_
# define GC_H_

#include "value.h"

void degat_GC_increfObject(void *object);
void degat_GC_decrefObject(void *object);
void degat_GC_xIncrefObject(void *object);
void degat_GC_xDecrefObject(void *object);

void degat_GC_increfValue(degat_Value v);
void degat_GC_decrefValue(degat_Value v);


/* Error if `object` is `NULL` */
/*# define degat_INCREF(object) ((object)->gcReferenceCount++)*/
# define degat_INCREF(object) (degat_GC_increfObject(object))
# define degat_DECREF(object) (degat_GC_decrefObject(object))

/* Does nothing if `object` is `NULL` */
# define degat_XINCREF(object) (degat_GC_xIncrefObject(object))
# define degat_XDECREF(object) (degat_GC_xDecrefObject(object))

/* Error if `object` is `NULL` */
# define degat_Value_INCREF(v) (degat_GC_increfValue(v))
# define degat_Value_DECREF(object) (degat_GC_decrefValue(v))


#endif /* !GC_H_ */
