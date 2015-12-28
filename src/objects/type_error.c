#include "objects/type_error.h"

#include <stdlib.h>

typedef wsky_TypeError TypeError;
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

wsky_Class wsky_TypeError_CLASS = {
  .super = &wsky_Exception_CLASS,
  .name = "TypeError",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(TypeError),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};


TypeError *wsky_TypeError_new(const char *message) {
  wsky_Value v = wsky_buildValue("s", message);
  wsky_ReturnValue r;
  r = wsky_Object_new(&wsky_TypeError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (TypeError *) r.v.v.objectValue;
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
