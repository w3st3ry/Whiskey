#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "objects/class.h"
#include "class_def.h"


typedef wsky_Object Object;
typedef wsky_Value Value;


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

void wsky_GC__visitValue(Value value) {
  if (value.type == wsky_Type_OBJECT) {
    wsky_GC_VISIT(value.v.objectValue);
  }
}


static void freeObjectField(const char *name, void *value) {
  (void) name;
  free(value);
}

static void destroy(Object *object) {
  Object *next = object->gcNext;
  Object *previous = object->gcPrevious;

  if (!previous)
    firstObject = next;

  if (next)
    next->gcPrevious = previous;
  if (previous)
    previous->gcNext = next;

  wsky_Class *class = object->class;
  while (class != wsky_Object_CLASS) {
    if (class->destructor)
      class->destructor(object);
    /*
    else
      printf("No destructor\n");
    */
    class = class->super;
  }

  if (!object->class->native) {
    wsky_Dict_apply(&object->fields, freeObjectField);
    wsky_Dict_free(&object->fields);
  }

  wsky_free(object);
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



void *wsky__safeMallocImpl(size_t size, const char *file, int line) {
  void *data = wsky_malloc(size);
  if (data)
    return data;
  fprintf(stderr,
          "%s:%d: malloc() returned NULL."
          "You are probably running out of memory\n",
          file, line);
  abort();
}


char *wsky_strdup(const char *string) {
  size_t length = strlen(string);
  char *newString = wsky_malloc(length + 1);
  if (!newString)
    return NULL;
  strcpy(newString, string);
  return newString;
}

char *wsky_strndup(const char *string, size_t maximum) {
  size_t length = strlen(string);
  if (maximum < length)
    length = maximum;
  char *newString = wsky_malloc(length + 1);
  if (!newString)
    return NULL;
  strncpy(newString, string, length);
  newString[length] = '\0';
  return newString;
}
