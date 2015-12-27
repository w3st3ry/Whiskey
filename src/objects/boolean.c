#include "objects/boolean.h"

#include <stdlib.h>
#include "objects/str.h"
#include "return_value.h"


#define M(name, paramCount)                             \
  {#name, paramCount, (void *) &name}


static wsky_MethodDef methods[] = {
  {0, 0, 0},
};

wsky_Class wsky_Boolean_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Boolean",
  .constructor = NULL,
  .destructor = NULL,
  .objectSize = 0,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};
