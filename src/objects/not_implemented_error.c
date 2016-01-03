#include "objects/not_implemented_error.h"

#include <stdlib.h>

typedef wsky_NotImplementedError NotImplError;
typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static wsky_ReturnValue construct(wsky_Object *object,
                                  unsigned paramCount,
                                  wsky_Value *params);

static wsky_ReturnValue destroy(wsky_Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_NotImplementedError_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "NotImplementedError",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(NotImplError),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};


NotImplError *wsky_NotImplementedError_new(const char *message) {
  wsky_Value v = wsky_buildValue("s", message);
  wsky_ReturnValue r;
  r = wsky_Object_new(wsky_NotImplementedError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (NotImplError *) r.v.v.objectValue;
}


static wsky_ReturnValue construct(wsky_Object *object,
                                  unsigned paramCount,
                                  wsky_Value *params) {
  wsky_Exception_CLASS_DEF.constructor(object, paramCount, params);
  wsky_RETURN_NULL;
}

static wsky_ReturnValue destroy(wsky_Object *object) {
  (void) object;
  wsky_RETURN_NULL;
}
