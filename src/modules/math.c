#include "modules/math.h"

#include "gc.h"

typedef wsky_Value Value;
typedef wsky_Object Object;
typedef wsky_Dict Dict;

wsky_Module *wsky_MATH_MODULE;


void wsky_math_init(void) {
  Dict members;
  wsky_Dict_init(&members);
  Value *pi = wsky_safeMalloc(sizeof(Value));
  *pi = wsky_Value_fromFloat(3.14159);
  wsky_Dict_set(&members, "PI", pi);
  wsky_MATH_MODULE = wsky_Module_new("math", &members, true, NULL);
}
