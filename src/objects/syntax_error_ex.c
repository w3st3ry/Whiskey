#include "../whiskey_private.h"


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);


static MethodDef methods[] = {
  {0, 0, 0, 0},
};

const ClassDef wsky_SyntaxErrorEx_CLASS_DEF = {
  .super = &wsky_Exception_CLASS_DEF,
  .name = "SyntaxError",
  .final = true,
  .constructor = &construct,
  .privateConstructor = true,
  .destructor = &destroy,
  .methodDefs = methods,
  .gcAcceptFunction = &acceptGC,
};

Class *wsky_SyntaxErrorEx_CLASS;



SyntaxErrorEx *wsky_SyntaxErrorEx_new(const SyntaxError *syntaxError) {
  Value v = wsky_buildValue("s", syntaxError->message);
  ReturnValue r = wsky_Object_new(wsky_SyntaxErrorEx_CLASS, 1, &v);
  if (r.exception)
    abort();

  SyntaxErrorEx *self = (SyntaxErrorEx *) r.v.v.objectValue;
  wsky_SyntaxError_copy(&self->syntaxError, syntaxError);
  return self;
}


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  wsky_Exception_CLASS_DEF.constructor(object, paramCount, params);
  RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  SyntaxErrorEx *self = (SyntaxErrorEx *) object;
  wsky_SyntaxError_free(&self->syntaxError);
  RETURN_NULL;
}


static void acceptGC(wsky_Object *object) {
  SyntaxErrorEx *self = (SyntaxErrorEx *) object;
  wsky_GC_visitObject(self->syntaxError.position.file);
}
