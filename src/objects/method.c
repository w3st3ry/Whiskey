#include "objects/method.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gc.h"


typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_MethodObject MethodObject;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue destroy(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

#undef M
#undef GET
#undef OP

const wsky_ClassDef wsky_MethodObject_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "MethodObject",
  .constructor = NULL,
  .destructor = &destroy,
  .objectSize = sizeof(MethodObject),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};


wsky_Class *wsky_MethodObject_CLASS;


static ReturnValue destroy(Object *object) {
  MethodObject *self = (MethodObject *) object;
  /*printf("Destroying method %s\n", self->name);*/
  wsky_FREE(self->name);
  wsky_RETURN_NULL;
}


static MethodObject *new() {
  ReturnValue r = wsky_Object_new(wsky_MethodObject_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  return (MethodObject *) r.v.v.objectValue;
}


MethodObject *wsky_MethodObject_newFromC(wsky_MethodDef *cMethod) {
  if (cMethod->flags == wsky_MethodFlags_GET) {
    assert(cMethod->parameterCount == 0);
  }
  if (cMethod->flags == wsky_MethodFlags_SET) {
    assert(cMethod->parameterCount == 1);
  }
  MethodObject *self = new();
  self->name = wsky_STRDUP(cMethod->name);
  self->flags = cMethod->flags;
  self->cMethod = *cMethod;
  self->wskyMethod = NULL;
  return self;
}

MethodObject *wsky_MethodObject_newFromWsky(wsky_Function *wskyMethod,
                                            wsky_MethodFlags flags) {
  MethodObject *self = new();
  self->name = wsky_STRDUP(wskyMethod->name);
  self->flags = flags;
  self->wskyMethod = wskyMethod;
  return self;
}


ReturnValue wsky_MethodObject_call(MethodObject *method,
                                   Object *self,
                                   unsigned parameterCount,
                                   Value *parameters) {
  if (method->wskyMethod) {
    return wsky_Function_call(method->wskyMethod,
                              self, parameterCount, parameters);
  }
  return wsky_MethodDef_call(&method->cMethod,
                             self, parameterCount, parameters);
}


ReturnValue wsky_MethodObject_call0(MethodObject *method,
                                    Object *self) {
  return wsky_MethodObject_call(method, self, 0, NULL);
}


ReturnValue wsky_MethodObject_call1(MethodObject *method,
                                    Object *self,
                                    Value a) {
  return wsky_MethodObject_call(method, self, 1, &a);
}
