#include "gc.h"

#include <stdlib.h>
#include "object.h"

void degat_GC_decrefObject(void *objectVoid) {
  degat_Object *object = (degat_Object *) objectVoid;
  object->gcReferenceCount--;
  if (!object->gcReferenceCount)
    {
      object->class->destructor(object);
      free(object);
      object = NULL;
    }
}

void degat_GC_incref(degat_Value value) {
  if (value.type != degat_Type_OBJECT)
    return;
  degat_Object_INCREF(value.v.objectValue);
}
void degat_GC_decref(degat_Value value) {
  if (value.type != degat_Type_OBJECT)
    return;
  degat_Object_DECREF(value.v.objectValue);
}
