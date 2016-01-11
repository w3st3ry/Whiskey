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
#include "objects/type_error.h"
#include "gc.h"


typedef wsky_Object Object;
typedef wsky_Class Class;
typedef wsky_ClassDef ClassDef;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_MethodDef MethodDef;
typedef wsky_Method Method;


static ReturnValue toString(Value *self) {
  static char buffer[100];
  const Class *class = wsky_getClass(*self);
  snprintf(buffer, 90, "<%s>", class->name);
  wsky_RETURN_CSTRING(buffer);
}

static ReturnValue getClass(Value *self) {
  wsky_RETURN_OBJECT((Object *) wsky_getClass(*self));
}


#define OP(name)                                                        \
  static ReturnValue operator##name(Value *self, Value *value) {        \
    (void) self;                                                        \
    (void) value;                                                       \
    wsky_NotImplementedError *e;                                        \
    e = wsky_NotImplementedError_new("Not implemented");                \
    wsky_RETURN_EXCEPTION(e);                                           \
  }

#define ROP(name) OP(name) OP(R##name)
OP(Misc)
ROP(Plus)
ROP(Minus)
ROP(Slash)
ROP(Star)
#undef ROP
#undef OP

#define VALUE (wsky_MethodFlags_VALUE)
#define GETTER (VALUE | wsky_MethodFlags_GET)
#define PUBLIC (VALUE | wsky_MethodFlags_PUBLIC)
#define PUBLIC_GETTER (GETTER | PUBLIC)

static MethodDef methodsDefs[] = {
  {"toString", 0, PUBLIC_GETTER, (void *) *toString},
  {"class", 0, PUBLIC_GETTER, (void *) *getClass},

  {"operator ==", 1, PUBLIC, (void *) *operatorMisc},
  {"operator !=", 1, PUBLIC, (void *) *operatorMisc},

  {"operator r==", 1, PUBLIC, (void *) *operatorMisc},
  {"operator r!=", 1, PUBLIC, (void *) *operatorMisc},

  {"operator +", 1, PUBLIC, (void *) *operatorPlus},
  {"operator -", 1, PUBLIC, (void *) *operatorMinus},
  {"operator *", 1, PUBLIC, (void *) *operatorStar},
  {"operator /", 1, PUBLIC, (void *) *operatorSlash},

  {"operator r+", 1, PUBLIC, (void *) *operatorRPlus},
  {"operator r-", 1, PUBLIC, (void *) *operatorRMinus},
  {"operator r*", 1, PUBLIC, (void *) *operatorRStar},
  {"operator r/", 1, PUBLIC, (void *) *operatorRSlash},
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
  object->fields = NULL;

  if (class->constructor) {
    ReturnValue rv = wsky_Method_call(class->constructor,
                                            object,
                                            paramCount, params);
    if (rv.exception) {
      return rv;
    }
  }

  wsky_GC_register(object);

  wsky_RETURN_OBJECT(object);
}


#define GET_CLASS(object) ((object) ? object->class : wsky_Null_CLASS)


Method *wsky_Object_findMethod(wsky_Object *object, const char *name) {
  return wsky_Class_findMethod(GET_CLASS(object), name);
}


wsky_ReturnValue wsky_Object_get(wsky_Object *object,
                                 const char *name) {
  Method *method = wsky_Object_findMethod(object, name);

  if (!method) {
    Class *class = GET_CLASS(object);
    char message[64];
    snprintf(message, 63, "%s class has no getter %s",
             class->name, name);
    // TODO: Replace
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  if (!(method->flags & wsky_MethodFlags_PUBLIC)) {
    Class *class = GET_CLASS(object);
    char message[64];
    snprintf(message, 63, "The getter %s.%s is private",
             class->name, name);
    // TODO: Replace
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  return wsky_Method_call0(method, object);
}


wsky_ReturnValue wsky_Object_set(wsky_Object *object,
                                 const char *name,
                                 wsky_Value *value) {
  wsky_RETURN_NEW_EXCEPTION("TODO");
}


ReturnValue wsky_Object_callMethod(Object *object,
                                   const char *methodName,
                                   unsigned parameterCount,
                                   Value *parameters) {

  Method *method = wsky_Object_findMethod(object, methodName);

  if (!method) {
    Class *class = GET_CLASS(object);
    char message[64];
    snprintf(message, 63, "%s class has no method %s",
             class->name, methodName);
    // TODO: Replace
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  if (!(method->flags & wsky_MethodFlags_PUBLIC)) {
    Class *class = GET_CLASS(object);
    char message[64];
    snprintf(message, 63, "%s.%s is private",
             class->name, methodName);
    // TODO: Replace
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  return wsky_Method_call(method, object, parameterCount, parameters);
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



ReturnValue wsky_Object_toString(Object *object) {
  if (!object) {
    wsky_RETURN_CSTRING("null");
  }

  const wsky_Class *class = object->class;
  if (class == wsky_String_CLASS) {
    wsky_String *s = (wsky_String *) object;
    wsky_RETURN_CSTRING(s->string);
  }

  wsky_ReturnValue rv = wsky_Object_get(object, "toString");

  if (!rv.exception && !wsky_isString(rv.v)) {
    char buffer[100];
    snprintf(buffer, sizeof buffer,
             "The toString getter has returned a %s",
             wsky_getClassName(rv.v));
    wsky_RETURN_NEW_TYPE_ERROR(buffer);
  }

  return rv;
}
