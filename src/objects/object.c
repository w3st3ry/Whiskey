#include "objects/object.h"

#include <assert.h>
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
#include "objects/attribute_error.h"
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
  /** TODO: Manage the case where malloc() returns 0 */
  Object *object = wsky_malloc(class->objectSize);
  if (!object)
    return wsky_ReturnValue_NULL;

  wsky_GC_register(object);

  object->class = class;

  if (!class->native)
    wsky_Dict_init(&object->fields);

  if (class->constructor) {
    ReturnValue rv;
    rv = wsky_Method_call(class->constructor, object, paramCount, params);
    if (rv.exception)
      return rv;
  }

  wsky_RETURN_OBJECT(object);
}


const char *wsky_Object_getClassName(wsky_Object *o) {
  return wsky_Object_getClass(o)->name;
}


Method *wsky_Object_findMethod(Object *object, const char *name) {
  Class *class = wsky_Object_getClass(object);
  Method *m = wsky_Class_findMethod(class, name);
  if (m && (m->flags & wsky_MethodFlags_GET))
    return NULL;
  return m;
}

Method *wsky_Object_findGetter(Object *object, const char *name) {
  Class *class = wsky_Object_getClass(object);
  Method *m = wsky_Class_findMethod(class, name);
  if (m && !(m->flags & wsky_MethodFlags_GET))
    return NULL;
  return m;
}

static inline bool isPublic(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_PUBLIC;
}

ReturnValue wsky_Object_get(Object *object, const char *name) {
  Method *method = wsky_Object_findGetter(object, name);

  if (!method) {
    char message[64];
    snprintf(message, 63, "'%s' class has no getter '%s'",
             wsky_Object_getClassName(object), name);
    wsky_RETURN_NEW_ATTRIBUTE_ERROR(message);
  }

  if (!isPublic(method->flags)) {
    char message[64];
    snprintf(message, 63, "The getter '%s.%s' is private",
             wsky_Object_getClassName(object), name);
    wsky_RETURN_NEW_ATTRIBUTE_ERROR(message);
  }

  return wsky_Method_call0(method, object);
}


static ReturnValue setField(Object *object,
                            const char *name, const Value *value) {
  assert(!wsky_Object_getClass(object)->native);
  free(wsky_Dict_get(&object->fields, name));

  Value *mv = malloc(sizeof(Value));
  *mv = *value;
  wsky_Dict_set(&object->fields, name, mv);
  return wsky_ReturnValue_fromValue(*value);
}

static ReturnValue callSetter(Object *object,
                              wsky_Method *method,
                              const char *name, const Value *value) {
  if (wsky_Method_isDefault(method))
    return setField(object, name, value);

  return wsky_Method_call1(method, object, *value);
}


static ReturnValue setOutsideOfClass(Object *object,
                                     const char *name, const Value *value) {
  Class *class = wsky_Object_getClass(object);
  wsky_Method *method = wsky_Class_findSetter(class, name);

  if (method && (method->flags & wsky_MethodFlags_PUBLIC))
    return callSetter(object, method, name, value);

  char message[64];
  snprintf(message, 63, "'%s' class has no public setter '%s'",
           class->name, name);
  wsky_RETURN_NEW_ATTRIBUTE_ERROR(message);
}


static ReturnValue setInsideOfClass(Object *object,
                                    const char *name, const Value *value) {
  Class *class = wsky_Object_getClass(object);
  wsky_Method *method = wsky_Class_findSetter(class, name);

  if (method)
    return callSetter(object, method, name, value);

  return setField(object, name, value);
}

ReturnValue wsky_Object_set(Object *object,
                            const char *name, const Value *value,
                            bool privateAccess) {
  if (privateAccess)
    return setInsideOfClass(object, name, value);

  return setOutsideOfClass(object, name, value);
}


ReturnValue wsky_Object_callMethod(Object *object,
                                   const char *methodName,
                                   unsigned parameterCount,
                                   Value *parameters) {

  Method *method = wsky_Object_findMethod(object, methodName);

  if (!method) {
    char message[64];
    snprintf(message, 63, "'%s' class has no method '%s'",
             wsky_Object_getClassName(object), methodName);
    wsky_RETURN_NEW_ATTRIBUTE_ERROR(message);
  }

  if (!(method->flags & wsky_MethodFlags_PUBLIC)) {
    char message[64];
    snprintf(message, 63, "'%s.%s' is private",
             wsky_Object_getClassName(object), methodName);
    wsky_RETURN_NEW_ATTRIBUTE_ERROR(message);
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

  const Class *class = object->class;
  if (class == wsky_String_CLASS) {
    wsky_String *s = (wsky_String *) object;
    wsky_RETURN_CSTRING(s->string);
  }

  ReturnValue rv = wsky_Object_get(object, "toString");

  if (!rv.exception && !wsky_isString(rv.v)) {
    char buffer[100];
    snprintf(buffer, sizeof buffer,
             "The toString getter has returned a %s",
             wsky_getClassName(rv.v));
    wsky_RETURN_NEW_TYPE_ERROR(buffer);
  }

  return rv;
}
