#include "objects/boolean.h"

#include <stdlib.h>
#include "objects/str.h"
#include "return_value.h"


typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_String String;
typedef wsky_ReturnValue ReturnValue;

static ReturnValue toString(Value *self);


#define M(name, flags, paramCount)              \
  {#name, paramCount, flags, (void *) &name}

static wsky_MethodDef methods[] = {
  M(toString, wsky_MethodFlags_GET, 0),
  {0, 0, 0, 0},
};

#undef M


const wsky_ClassDef wsky_Boolean_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Boolean",
  .constructor = NULL,
  .destructor = NULL,
  .objectSize = 0,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_Boolean_CLASS;



static ReturnValue toString(Value *self) {
  wsky_RETURN_OBJECT((Object *) wsky_toString(*self));
}
