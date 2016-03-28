#include "../whiskey_private.h"


static ReturnValue toString(Value *self);


#define M(name, flags, paramCount)                                      \
  {#name, paramCount, wsky_MethodFlags_VALUE | flags, (wsky_Method0)&name}

static MethodDef methods[] = {
  M(toString, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC, 0),
  {0, 0, 0, 0},
};

#undef M


const ClassDef wsky_Null_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "NullClass",
  .final = true,
  .constructor = NULL,
  .privateConstructor = true,
  .destructor = NULL,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

Class *wsky_Null_CLASS;



static ReturnValue toString(Value *self) {
  (void) self;
  RETURN_C_STRING("null");
}
