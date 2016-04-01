#include <string.h>
#include <assert.h>
#include "../whiskey_private.h"


static Result destroy(Object *object);

static void acceptGC(Object *object);



static MethodDef methods[] = {
  {0, 0, 0, 0},
};

#undef M
#undef GET
#undef OP

const ClassDef wsky_Method_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Method",
  .final = true,
  .constructor = NULL,
  .privateConstructor = true,
  .destructor = &destroy,
  .methodDefs = methods,
  .gcAcceptFunction = &acceptGC,
};


Class *wsky_Method_CLASS;


static Result destroy(Object *object) {
  Method *self = (Method *)object;
  /*printf("Destroying method %s\n", self->name);*/
  wsky_free(self->name);
  RETURN_NULL;
}

static void acceptGC(Object *object) {
  Method *self = (Method *)object;
  wsky_GC_visitObject(self->defClass);
  wsky_GC_visitObject(self->function);
}


static Method *new(Class *class, const char *name, MethodFlags flags,
                   Function *function) {
  Result r = wsky_Object_new(wsky_Method_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  Method *self = (Method *) r.v.v.objectValue;
  self->defClass = class;
  self->name = wsky_strdup(name);
  self->flags = flags;
  self->function = function;
  return self;
}


Method *wsky_Method_newFromC(const MethodDef *cMethod, Class *class) {
  if (cMethod->flags == wsky_MethodFlags_GET)
    assert(cMethod->parameterCount == 0);

  if (cMethod->flags == wsky_MethodFlags_SET)
    assert(cMethod->parameterCount == 1);

  Function *function = wsky_Function_newFromC(cMethod);
  Method *self = new(class, cMethod->name, cMethod->flags, function);
  return self;
}

Method *wsky_Method_newFromWsky(Function *wskyMethod,
                                MethodFlags flags,
                                Class *class) {
  Method *self = new(class, wskyMethod->name, flags, wskyMethod);
  return self;
}

Method *wsky_Method_newFromWskyDefault(const char *name,
                                       MethodFlags flags,
                                       Class *class) {
  Method *self = new(class, name, flags, NULL);
  self->function = NULL;
  return self;
}


Result wsky_Method_call(Method *method,
                             Object *self,
                             unsigned parameterCount,
                             const Value *parameters) {
  assert(method->function);

  return wsky_Function_callSelf(method->function,
                                method->defClass, self,
                                parameterCount, parameters);
}

Result wsky_Method_call0(Method *method,
                              Object *self) {
  return wsky_Method_call(method, self, 0, NULL);
}

Result wsky_Method_call1(Method *method,
                              Object *self,
                              Value a) {
  return wsky_Method_call(method, self, 1, &a);
}



Result wsky_Method_callValue(Method *method,
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

Result wsky_Method_callValue0(Method *method,
                                   Value self) {
  return wsky_Method_callValue(method, self, 0, NULL);
}

Result wsky_Method_callValue1(Method *method,
                                   Value self,
                                   Value a) {
  return wsky_Method_callValue(method, self, 1, &a);
}
