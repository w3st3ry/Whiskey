#include "modules/math.h"

#include "gc.h"

typedef wsky_Value Value;
typedef wsky_Object Object;
typedef wsky_Dict Dict;

wsky_Module *wsky_MATH_MODULE;


#define PI 3.14159265358979323846

static void setValue(Dict *members, const char *name, Value v) {
  Value *valuePointer = wsky_Value_new(v);
  if (!valuePointer)
    abort();
  wsky_Dict_set(members, name, valuePointer);
}

void wsky_math_init(void) {
  Dict members;
  wsky_Dict_init(&members);

  setValue(&members, "PI", wsky_Value_fromFloat(PI));

  // setValue(&members, "toDegrees", );

  wsky_MATH_MODULE = wsky_Module_new("math", &members, true, NULL);
}
