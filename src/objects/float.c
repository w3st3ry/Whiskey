#include "../whiskey_private.h"


static ReturnValue toString(Value *self);


#define M(name, flags, paramCount)                                      \
  {#name, paramCount, wsky_MethodFlags_VALUE | flags, (wsky_Method0)&name}

static wsky_MethodDef methods[] = {
  M(toString, wsky_MethodFlags_GET, 0),
  {0, 0, 0, 0},
};

#undef M


const wsky_ClassDef wsky_Float_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Float",
  .final = true,
  .constructor = NULL,
  .destructor = NULL,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_Float_CLASS;



static ReturnValue toString(Value *self) {
  return wsky_toString(*self);
}
