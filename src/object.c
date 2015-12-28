#include "object.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "return_value.h"
#include "objects/null.h"
#include "objects/not_implemented_error.h"
#include "gc.h"


typedef wsky_Object Object;
typedef wsky_Class Class;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_MethodDef MethodDef;
typedef wsky_MethodList MethodList;


#define OP(name)                                                        \
  static ReturnValue operator##name(Object *this, Value *value) {       \
    (void) this;                                                        \
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
  {"operator +", 1, (void *) *operatorPlus},
  {"operator -", 1, (void *) *operatorMinus},
  {"operator *", 1, (void *) *operatorStar},
  {"operator /", 1, (void *) *operatorSlash},

  {"operator r+", 1, (void *) *operatorRPlus},
  {"operator r-", 1, (void *) *operatorRMinus},
  {"operator r*", 1, (void *) *operatorRStar},
  {"operator r/", 1, (void *) *operatorRSlash},
  {0, 0, 0},
};

Class wsky_Object_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Object",
  .constructor = NULL,
  .destructor = NULL,
  .objectSize = sizeof(Object),
  .methodDefs = methodsDefs,
  .gcAcceptFunction = NULL,
};



ReturnValue wsky_Object_new(const wsky_Class *class,
                            unsigned paramCount,
                            Value *params) {
  Object *object = wsky_MALLOC(class->objectSize);
  if (!object)
    return wsky_ReturnValue_NULL;
  object->class = class;
  wsky_Exception *exception = class->constructor(object, paramCount, params);
  if (exception) {
    wsky_RETURN_EXCEPTION(exception);
  }

  wsky_GC_register(object);

  wsky_RETURN_OBJECT(object);
}


#define GET_CLASS(object) ((object) ? object->class : &wsky_Null_CLASS)


const MethodDef *wsky_Object_findMethod(Object *object,
                                        const char *methodName) {
  return wsky_Class_findMethod(GET_CLASS(object), methodName);
}


ReturnValue wsky_Object_callMethod(Object *object,
                                   const char *methodName,
                                   unsigned parameterCount,
                                   Value *parameters) {

  const MethodDef *method = wsky_Object_findMethod(object, methodName);
  const wsky_Class *class = GET_CLASS(object);

  if (!method) {
    char message[64];
    snprintf(message, 63, "%s class has no method %s",
             class->name, methodName);
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  if (method->parameterCount != -1 &&
      method->parameterCount != (int) parameterCount) {

    char message[64];
    snprintf(message, 63, "Invalid parameter count");
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  return wsky_MethodDef_call(method, object, parameterCount, parameters);
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
