#include "objects/not_implemented_error.h"

#include <stdlib.h>

typedef wsky_NotImplementedError NotImplError;
typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static Exception *construct(wsky_Object *object,
                            unsigned paramCount,
                            wsky_Value *params);
static void destroy(wsky_Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0},
};

wsky_Class wsky_NotImplementedError_CLASS = {
  .super = &wsky_Exception_CLASS,
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
  r = wsky_Object_new(&wsky_NotImplementedError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (NotImplError *) r.v.v.objectValue;
}


static Exception *construct(wsky_Object *object,
                            unsigned paramCount,
                            wsky_Value *params) {
  wsky_Exception_CLASS.constructor(object, paramCount, params);
  return NULL;
}

static void destroy(wsky_Object *object) {
  (void) object;
}
