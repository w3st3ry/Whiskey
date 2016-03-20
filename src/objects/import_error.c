#include "../return_value_private.h"

typedef wsky_Object Object;
typedef wsky_ImportError ImportError;
typedef wsky_Exception Exception;
typedef wsky_Value Value;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_ImportError_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "ImportError",
  .final = false,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(ImportError),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_ImportError_CLASS;



ImportError *wsky_ImportError_new(const char *message) {
  Value v = wsky_buildValue("s", message);
  ReturnValue r;
  r = wsky_Object_new(wsky_ImportError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (ImportError *) r.v.v.objectValue;
}


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  wsky_Exception_CLASS_DEF.constructor(object, paramCount, params);
  RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  (void) object;
  RETURN_NULL;
}
