#include "class.h"

#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "function.h"

static unsigned getMethodCount(wsky_Class *class) {
  unsigned i = 0;
  wsky_MethodDef *method = class->methodDefs;
  while (method->name) {
    method++;
    i++;
  }
  return i;
}

static void initClass(wsky_Class *class) {
  unsigned methodCount = getMethodCount(class);
  wsky_MethodList_init(&class->methods, methodCount);
  unsigned i;
  for (i = 0; i < methodCount; i++) {
    wsky_MethodList_add(&class->methods, &class->methodDefs[i]);
  }
}

static void freeClass(wsky_Class *class) {
  wsky_MethodList_free(&class->methods);
}

void wsky_init(void) {
  initClass(&wsky_String_CLASS);
  initClass(&wsky_Function_CLASS);
}

void wsky_free(void) {
  freeClass(&wsky_Function_CLASS);
  freeClass(&wsky_String_CLASS);
}
