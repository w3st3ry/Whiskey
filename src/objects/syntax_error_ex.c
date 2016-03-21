#include "../whiskey_private.h"


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);



static MethodDef methods[] = {
  {0, 0, 0, 0},
};

const ClassDef wsky_SyntaxErrorEx_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "SyntaxError",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(SyntaxErrorEx),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

Class *wsky_SyntaxErrorEx_CLASS;



SyntaxErrorEx *wsky_SyntaxErrorEx_new(SyntaxError *syntaxError) {
  Value v = wsky_buildValue("s", syntaxError->message);
  ReturnValue r = wsky_Object_new(wsky_SyntaxErrorEx_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (SyntaxErrorEx *) r.v.v.objectValue;
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
