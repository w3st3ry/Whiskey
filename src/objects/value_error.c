#include "objects/value_error.h"

#include <stdlib.h>

typedef wsky_ValueError ValueError;
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

wsky_Class wsky_ValueError_CLASS = {
  .super = &wsky_Exception_CLASS,
  .name = "ValueError",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(ValueError),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};


ValueError *wsky_ValueError_new(const char *message) {
  wsky_Value v = wsky_buildValue("s", message);
  wsky_ReturnValue r;
  r = wsky_Object_new(&wsky_ValueError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (ValueError *) r.v.v.objectValue;
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
