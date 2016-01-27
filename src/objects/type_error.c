#include "objects/type_error.h"

#include <stdlib.h>

typedef wsky_Object Object;
typedef wsky_TypeError TypeError;
typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_TypeError_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "TypeError",
  .final = false,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(TypeError),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_TypeError_CLASS;



TypeError *wsky_TypeError_new(const char *message) {
  Value v = wsky_buildValue("s", message);
  ReturnValue r;
  r = wsky_Object_new(wsky_TypeError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (TypeError *) r.v.v.objectValue;
}


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  wsky_Exception_CLASS_DEF.constructor(object, paramCount, params);
  wsky_RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  (void) object;
  wsky_RETURN_NULL;
}
