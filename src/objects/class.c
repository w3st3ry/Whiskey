#include "objects/class.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"
#include "return_value.h"
#include "objects/str.h"


typedef wsky_Class Class;
typedef wsky_ClassDef ClassDef;
typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(wsky_Object *object);

static ReturnValue toString(Class *self);


#define GET(name)                                               \
  {#name, 0, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC,    \
      (void *) &name}

static wsky_MethodDef methods[] = {
  GET(toString),

  {0, 0, 0, 0},
};

#undef GET


const ClassDef wsky_Class_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Class",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Class),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

wsky_Class *wsky_Class_CLASS;



void wsky_Class_initMethods(Class *class, const ClassDef *def) {
  class->methods = wsky_Dict_new();
  wsky_MethodDef *methodDef = def->methodDefs;
  while (methodDef->name) {
    wsky_Method* method = wsky_Method_newFromC(methodDef);
    /*printf("wsky_Class_initMethods() %s %s %d\n", class->name,
      method->name, method->cMethod.parameterCount);*/
    wsky_Dict_set(class->methods, method->name, method);
    methodDef++;
  }
}


Class *wsky_Class_new(const ClassDef *def, Class *super) {
  Class *class = wsky_MALLOC(sizeof(Class));
  if (!class)
    return NULL;
  class->class = wsky_Class_CLASS;
  wsky_GC_register((Object *) class);
  class->name = wsky_STRDUP(def->name);
  class->objectSize = def->objectSize;
  class->super = super;
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
      (void *) def->constructor,
    };
    class->constructor = wsky_Method_newFromC(&ctorDef);
  }
  return class;
}

static wsky_ReturnValue construct(Object *object,
                                  unsigned paramCount,
                                  Value *params) {
  (void) paramCount;
  (void) params;
  (void) object;
  wsky_RETURN_NULL;
}

static wsky_ReturnValue destroy(Object *object) {
  Class *self = (Class *) object;
  /*printf("Destroying class %s\n", self->name);*/
  wsky_FREE(self->name);
  wsky_Dict_delete(self->methods);
  wsky_RETURN_NULL;
}


static void methodAcceptGC(const char *name, void *value) {
  (void) name;
  wsky_GC_VISIT(value);
}

static void acceptGC(wsky_Object *object) {
  Class *self = (Class *) object;
  wsky_GC_VISIT(self->constructor);
  wsky_Dict_apply(self->methods, methodAcceptGC);
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


static void acceptGcOnField(const char* name, void *value_) {
  (void) name;
  wsky_GC_VISIT_VALUE(*(Value *) value_);
}

void wsky_Class_acceptGC(wsky_Object *object) {
  Class *class = object->class;
  wsky_GC_VISIT(class);
  if (object->class == wsky_Object_CLASS)
    wsky_Dict_apply(object->fields, &acceptGcOnField);
  if (class->gcAcceptFunction) {
    class->gcAcceptFunction(object);
  }
}


wsky_Method *wsky_Class_findLocalMethod(Class *class,
                                              const char *name) {
  if (!wsky_Dict_contains(class->methods, name)) {
    return NULL;
  }
  return wsky_Dict_get(class->methods, name);
}

wsky_Method *wsky_Class_findMethod(Class *class, const char *name) {
  wsky_Method *method = wsky_Class_findLocalMethod(class, name);
  if (method)
    return method;
  if (class->super) {
    return wsky_Class_findMethod(class->super, name);
  }
  return NULL;
}
