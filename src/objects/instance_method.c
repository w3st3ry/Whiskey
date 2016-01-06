#include "objects/instance_method.h"

#include <stdlib.h>
#include "objects/str.h"


typedef wsky_MethodObject Method;
typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_InstanceMethod InstanceMethod;


static wsky_ReturnValue construct(Object *object,
                                  unsigned paramCount,
                                  Value *params);
static wsky_ReturnValue destroy(Object *object);


static ReturnValue toString(InstanceMethod *object, Value *value);


#define M(name, flags, paramCount)              \
  {#name, paramCount, flags, (void *) &name}

static wsky_MethodDef methods[] = {
  M(toString, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC, 0),
  {0, 0, 0, 0},
};

#undef M

const wsky_ClassDef wsky_InstanceMethod_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "InstanceMethod",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(InstanceMethod),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_InstanceMethod_CLASS;


InstanceMethod *wsky_InstanceMethod_new(Method *method,
                                        Value *self) {
  ReturnValue r = wsky_Object_new(wsky_InstanceMethod_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  InstanceMethod *instanceMethod = (InstanceMethod *) r.v.v.objectValue;
  instanceMethod->method = method;
  instanceMethod->self = *self;
  return instanceMethod;
}

static wsky_ReturnValue construct(Object *object,
                                  unsigned paramCount,
                                  Value *params) {
  (void) paramCount;
  (void) params;

  InstanceMethod *self = (InstanceMethod *) object;
  self->method = NULL;
  self->self = wsky_Value_NULL;
  wsky_RETURN_NULL;
}

static wsky_ReturnValue destroy(Object *object) {
  (void) object;
  wsky_RETURN_NULL;
}



static ReturnValue toString(InstanceMethod *object, Value *value) {
  (void) object;
  (void) value;
  wsky_RETURN_CSTRING("<InstanceMethod>");
}

bool wsky_isInstanceMethod(const Value value) {
  return value.type == wsky_Type_OBJECT &&
    wsky_getClass(value) == wsky_InstanceMethod_CLASS;
}
