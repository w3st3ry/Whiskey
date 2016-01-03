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


static ReturnValue toString(Class *self);


#define GET(name)                                       \
  {#name, 0, wsky_MethodFlags_GET, (void *) &name}

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
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_Class_CLASS;



Class *wsky_Class_new(const ClassDef *def) {
  ReturnValue r = wsky_Object_new(wsky_Class_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  Class *class = (Class *) r.v.v.objectValue;
  class->name = wsky_STRDUP(def->name);
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
  wsky_FREE(self->name);
  wsky_RETURN_NULL;
}


static ReturnValue toString(Class *self) {
  (void) self;
  wsky_RETURN_CSTRING("<Class>");

}


static void acceptGcOnField(const char* name, void *value_) {
  (void) name;
  wsky_GC_VISIT_VALUE(*(Value *) value_);
}

void wsky_Class_acceptGC(wsky_Object *object) {
  wsky_GC_VISIT(object->class);
  wsky_Dict_apply(object->fields, &acceptGcOnField);
}
