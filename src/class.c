#include "class.h"

#include <stdlib.h>
#include <string.h>
#include "objects/null.h"
#include "objects/function.h"
#include "objects/instance_method.h"
#include "objects/str.h"
#include "gc.h"


typedef wsky_Class Class;
typedef wsky_MethodDef MethodDef;
typedef wsky_MethodList MethodList;


static unsigned getMethodCount(Class *class) {
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
  if (class == &wsky_Object_CLASS) {
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
  &wsky_Null_CLASS,
  &wsky_Function_CLASS,
  &wsky_Scope_CLASS,
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
