#include "class.h"

#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "function.h"
#include "gc.h"


typedef wsky_Class Class;


static unsigned getMethodCount(Class *class) {
  unsigned i = 0;
  wsky_MethodDef *method = class->methodDefs;
  while (method->name) {
    method++;
    i++;
  }
  return i;
}

static void initClass(Class *class) {
  unsigned methodCount = getMethodCount(class);
  wsky_MethodList_init(&class->methods, methodCount);
  unsigned i;
  for (i = 0; i < methodCount; i++) {
    wsky_MethodList_add(&class->methods, &class->methodDefs[i]);
  }
}

static void freeClass(Class *class) {
  wsky_MethodList_free(&class->methods);
}

static Class *CLASSES[] = {
  &wsky_Function_CLASS,
  &wsky_Scope_CLASS,
  &wsky_String_CLASS,
  NULL,
};

void wsky_init(void) {
  Class **class = CLASSES;
  while (*class) {
    initClass(*class);
    class++;
  }
}

void wsky_free(void) {
  wsky_GC_unmarkAll();
  wsky_GC_collect();

  Class **class = CLASSES;
  while (*class) {
    freeClass(*class);
    class++;
  }
}
