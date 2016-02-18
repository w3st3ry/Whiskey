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
typedef wsky_Function Function;


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
  .final = true,
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
  if (self->function)
    wsky_GC_VISIT(self->function);
}


static Method *new(Class *class, const char *name, wsky_MethodFlags flags,
                   Function *function) {
  ReturnValue r = wsky_Object_new(wsky_Method_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  Method *self = (Method *) r.v.v.objectValue;
  self->defClass = class;
  self->name = wsky_strdup(name);
  self->flags = flags;
  self->function = function;
  return self;
}


Method *wsky_Method_newFromC(const wsky_MethodDef *cMethod, Class *class) {
  if (cMethod->flags == wsky_MethodFlags_GET)
    assert(cMethod->parameterCount == 0);

  if (cMethod->flags == wsky_MethodFlags_SET)
    assert(cMethod->parameterCount == 1);

  Function *function = wsky_Function_newFromC(cMethod->name, cMethod);
  Method *self = new(class, cMethod->name, cMethod->flags, function);
  return self;
}

Method *wsky_Method_newFromWsky(Function *wskyMethod,
                                wsky_MethodFlags flags,
                                Class *class) {
  Method *self = new(class, wskyMethod->name, flags, wskyMethod);
  return self;
}

Method *wsky_Method_newFromWskyDefault(const char *name,
                                       wsky_MethodFlags flags,
                                       Class *class) {
  Method *self = new(class, name, flags, NULL);
  self->function = NULL;
  return self;
}


ReturnValue wsky_Method_call(Method *method,
                             Object *self,
                             unsigned parameterCount,
                             const Value *parameters) {
  assert(method->function);

  return wsky_Function_callSelf(method->function,
                                method->defClass, self,
                                parameterCount, parameters);
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
                                  const Value *parameters) {
  assert(method->function);
  assert(method->flags & wsky_MethodFlags_VALUE);
  if (method->function->node)
    abort();

  return wsky_MethodDef_callValue(&method->function->cMethod,
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
