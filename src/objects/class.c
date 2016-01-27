#include "objects/class.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"
#include "return_value.h"

#include "objects/str.h"
#include "objects/instance_method.h"

#include "objects/attribute_error.h"
#include "objects/parameter_error.h"
#include "objects/type_error.h"


typedef wsky_Class Class;
typedef wsky_ClassDef ClassDef;
typedef wsky_Method Method;
typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);

static ReturnValue toString(Class *self);

static ReturnValue superGetter(Class *self);

static ReturnValue get(Class *class, Value *selfObject,
                       Value *name);
static ReturnValue set(Class *class, Value *selfObject,
                       Value *name, Value *value);

static ReturnValue init(Class *class,
                        unsigned parameterCount,
                        const Value *parameters);


#define GET_NAME(function, name)                                \
  {#name, 0, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC,    \
      (wsky_Method0)&function}

#define GET(name) GET_NAME(name, name)

static wsky_MethodDef methods[] = {
  GET(toString),
  GET_NAME(superGetter, super),

  {"get", 2, wsky_MethodFlags_PUBLIC, (wsky_Method0)&get},
  {"set", 3, wsky_MethodFlags_PUBLIC, (wsky_Method0)&set},

  {"init", -1, wsky_MethodFlags_PUBLIC, (wsky_Method0)&init},

  {0, 0, 0, 0},
};

#undef GET


const ClassDef wsky_Class_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Class",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Class),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

Class *wsky_Class_CLASS;



static inline bool isSetter(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_SET;
}

static inline bool isConstructor(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_INIT;
}


void wsky_Class_initMethods(Class *class, const ClassDef *def) {
  wsky_MethodDef *methodDef = def->methodDefs;

  while (methodDef->name) {
    Method* method = wsky_Method_newFromC(methodDef, class);
    if (isConstructor(method->flags))
      abort();

    if (isSetter(method->flags))
      wsky_Dict_set(class->setters, method->name, method);
    else
      wsky_Dict_set(class->methods, method->name, method);
    methodDef++;
  }
}


Class *wsky_Class_new(const char *name, Class *super) {
  if (super)
    assert(!super->final);

  Class *class = wsky_safeMalloc(sizeof(Class));
  if (!class)
    return NULL;
  class->class = wsky_Class_CLASS;
  wsky_GC_register((Object *) class);
  class->name = wsky_strdup(name);
  class->native = false;
  class->final = false;
  class->objectSize = sizeof(Object);
  class->super = super;
  class->gcAcceptFunction = NULL;
  class->destructor = NULL;

  class->methods = wsky_Dict_new();
  class->setters = wsky_Dict_new();
  class->constructor = NULL;
  return class;
}


Class *wsky_Class_newFromC(const ClassDef *def, Class *super) {
  Class *class = wsky_Class_new(def->name, super);
  if (!class)
    return NULL;

  class->native = true;
  class->objectSize = def->objectSize;
  class->super = super;
  class->final = def->final;
  class->gcAcceptFunction = def->gcAcceptFunction;
  class->destructor = def->destructor;

  if (def == &wsky_Class_CLASS_DEF ||
      def == &wsky_Object_CLASS_DEF ||
      def == &wsky_Method_CLASS_DEF) {
    class->constructor = NULL;
  } else {
    wsky_Class_initMethods(class, def);

    wsky_MethodDef ctorDef = {
      "<Constructor>",
      -1,
      wsky_MethodFlags_PUBLIC,
      (wsky_Method0)def->constructor,
    };
    class->constructor = wsky_Method_newFromC(&ctorDef, class);
  }
  return class;
}

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  (void) paramCount;
  (void) params;
  (void) object;
  wsky_RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  Class *self = (Class *) object;
  /*printf("Destroying class %s\n", self->name);*/
  wsky_free(self->name);
  wsky_Dict_delete(self->methods);
  wsky_Dict_delete(self->setters);
  wsky_RETURN_NULL;
}


static void methodAcceptGC(const char *name, void *value) {
  (void) name;
  wsky_GC_VISIT(value);
}

