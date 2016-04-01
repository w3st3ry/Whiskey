#include "../whiskey_private.h"


static Result construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static Result destroy(Object *object);



static MethodDef methods[] = {
  {0, 0, 0, 0},
};

const ClassDef wsky_ValueError_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "ValueError",
  .final = false,
  .constructor = &construct,
  .privateConstructor = false,
  .destructor = &destroy,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

Class *wsky_ValueError_CLASS;



ValueError *wsky_ValueError_new(const char *message) {
  Value v = wsky_buildValue("s", message);
  Result r;
  r = wsky_Object_new(wsky_ValueError_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (ValueError *) r.v.v.objectValue;
}


static Result construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  wsky_Exception_CLASS_DEF.constructor(object, paramCount, params);
  RETURN_NULL;
}

static Result destroy(Object *object) {
  (void) object;
  RETURN_NULL;
}
