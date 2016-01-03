#include "object.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "return_value.h"
#include "class_def.h"
#include "objects/class.h"
#include "objects/null.h"
#include "objects/str.h"
#include "objects/not_implemented_error.h"
#include "gc.h"


typedef wsky_Object Object;
typedef wsky_Class Class;
typedef wsky_ClassDef ClassDef;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_MethodDef MethodDef;
typedef wsky_MethodObject MethodObject;


static ReturnValue toString(Object *self) {
  static char buffer[100];
  const Class *class = self->class;
  snprintf(buffer, 90, "<%s>", class->name);
  wsky_RETURN_CSTRING(buffer);
}

#define OP(name)                                                        \
  static ReturnValue operator##name(Object *self, Value *value) {       \
    (void) self;                                                        \
    (void) value;                                                       \
    wsky_NotImplementedError *e;                                        \
    e = wsky_NotImplementedError_new("Not implemented");                \
    wsky_RETURN_EXCEPTION(e);                                           \
  }

#define ROP(name) OP(name) OP(R##name)
ROP(Plus)
ROP(Minus)
ROP(Slash)
ROP(Star)
#undef ROP
#undef OP

static MethodDef methodsDefs[] = {
  {"toString", 0, wsky_MethodFlags_GET, (void *) *toString},

  {"operator +", 1, 0, (void *) *operatorPlus},
  {"operator -", 1, 0, (void *) *operatorMinus},
  {"operator *", 1, 0, (void *) *operatorStar},
  {"operator /", 1, 0, (void *) *operatorSlash},

  {"operator r+", 1, 0, (void *) *operatorRPlus},
  {"operator r-", 1, 0, (void *) *operatorRMinus},
  {"operator r*", 1, 0, (void *) *operatorRStar},
  {"operator r/", 1, 0, (void *) *operatorRSlash},
  {0, 0, 0, 0},
};

const ClassDef wsky_Object_CLASS_DEF = {
  .super = NULL,
  .name = "Object",
  .constructor = NULL,
  .destructor = NULL,
  .objectSize = sizeof(Object),
  .methodDefs = methodsDefs,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_Object_CLASS;



ReturnValue wsky_Object_new(Class *class,
                            unsigned paramCount,
                            Value *params) {
  Object *object = wsky_MALLOC(class->objectSize);
  if (!object)
    return wsky_ReturnValue_NULL;
  object->class = class;

  ReturnValue rv = wsky_MethodObject_call(class->constructor,
                                          object, paramCount, params);
  if (rv.exception) {
    return rv;
  }

  wsky_GC_register(object);

  wsky_RETURN_OBJECT(object);
}


#define GET_CLASS(object) ((object) ? object->class : wsky_Null_CLASS)


MethodObject *wsky_Object_findMethod(wsky_Object *object, const char *name) {
  Class *class = GET_CLASS(object);
  if (!wsky_Dict_contains(class->methods, name)) {
    return NULL;
  }
  return wsky_Dict_get(class->methods, name);
}


ReturnValue wsky_Object_callMethod(Object *object,
                                   const char *methodName,
                                   unsigned parameterCount,
                                   Value *parameters) {

  MethodObject *method = wsky_Object_findMethod(object, methodName);
  Class *class = GET_CLASS(object);

  if (!method) {
    char message[64];
    snprintf(message, 63, "%s class has no method %s",
             class->name, methodName);
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  return wsky_MethodObject_call(method, object, parameterCount, parameters);
}



ReturnValue wsky_Object_callMethod0(Object *object,
                                    const char *methodName) {
  return wsky_Object_callMethod(object, methodName, 0, NULL);
}

ReturnValue wsky_Object_callMethod1(Object *object,
                                    const char *methodName,
                                    Value a) {
  return wsky_Object_callMethod(object, methodName, 1, &a);
}

ReturnValue wsky_Object_callMethod2(Object *object,
                                    const char *methodName,
                                    Value a, Value b) {
  Value parameters[2] = {a, b};
  return wsky_Object_callMethod(object, methodName, 2, parameters);
}

ReturnValue wsky_Object_callMethod3(Object *object,
                                    const char *methodName,
                                    Value a, Value b, Value c) {
  Value parameters[3] = {a, b, c};
  return wsky_Object_callMethod(object, methodName, 3, parameters);
}
