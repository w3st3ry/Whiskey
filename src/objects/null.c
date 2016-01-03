#include "objects/null.h"

#include <stdlib.h>
#include "objects/str.h"
#include "return_value.h"

typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_String String;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue toString(Value *o);


#define M(name, flags, paramCount)              \
  {#name, paramCount, flags, (void *) &name}

static wsky_MethodDef methods[] = {
  M(toString, wsky_MethodFlags_GET, 0),
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_Null_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "NullClass",
  .constructor = NULL,
  .destructor = NULL,
  .objectSize = 0,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};



static ReturnValue toString(Value *self) {
  (void) self;
  wsky_RETURN_CSTRING("null");
}
