#ifndef GC_H_
# define GC_H_

#include "value.h"

void degat_GC_decrefObject(void *object);
void degat_GC_incref(degat_Value value);
void degat_GC_decref(degat_Value value);

# define degat_Object_INCREF(object_) ((object_)->gcReferenceCount++)
# define degat_Object_DECREF(object_) (degat_GC_decrefObject(object_))

# define degat_INCREF(value_) (degat_GC_incref(value))
# define degat_DECREF(value_) (degat_GC_decref(value))

#endif /* !GC_H_ */
