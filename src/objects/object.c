#include "objects/object.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../heap.h"
#include "../return_value_private.h"
#include "class_def.h"
#include "gc.h"

#include "objects/instance_method.h"
#include "objects/class.h"
#include "objects/null.h"
#include "objects/str.h"


#include "objects/not_implemented_error.h"
#include "objects/type_error.h"
#include "objects/attribute_error.h"


typedef wsky_ObjectFields ObjectFields;
typedef wsky_ClassDef ClassDef;
typedef wsky_Value Value;
typedef wsky_MethodDef MethodDef;


static void acceptGcOnField(const char* name, void *value_) {
  (void) name;
  wsky_GC_visitValue(*(Value *)value_);
}

void wsky_ObjectFields_acceptGc(ObjectFields *fields) {
  wsky_Dict_apply(&fields->fields, &acceptGcOnField);
  if (fields->parent)
    wsky_ObjectFields_acceptGc(fields->parent);
}

static void freeField(const char *name, void *value) {
  (void) name;
  wsky_free(value);
}

void wsky_ObjectFields_free(ObjectFields *fields) {
  wsky_Dict_apply(&fields->fields, &freeField);
  wsky_Dict_free(&fields->fields);
  if (fields->parent) {
    wsky_ObjectFields_free(fields->parent);
    wsky_free(fields->parent);
  }
}

static void initFields(ObjectFields *fields, Class *class);

static ObjectFields *newFields(Class *class) {
  if (class->native)
    return NULL;
  ObjectFields *fields = wsky_safeMalloc(sizeof(ObjectFields));
  initFields(fields, class);
  return fields;
}

static void initFields(ObjectFields *fields, Class *class) {
  if (class->native)
    return;
  wsky_Dict_init(&fields->fields);
  fields->parent = newFields(class->super);
}

static void printField(const char* name, void *value_) {
  Value *value = (Value *)value_;
  ReturnValue rv = wsky_toString(*value);
  printf("    %s = ", name);
  if (rv.exception) {
    puts("<toString has failed>");
  } else {
    wsky_String *s = (wsky_String *)rv.v.v.objectValue;
    puts(s->string);
  }
}

void wsky_ObjectFields_print(ObjectFields *fields,
                             const Class *class) {
  printf("\nfields (in %s):\n", class->name);
  wsky_Dict_apply(&fields->fields, &printField);
  if (fields->parent)
    wsky_ObjectFields_print(fields->parent, class->super);
  else
    puts("end");
}



static ReturnValue toString(Value *self) {
  static char buffer[100];
  const Class *class = wsky_getClass(*self);
  snprintf(buffer, 90, "<%s>", class->name);
  RETURN_C_STRING(buffer);
}

static ReturnValue getClass(Value *self) {
  RETURN_OBJECT((Object *) wsky_getClass(*self));
}


#define OP(name)                                                        \
  static ReturnValue operator##name(Value *self, Value *value) {        \
    (void) self;                                                        \
    (void) value;                                                       \
    wsky_NotImplementedError *e;                                        \
    e = wsky_NotImplementedError_new("Not implemented");                \
    RAISE_EXCEPTION((wsky_Exception *) e);                              \
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

#define BIN_OP(symbol, name)                                            \
  {"operator " #symbol, 1, PUBLIC, (wsky_Method0)&operator ## name}

static MethodDef methodsDefs[] = {
  {"toString", 0, PUBLIC_GETTER, (wsky_Method0)&toString},
  {"class", 0, PUBLIC_GETTER, (wsky_Method0)&getClass},

  BIN_OP(==, Misc),
  BIN_OP(!=, Misc),

  BIN_OP(r==, Misc),
  BIN_OP(r!=, Misc),

  BIN_OP(+, Plus),
  BIN_OP(-, Minus),
  BIN_OP(*, Star),
  BIN_OP(/, Slash),

  BIN_OP(r+, RPlus),
  BIN_OP(r-, RMinus),
  BIN_OP(r*, RStar),
  BIN_OP(r/, RSlash),

  {0, 0, 0, 0},
};

const ClassDef wsky_Object_CLASS_DEF = {
  .super = NULL,
  .name = "Object",
  .final = false,
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
  Object *object = wsky_heaps_allocateObject();
  if (!object)
    RETURN_NULL;

  object->class = class;

  if (!class->native)
    initFields(&object->fields, class);

  if (class->constructor) {
    ReturnValue rv;
    rv = wsky_Method_call(class->constructor, object, paramCount, params);
    if (rv.exception) {
      if (!class->native)
        wsky_ObjectFields_free(&object->fields);
      wsky_free(object);
      return rv;
    }
  }

  RETURN_OBJECT(object);
}


const char *wsky_Object_getClassName(const Object *o) {
  return wsky_Object_getClass(o)->name;
}

static bool wsky_Class_isSuperclassOf(const Class *super,
                                      const Class *sub) {
  if (!sub->super)
    return false;
  if (super == sub->super)
    return true;
  return wsky_Class_isSuperclassOf(super, sub->super);
}

bool wsky_Object_isA(const Object *object, const Class *class) {
  if (class == object->class)
    return true;
  return wsky_Class_isSuperclassOf(class, object->class);
}


Method *wsky_Object_findMethod(Object *object, const char *name) {
  Class *class = wsky_Object_getClass(object);
  Method *method = wsky_Class_findMethodOrGetter(class, name);
  if (!method)
    return NULL;
  if (method->flags & wsky_MethodFlags_GET)
    return NULL;
  return method;
}

/*
Method *wsky_Object_findMethodOrGetter(Object *object, const char *name) {
  Class *class = wsky_Object_getClass(object);
  Class *declClass = NULL;
  return wsky_Class_findMethodOrGetter(class, name, &declClass);
}
*/


ReturnValue wsky_Object_get(Object *object, const char *name) {
  Class *class = wsky_Object_getClass(object);
  return wsky_Class_get(class, object, name);
}



ReturnValue wsky_Object_set(Object *object, const char *name, Value value) {
  Class *class = wsky_Object_getClass(object);
  return wsky_Class_set(class, object, name, value);
}



ReturnValue wsky_Object_callMethod(Object *object,
                                   const char *methodName,
                                   unsigned parameterCount,
                                   Value *parameters) {

  Method *method = wsky_Object_findMethod(object, methodName);

  if (!method) {
    char message[64];
    snprintf(message, 63, "'%s' object has no method '%s'",
             wsky_Object_getClassName(object), methodName);
    RAISE_NEW_ATTRIBUTE_ERROR(message);
  }

  if (!(method->flags & wsky_MethodFlags_PUBLIC)) {
    char message[64];
    snprintf(message, 63, "'%s.%s' is private",
             wsky_Object_getClassName(object), methodName);
    RAISE_NEW_ATTRIBUTE_ERROR(message);
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
    RETURN_C_STRING("null");
  }

  const Class *class = object->class;
  if (class == wsky_String_CLASS) {
    wsky_String *s = (wsky_String *) object;
    RETURN_C_STRING(s->string);
  }

  ReturnValue rv = wsky_Object_get(object, "toString");

  if (!rv.exception && !wsky_isString(rv.v)) {
    char buffer[100];
    snprintf(buffer, sizeof buffer,
             "The toString getter has returned a %s",
             wsky_getClassName(rv.v));
    RAISE_NEW_TYPE_ERROR(buffer);
  }

  return rv;
}
