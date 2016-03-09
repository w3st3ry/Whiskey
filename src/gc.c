#include "gc.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "objects/class.h"
#include "objects/module.h"
#include "class_def.h"
#include "heap.h"


typedef wsky_Value Value;

void wsky_GC_unmarkAll(void) {
  wsky_heaps_unmark();
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



void wsky_GC_collect() {
  visitBuiltins();
  wsky_heaps_deleteUnmarkedObjects();
}

void wsky_GC_autoCollect(void) {
  wsky_GC_unmarkAll();
  wsky_eval_visitScopeStack();
  wsky_GC_collect();
}

void wsky_GC_deleteAll(void) {
  wsky_GC_unmarkAll();
  wsky_heaps_deleteUnmarkedObjects();
  wsky_heaps_free();
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
