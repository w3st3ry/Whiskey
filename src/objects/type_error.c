#include "../whiskey_private.h"


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);



static MethodDef methods[] = {
  {0, 0, 0, 0},
};

const ClassDef wsky_TypeError_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "TypeError",
  .final = false,
  .constructor = &construct,
  .privateConstructor = false,
  .destructor = &destroy,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

Class *wsky_TypeError_CLASS;



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
  RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  (void) object;
  RETURN_NULL;
}
