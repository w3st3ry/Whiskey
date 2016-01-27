#include "objects/syntax_error_ex.h"

#include <stdlib.h>

typedef wsky_Object Object;
typedef wsky_SyntaxErrorEx SyntaxErrorEx;
typedef wsky_SyntaxError SyntaxError;
typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_SyntaxErrorEx_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "SyntaxError",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(SyntaxErrorEx),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_SyntaxErrorEx_CLASS;



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
  wsky_RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  (void) object;
  wsky_RETURN_NULL;
}
