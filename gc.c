#include "wsky_gc.h"

#include <stdlib.h>
#include <string.h>
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
      wsky_FREE(object);
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
  wsky_XINCREF(v.v.objectValue);
}

void wsky_GC_decrefValue(wsky_Value v) {
  if (v.type != wsky_Type_OBJECT || !v.v.objectValue)
    return;
  wsky_XDECREF(v.v.objectValue);
}


char *wsky__strdup(const char *string) {
  size_t length = strlen(string);
  char *newString = wsky_MALLOC(length + 1);
  strcpy(newString, string);
  return newString;
}

char *wsky__strndup(const char *string, size_t maximum) {
  size_t length = strlen(string);
  if (maximum < length)
    length = maximum;
  char *newString = wsky_MALLOC(length + 1);
  strncpy(newString, string, length);
  newString[length] = '\0';
  return newString;
}
