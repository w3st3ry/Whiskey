#include "gc.h"

#include <stdlib.h>
#include "object.h"


void degat_GC_increfObject(void *objectVoid) {
  degat_Object *object = (degat_Object *) objectVoid;
  if (!object)
    abort();
  object->gcReferenceCount++;
}

void degat_GC_decrefObject(void *objectVoid) {
  degat_Object *object = (degat_Object *) objectVoid;
  if (!object)
    abort();
  object->gcReferenceCount--;
  if (!object->gcReferenceCount)
    {
      object->class->destructor(object);
      free(object);
      object = NULL;
    }
}


void degat_GC_xIncrefObject(void *object) {
  if (!object)
    return;
  degat_INCREF(object);
}

void degat_GC_xDecrefObject(void *object) {
  if (!object)
    return;
  degat_DECREF(object);
}


void degat_GC_increfValue(degat_Value v) {
  if (v.type != degat_Type_OBJECT)
    abort();
  degat_INCREF(v.v.objectValue);
}

void degat_GC_decrefValue(degat_Value v) {
  if (v.type != degat_Type_OBJECT)
    abort();
  degat_DECREF(v.v.objectValue);
}
