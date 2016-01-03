#include "class_def.h"

#include <stdlib.h>
#include <string.h>
#include "objects/null.h"
#include "objects/boolean.h"
#include "objects/integer.h"
#include "objects/float.h"
#include "objects/str.h"
#include "objects/function.h"
#include "objects/instance_method.h"
#include "gc.h"


typedef wsky_ClassDef ClassDef;
typedef wsky_MethodDef MethodDef;

/*
static unsigned getMethodCount(ClassDef *class) {
  unsigned i = 0;
  MethodDef *method = class->methodDefs;
  while (method->name) {
    method++;
    i++;
  }
  return i;
}

const MethodDef *wsky_Class_findLocalMethod(const Class *class,
                                            const char *methodName) {
  const MethodList *methods = &class->methods;
  unsigned i;
  for (i = 0; i < methods->count; i++) {
    const MethodDef *method = methods->methods + i;
    if (strcmp(method->name, methodName) == 0) {
      return method;
    }
  }
  return NULL;
}


const MethodDef *wsky_Class_findMethod(const Class *class,
                                       const char *methodName) {
  const MethodDef *method;
  method = wsky_Class_findLocalMethod(class, methodName);
  if (method) {
    return method;
  }
*/
/* Don't search methods in the superclass of theses special classes *//*
  if (class == &wsky_Object_CLASS ||
      class == &wsky_Boolean_CLASS ||
      class == &wsky_Integer_CLASS ||
      class == &wsky_Float_CLASS) {
    return NULL;
  }
  return wsky_Class_findMethod(class->super, methodName);
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
  &wsky_Object_CLASS,
  &wsky_InstanceMethod_CLASS,
  &wsky_Scope_CLASS,
  &wsky_Function_CLASS,

  &wsky_Null_CLASS,
  &wsky_Boolean_CLASS,
  &wsky_Integer_CLASS,
  &wsky_Float_CLASS,
  &wsky_String_CLASS,
  NULL,
};

void wsky_start(void) {
  Class **class = CLASSES;
  while (*class) {
    initClass(*class);
    class++;
  }
}

void wsky_stop(void) {
  wsky_GC_unmarkAll();
  wsky_GC_collect();

  Class **class = CLASSES;
  while (*class) {
    freeClass(*class);
    class++;
  }
}
*/
