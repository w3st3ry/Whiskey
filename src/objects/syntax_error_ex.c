#include "objects/syntax_error_ex.h"

#include <stdlib.h>

typedef wsky_SyntaxErrorEx SyntaxErrorEx;
typedef wsky_SyntaxError SyntaxError;
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

const wsky_ClassDef wsky_SyntaxErrorEx_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "SyntaxError",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(SyntaxErrorEx),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_SyntaxErrorEx_CLASS;



SyntaxErrorEx *wsky_SyntaxErrorEx_new(SyntaxError *syntaxError) {
  wsky_Value v = wsky_buildValue("s", syntaxError->message);
  wsky_ReturnValue r;
  r = wsky_Object_new(wsky_SyntaxErrorEx_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (SyntaxErrorEx *) r.v.v.objectValue;
}


static wsky_ReturnValue construct(wsky_Object *object,
                                  unsigned paramCount,
                                  Value *params) {
  wsky_Exception_CLASS_DEF.constructor(object, paramCount, params);
  wsky_RETURN_NULL;
}

static wsky_ReturnValue destroy(wsky_Object *object) {
  (void) object;
  wsky_RETURN_NULL;
}
