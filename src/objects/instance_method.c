#include "objects/instance_method.h"

#include <stdlib.h>
#include "../return_value_private.h"
#include "objects/str.h"
#include "gc.h"


typedef wsky_Method Method;
typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_InstanceMethod InstanceMethod;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);

static ReturnValue toString(InstanceMethod *object);


#define M(name, flags, paramCount)                      \
  {#name, paramCount, flags, (wsky_Method0)&name}

static wsky_MethodDef methods[] = {
  M(toString, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC, 0),
  {0, 0, 0, 0},
};

#undef M

const wsky_ClassDef wsky_InstanceMethod_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "InstanceMethod",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(InstanceMethod),
  .methodDefs = methods,
  .gcAcceptFunction = &acceptGC,
};

wsky_Class *wsky_InstanceMethod_CLASS;


InstanceMethod *wsky_InstanceMethod_new(Method *method, Value self) {
  ReturnValue r = wsky_Object_new(wsky_InstanceMethod_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  InstanceMethod *instanceMethod = (InstanceMethod *) r.v.v.objectValue;
  instanceMethod->method = method;
  instanceMethod->self = self;

  return instanceMethod;
}

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  (void) paramCount;
  (void) params;

  InstanceMethod *self = (InstanceMethod *) object;
  self->method = NULL;
  self->self = wsky_Value_NULL;
  RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  (void) object;
  RETURN_NULL;
}

static void acceptGC(Object *object) {
  InstanceMethod *self = (InstanceMethod *) object;
  wsky_GC_visitObject(self->method);
  wsky_GC_visitValue(self->self);
}



static ReturnValue toString(InstanceMethod *object) {
  (void) object;
  RETURN_C_STRING("<InstanceMethod>");
}

bool wsky_isInstanceMethod(const Value value) {
  return value.type == wsky_Type_OBJECT &&
    wsky_getClass(value) == wsky_InstanceMethod_CLASS;
}
