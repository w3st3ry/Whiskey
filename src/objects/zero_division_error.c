#include <string.h>
#include "../whiskey_private.h"


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);

static ReturnValue destroy(Object *object);



static MethodDef methods[] = {
  {0, 0, 0, 0},
};

const ClassDef wsky_ZeroDivisionError_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "ZeroDivisionError",
  .final = false,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(ZeroDivisionError),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

Class *wsky_ZeroDivisionError_CLASS;



ZeroDivisionError *wsky_ZeroDivisionError_new(void) {
  Value v = wsky_buildValue("s", "Division by zero");
  ReturnValue r;
  r = wsky_Object_new(wsky_ZeroDivisionError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (ZeroDivisionError *) r.v.v.objectValue;
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
