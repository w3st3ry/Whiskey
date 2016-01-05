#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "object.h"
#include "objects/class.h"
#include "class_def.h"


typedef wsky_Object Object;


static Object *firstObject = NULL;



void wsky_GC_register(Object *object) {
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
  Object *object = firstObject;

  while (object) {
    object->gcMark = false;
    object = object->gcNext;
  }
}

void wsky_GC__visit(void *objectVoid) {
  Object *object = (Object *) objectVoid;
  if (!object)
    return;

  if (object->gcMark)
    return;
  object->gcMark = true;

  wsky_Class_acceptGC(object);
}

void wsky_GC__visitValue(wsky_Value value) {
  if (value.type == wsky_Type_OBJECT) {
    wsky_GC_VISIT(value.v.objectValue);
  }
}



static void destroy(Object *object) {
  Object *next = object->gcNext;
  Object *previous = object->gcPrevious;

  if (!previous) {
    firstObject = next;
  }

  if (next) {
    next->gcPrevious = previous;
  }
  if (previous) {
    previous->gcNext = next;
  }

  wsky_Class *class = object->class;
  while (class != wsky_Object_CLASS) {
    /* printf("%s\n", class->name); */

    if (class->destructor)
      class->destructor(object);
    class = class->super;
  }
  free(object);
}

void wsky_GC_collect(void) {
  Object *object = firstObject;
  while (object) {
    Object *next = object->gcNext;
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
