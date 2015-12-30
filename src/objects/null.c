#include "objects/null.h"

#include <stdlib.h>
#include "objects/str.h"
#include "return_value.h"

typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_String String;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue toString(Value *o);


#define M(name, paramCount)                     \
  {#name, paramCount, (void *) &name}

static wsky_MethodDef methods[] = {
  M(toString, 0),
  {0, 0, 0},
};

wsky_Class wsky_Null_CLASS = {
  .super = &wsky_Object_CLASS,
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
