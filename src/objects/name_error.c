#include "objects/name_error.h"

#include <string.h>
#include <stdlib.h>

typedef wsky_NameError NameError;
typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue construct(wsky_Object *object,
                             unsigned paramCount,
                             Value *params);

static ReturnValue destroy(wsky_Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_NameError_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "NameError",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(NameError),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_NameError_CLASS;



NameError *wsky_NameError_new(const char *message) {
  Value v = wsky_buildValue("s", message);
  ReturnValue r;
  r = wsky_Object_new(wsky_NameError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (NameError *) r.v.v.objectValue;
}


static ReturnValue construct(wsky_Object *object,
                             unsigned paramCount,
                             Value *params) {
  wsky_Exception_CLASS_DEF.constructor(object, paramCount, params);
  wsky_RETURN_NULL;
}

static ReturnValue destroy(wsky_Object *object) {
  (void) object;
  wsky_RETURN_NULL;
}