static void acceptGC(Object *object) {
  Class *self = (Class *) object;
  wsky_GC_VISIT(self->constructor);
  wsky_Dict_apply(self->methods, methodAcceptGC);
  wsky_Dict_apply(self->setters, methodAcceptGC);
  if (self->super) {
    wsky_GC_VISIT(self->super);
  }
}


static ReturnValue toString(Class *self) {
  (void) self;
  char buffer[64];
  snprintf(buffer, 63, "<Class %s>", self->name);
  wsky_RETURN_CSTRING(buffer);
}

static ReturnValue superGetter(Class *self) {
  wsky_RETURN_OBJECT((Object *)self->super);
}

static ReturnValue init(Class *class,
                        unsigned parameterCount,
                        const Value *parameters) {
  if (parameterCount == 0)
    wsky_RETURN_NEW_PARAMETER_ERROR("init() takes at least one argument");

  const Value *self_ = parameters;

  if (self_->type != wsky_Type_OBJECT)
    wsky_RETURN_NEW_EXCEPTION("Not implemented");

  Object *self = self_->v.objectValue;
  if (!wsky_Object_isA(self, class))
    wsky_RETURN_NEW_TYPE_ERROR("Type error");

  parameterCount--;
  parameters++;
  ReturnValue rv = wsky_Method_call(class->constructor, self,
                                    parameterCount, parameters);
  if (rv.exception)
    return rv;

  wsky_RETURN_OBJECT(self);
}

static ReturnValue get(Class *class, Value *self_, Value *name_) {
  if (!wsky_isString(*name_))
    wsky_RETURN_NEW_PARAMETER_ERROR("The 2nd parameter must be a string");

  const char *name = ((wsky_String *)name_->v.objectValue)->string;

  if (self_->type != wsky_Type_OBJECT)
    wsky_RETURN_NEW_EXCEPTION("Not implemented");

  Object *self = self_->v.objectValue;
  if (!self)
    wsky_RETURN_NEW_EXCEPTION("Not implemented");

  return wsky_Class_get(class, self, name);
}

static ReturnValue set(Class *class, Value *self_,
                       Value *name_, Value *value) {
  if (!wsky_isString(*name_))
    wsky_RETURN_NEW_PARAMETER_ERROR("The 2nd parameter must be a string");

  const char *name = ((wsky_String *)name_->v.objectValue)->string;

  if (self_->type != wsky_Type_OBJECT)
    wsky_RETURN_NEW_EXCEPTION("Not implemented");

  Object *self = self_->v.objectValue;
  if (!self)
    wsky_RETURN_NEW_EXCEPTION("Not implemented");

  return wsky_Class_set(class, self, name, value);
}



void wsky_Class_acceptGC(Object *object) {
  Class *class = object->class;
  wsky_GC_VISIT(class);
  if (!class->native)
    wsky_ObjectFields_acceptGc(&object->fields);
  if (class->gcAcceptFunction) {
    class->gcAcceptFunction(object);
  }
}



static inline bool isPublic(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_PUBLIC;
}

static inline bool isGetter(wsky_MethodFlags flags) {
  return flags & wsky_MethodFlags_GET;
}



static wsky_ObjectFields *getFields(Class *wantedClass, Object *self) {
  wsky_ObjectFields *fields = &self->fields;
  Class *class = self->class;
  while (fields) {
    assert(!class->native);
    if (class == wantedClass)
      return fields;
    class = class->super;
    fields = fields->parent;
  }
  return NULL;
}

ReturnValue wsky_Class_getField(Class *class, Object *self,
                                const char *name) {
  assert(!class->native);
  wsky_ObjectFields *fields = getFields(class, self);

  if (fields) {
    Value *v = wsky_Dict_get(&fields->fields, name);
    if (v)
      return wsky_ReturnValue_fromValue(*v);
  }

  const char *className = wsky_Object_getClassName(self);
  return wsky_AttributeError_raiseNoAttr(className, name);
}

ReturnValue wsky_Class_callGetter(Object *self,
                                  Method *method, const char *name) {
  assert(isGetter(method->flags));

  if (wsky_Method_isDefault(method))
    return wsky_Class_getField(method->defClass, self, name);

  return wsky_Method_call0(method, self);
}


