#include "gc.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "objects/class.h"
#include "objects/module.h"
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

void wsky_GC_visitObject(void *objectVoid) {
  Object *object = (Object *) objectVoid;
  if (!object)
    return;

  if (object->gcMark)
    return;
  object->gcMark = true;

  wsky_Class_acceptGC(object);
}

void wsky_GC_visitValue(Value value) {
  if (value.type == wsky_Type_OBJECT) {
    wsky_GC_visitObject(value.v.objectValue);
  }
}

static void visitBuiltinClasses(void) {
  const wsky_ClassArray *classArray = wsky_getBuiltinClasses();
  for (size_t i = 0; i < classArray->count; i++)
    wsky_GC_visitObject(classArray->classes[i]);
}

static void visitModules(void) {
  wsky_ModuleList *modules = wsky_Module_getModules();
  while (modules) {
    wsky_GC_visitObject(modules->module);
    modules = modules->next;
  }
}

static void visitBuiltins(void) {
  visitBuiltinClasses();
  visitModules();
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
  assert(class);
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
    wsky_ObjectFields_free(&object->fields);
  }

  wsky_free(object);
}

static void collectUnmarked(void) {
  Object *object = firstObject;
  while (object) {
    Object *next = object->gcNext;
    if (!object->gcMark)
      destroy(object);
    object = next;
  }
}

void wsky_GC_collect() {
  visitBuiltins();
  collectUnmarked();
}

void wsky_GC_autoCollect(void) {
  wsky_GC_unmarkAll();
  wsky_eval_visitScopeStack();
  wsky_GC_collect();
}

void wsky_GC_deleteAll(void) {
  wsky_GC_unmarkAll();
  collectUnmarked();
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
