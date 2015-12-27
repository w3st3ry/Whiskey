#include "objects/float.h"

#include <stdlib.h>
#include "objects/str.h"
#include "return_value.h"


static wsky_MethodDef methods[] = {
  {0, 0, 0},
};

wsky_Class wsky_Float_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Integer",
  .constructor = NULL,
  .destructor = NULL,
  .objectSize = 0,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};