static ReturnValue raiseTypeError(const char *expectedClass,
                                  const char *class) {
  static char buffer[64];
  snprintf(buffer, 63, "Expected a '%s', got a '%s'", expectedClass, class);
  wsky_RETURN_NEW_TYPE_ERROR(buffer);
}

ReturnValue wsky_Class_get(Class *class, Object *self,
                           const char *attribute) {
  if (!wsky_Object_isA(self, class))
    return raiseTypeError(class->name, wsky_Object_getClass(self)->name);

  Method *method = wsky_Class_findMethodOrGetter(class, attribute);

  if (!method || !isPublic(method->flags))
    return wsky_AttributeError_raiseNoAttr(class->name, attribute);

  if (isGetter(method->flags))
    return wsky_Class_callGetter(self, method, attribute);

  Value v = wsky_Value_fromObject(self);
  wsky_RETURN_OBJECT((Object *)wsky_InstanceMethod_new(method, &v));
}

ReturnValue wsky_Class_getPrivate(Class *class, Object *self,
                                  const char *attribute) {
  if (!wsky_Object_isA(self, class))
    return raiseTypeError(class->name, wsky_Object_getClass(self)->name);

  wsky_Method *method = wsky_Class_findMethodOrGetter(class, attribute);
  if (method)
    return wsky_Class_callGetter(self, method, attribute);

  return wsky_Class_getField(class, self, attribute);
}




ReturnValue wsky_Class_setField(Class *class, Object *self,
                                const char *name, const Value *value) {
  assert(!class->native);
  wsky_ObjectFields *fields = getFields(class, self);

  if (fields) {
    free(wsky_Dict_get(&fields->fields, name));

    Value *mv = malloc(sizeof(Value));
    *mv = *value;
    wsky_Dict_set(&fields->fields, name, mv);
    wsky_RETURN_VALUE(*value);
  }

  const char *className = wsky_Object_getClassName(self);
  return wsky_AttributeError_raiseNoAttr(className, name);
}

ReturnValue wsky_Class_callSetter(Object *self,
                                  Method *method, const char *name,
                                  const Value *value) {
  assert(isSetter(method->flags));

  if (wsky_Method_isDefault(method))
    return wsky_Class_setField(method->defClass, self, name, value);

  return wsky_Method_call1(method, self, *value);
}

ReturnValue wsky_Class_set(Class *class, Object *self,
                           const char *attribute, const Value *value) {
  if (!wsky_Object_isA(self, class))
    return raiseTypeError(class->name, wsky_Object_getClass(self)->name);

  Method *method = wsky_Class_findSetter(class, attribute);

  if (method && isPublic(method->flags))
    return wsky_Class_callSetter(self, method, attribute, value);

  return wsky_AttributeError_raiseNoAttr(class->name, attribute);
}

ReturnValue wsky_Class_setPrivate(Class *class, Object *self,
                                  const char *attribute,
                                  const Value *value) {
  if (!wsky_Object_isA(self, class))
    return raiseTypeError(class->name, wsky_Object_getClass(self)->name);

  wsky_Method *method = wsky_Class_findSetter(class, attribute);
  if (method)
    return wsky_Class_callSetter(self, method, attribute, value);

  return wsky_Class_setField(class, self, attribute, value);
}



Method *wsky_Class_findLocalMethod(Class *class, const char *name) {
  return wsky_Dict_get(class->methods, name);
}

Method *wsky_Class_findMethodOrGetter(Class *class, const char *name) {
  Method *method = wsky_Class_findLocalMethod(class, name);
  if (method)
    return method;

  if (class->super)
    return wsky_Class_findMethodOrGetter(class->super, name);
  return NULL;
}


Method *wsky_Class_findLocalSetter(Class *class, const char *name) {
  return wsky_Dict_get(class->setters, name);
}

Method *wsky_Class_findSetter(Class *class, const char *name) {
  Method *method = wsky_Class_findLocalSetter(class, name);
  if (method)
    return method;

  if (class->super)
    return wsky_Class_findSetter(class->super, name);
  return NULL;
}


ReturnValue wsky_Class_construct(Class *class,
                                 unsigned parameterCount,
                                 Value *parameters) {
  if (!class->constructor)
    wsky_RETURN_NEW_TYPE_ERROR("This class has no constructor");

  return wsky_Object_new(class, parameterCount, parameters);
}
