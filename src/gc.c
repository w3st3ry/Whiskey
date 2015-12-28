#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include "object.h"
#include <stdio.h>



static wsky_Object *firstObject = NULL;



void wsky_GC_register(wsky_Object *object) {
  object->gcPrevious = NULL;
  object->gcNext = firstObject;

  if (!firstObject) {
    firstObject = object;
    return;
  }

  firstObject->gcPrevious = object;
  firstObject = object;
}


void wsky_GC_unmarkAll(void) {
  wsky_Object *object = firstObject;

  while (object) {
    object->gcMark = false;
    object = object->gcNext;
  }
}

void wsky_GC__visit(void *objectVoid) {
  wsky_Object *object = (wsky_Object *) objectVoid;
  if (!object)
    return;

  if (object->gcMark)
    return;
  object->gcMark = true;

  wsky_GCAcceptFunction acceptFunction = object->class->gcAcceptFunction;
  if (acceptFunction) {
    acceptFunction(object);
  }
}

void wsky_GC__visitValue(wsky_Value value) {
  if (value.type == wsky_Type_OBJECT) {
    wsky_GC_VISIT(value.v.objectValue);
  }
}



static void destroy(wsky_Object *object) {
  wsky_Object *next = object->gcNext;
  wsky_Object *previous = object->gcPrevious;

  if (!previous) {
    firstObject = next;
  }

  if (next) {
    next->gcPrevious = previous;
  }
  if (previous) {
    previous->gcNext = next;
  }

  const wsky_Class *class = object->class;
  while (class != &wsky_Object_CLASS) {
    /* printf("%s\n", class->name); */
    class->destructor(object);
    class = class->super;
  }
  free(object);
}

void wsky_GC_collect(void) {
  wsky_Object *object = firstObject;
  while (object) {
    wsky_Object *next = object->gcNext;
    if (!object->gcMark)
      destroy(object);
    object = next;
  }
}



/*
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
*/
