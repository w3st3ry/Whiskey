#include "objects/float.h"

#include <stdlib.h>
#include "objects/str.h"
#include "return_value.h"


typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_String String;
typedef wsky_ReturnValue ReturnValue;

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
  .objectSize = 0,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_Float_CLASS;



static ReturnValue toString(Value *self) {
  return wsky_toString(*self);
}
