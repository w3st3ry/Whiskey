#include "gc.h"

#include <stdlib.h>
#include "object.h"


void wsky_GC_increfObject(void *objectVoid) {
  wsky_Object *object = (wsky_Object *) objectVoid;
  if (!object)
    abort();
  object->gcReferenceCount++;
}

void wsky_GC_decrefObject(void *objectVoid) {
  wsky_Object *object = (wsky_Object *) objectVoid;
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


void wsky_GC_xIncrefObject(void *object) {
  if (!object)
    return;
  wsky_INCREF(object);
}

void wsky_GC_xDecrefObject(void *object) {
  if (!object)
    return;
  wsky_DECREF(object);
}


void wsky_GC_increfValue(wsky_Value v) {
  if (v.type != wsky_Type_OBJECT)
    return;
  wsky_INCREF(v.v.objectValue);
}

void wsky_GC_decrefValue(wsky_Value v) {
  if (v.type != wsky_Type_OBJECT || !v.v.objectValue)
    return;
  wsky_DECREF(v.v.objectValue);
}
