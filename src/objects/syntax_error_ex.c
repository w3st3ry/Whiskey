#include "objects/syntax_error_ex.h"

#include <stdlib.h>

typedef wsky_SyntaxErrorEx SyntaxErrorEx;
typedef wsky_SyntaxError SyntaxError;
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

wsky_Class wsky_SyntaxErrorEx_CLASS = {
  .super = &wsky_Exception_CLASS,
  .name = "SyntaxError",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(SyntaxErrorEx),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};


SyntaxErrorEx *wsky_SyntaxErrorEx_new(SyntaxError *syntaxError) {
  wsky_Value v = wsky_buildValue("s", syntaxError->message);
  wsky_ReturnValue r;
  r = wsky_Object_new(&wsky_SyntaxErrorEx_CLASS, 1, &v);
  if (r.exception)
    abort();
  return (SyntaxErrorEx *) r.v.v.objectValue;
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
