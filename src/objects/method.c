#include "objects/method.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gc.h"


typedef wsky_Class Class;
typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_Method Method;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

#undef M
#undef GET
#undef OP

const wsky_ClassDef wsky_Method_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Method",
  .constructor = NULL,
  .destructor = &destroy,
  .objectSize = sizeof(Method),
  .methodDefs = methods,
  .gcAcceptFunction = &acceptGC,
};


Class *wsky_Method_CLASS;


static ReturnValue destroy(Object *object) {
  Method *self = (Method *)object;
  /*printf("Destroying method %s\n", self->name);*/
  wsky_free(self->name);
  wsky_RETURN_NULL;
}

static void acceptGC(Object *object) {
  Method *self = (Method *)object;
  wsky_GC_VISIT(self->defClass);
  if (self->wskyMethod)
    wsky_GC_VISIT(self->wskyMethod);
}


static Method *new(Class *class, const char *name, wsky_MethodFlags flags) {
  ReturnValue r = wsky_Object_new(wsky_Method_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  Method *self = (Method *) r.v.v.objectValue;
  self->defClass = class;
  self->name = wsky_strdup(name);
  self->flags = flags;
  return self;
}


Method *wsky_Method_newFromC(wsky_MethodDef *cMethod, Class *class) {
  if (cMethod->flags == wsky_MethodFlags_GET)
    assert(cMethod->parameterCount == 0);

  if (cMethod->flags == wsky_MethodFlags_SET)
    assert(cMethod->parameterCount == 1);

  Method *self = new(class, cMethod->name, cMethod->flags);
  self->cMethod = *cMethod;
  self->wskyMethod = NULL;
  return self;
}

Method *wsky_Method_newFromWsky(wsky_Function *wskyMethod,
                                wsky_MethodFlags flags,
                                Class *class) {
  Method *self = new(class, wskyMethod->name, flags);
  self->wskyMethod = wskyMethod;
  self->cMethod.function = NULL;
  return self;
}

Method *wsky_Method_newFromWskyDefault(const char *name,
                                       wsky_MethodFlags flags,
                                       Class *class) {
  Method *self = new(class, name, flags);
  self->wskyMethod = NULL;
  self->cMethod.function = NULL;
  return self;
}


ReturnValue wsky_Method_call(Method *method,
                             Object *self,
                             unsigned parameterCount,
                             Value *parameters) {
  if (method->wskyMethod)
    return wsky_Function_call(method->wskyMethod,
                              method->defClass, self,
                              parameterCount, parameters);

  assert(method->cMethod.function);

  return wsky_MethodDef_call(&method->cMethod,
                             self, parameterCount, parameters);
}

ReturnValue wsky_Method_call0(Method *method,
                              Object *self) {
  return wsky_Method_call(method, self, 0, NULL);
}

ReturnValue wsky_Method_call1(Method *method,
                              Object *self,
                              Value a) {
  return wsky_Method_call(method, self, 1, &a);
}



ReturnValue wsky_Method_callValue(Method *method,
                                  Value self,
                                  unsigned parameterCount,
                                  Value *parameters) {
  if (method->wskyMethod) {
    abort();
  }
  return wsky_MethodDef_callValue(&method->cMethod,
                                  self, parameterCount, parameters);
}

ReturnValue wsky_Method_callValue0(Method *method,
                                   Value self) {
  return wsky_Method_callValue(method, self, 0, NULL);
}

ReturnValue wsky_Method_callValue1(Method *method,
                                   Value self,
                                   Value a) {
  return wsky_Method_callValue(method, self, 1, &a);
}
