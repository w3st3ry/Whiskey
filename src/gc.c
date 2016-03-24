#include <setjmp.h>
#include <assert.h>
#include <string.h>
#include "heaps.h"


void wsky_GC_unmarkAll(void) {
  wsky_heaps_unmark();
}

void wsky_GC_visitObject(void *objectVoid) {
  Object *object = (Object *) objectVoid;
  if (!object)
    return;

  assert(wsky_heaps_contains(object));

  if (object->_gcMark)
    return;
  object->_gcMark = true;

  if (object->_initialized)
    wsky_Class_acceptGC(object);
}

void wsky_GC_visitValue(Value value) {
  if (value.type == Type_OBJECT) {
    wsky_GC_visitObject(value.v.objectValue);
  }
}

static void visitBuiltinClasses(void) {
  const wsky_ClassArray *classArray = wsky_getBuiltinClasses();
  for (size_t i = 0; i < classArray->count; i++)
    wsky_GC_visitObject(classArray->classes[i]);
}

static void visitModules(void) {
  ModuleList *modules = wsky_Module_getModules();
  while (modules) {
    wsky_GC_visitObject(modules->module);
    modules = modules->next;
  }
}

static void visitBuiltins(void) {
  visitBuiltinClasses();
  visitModules();
}

static void visitObjectArray(void *pointers_, size_t size) {
  Object **pointers = (Object **)pointers_;
  ptrdiff_t s = (ptrdiff_t)size;
  while (s > 0) {
    if (wsky_heaps_contains(*pointers)) {
      assert((*pointers)->class);
      wsky_GC_visitObject(*pointers);
    }
    pointers++;
    s -= sizeof(Object *);
  }
}

/*
static void visitObjectArray(void *pointers_, size_t size) {
  char *pointers = (char *)pointers_;
  while (size--) {
    Object *object = *(Object **)pointers;
    if (wsky_heaps_contains(object)) {
      assert(object->class);
      wsky_GC_visitObject(object);
    }
    pointers++;
  }
}
*/

static void visitObjectPointers(void *start, void *end) {
  if (start > end) {
    void *tmp = start;
    start = end;
    end = tmp;
  }
  size_t size = (size_t)((char *)end - (char *)start + 1);
  visitObjectArray(start, size);
}

static void visitRegisters(void) {
  /* All registers must be saved into jmp_buf. */
  jmp_buf registers;
  setjmp(registers);

  /*
  size_t count = sizeof(jmp_buf) / sizeof(Object **);
  visitObjectArray((Object **) registers, count);
  */
  visitObjectArray((Object **) registers, sizeof(jmp_buf));
}

static void *stackStart = NULL;

static void visitStack(void) {
  assert(stackStart);
  void *op = NULL;
  void **stackEnd = &op;
  assert(stackEnd);
  assert(stackStart != stackEnd);
  assert(wsky_heaps_contains(wsky_Object_CLASS));
  assert(wsky_heaps_contains(wsky_Exception_CLASS));
  assert(wsky_heaps_contains(wsky_String_CLASS));
  visitObjectPointers(stackStart, (void *)stackEnd);
}


void wsky_GC_initImpl(void *stackStart_) {
  stackStart = stackStart_;
}



void wsky_GC_collect(void) {
  visitBuiltins();
  visitRegisters();
  visitStack();
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

static bool stressed = false;

void wsky_GC_setStressed(bool stressed_) {
  stressed = stressed_;
}

bool wsky_GC_isStressed(void) {
  return stressed;
}

void wsky_GC_requestCollection(void) {
  if (wsky_GC_isStressed()) {
    wsky_GC_autoCollect();
  }
}
