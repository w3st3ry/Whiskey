#include "../whiskey_private.h"


static ReturnValue toString(Value *self);


#define M(name, flags, paramCount)                                      \
  {#name, paramCount, wsky_MethodFlags_VALUE | flags, (wsky_Method0)&name}

static MethodDef methods[] = {
  M(toString, wsky_MethodFlags_GET, 0),
  {0, 0, 0, 0},
};

#undef M


const ClassDef wsky_Boolean_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Boolean",
  .final = true,
  .constructor = NULL,
  .destructor = NULL,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

Class *wsky_Boolean_CLASS;



static ReturnValue toString(Value *self) {
  return wsky_toString(*self);
}
