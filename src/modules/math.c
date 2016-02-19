#include "modules/math.h"

#include "objects/function.h"
#include "objects/float.h"
#include "objects/integer.h"
#include "objects/parameter_error.h"
#include "gc.h"

typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_Object Object;
typedef wsky_Dict Dict;
typedef wsky_Function Function;

wsky_Module *wsky_MATH_MODULE;


#define PI 3.14159265358979323846

static void addValue(Dict *members, const char *name, Value v) {
  Value *valuePointer = wsky_Value_new(v);
  if (!valuePointer)
    abort();
  wsky_Dict_set(members, name, valuePointer);
}

static void addFunction(Dict *members, const char *name,
                        int parameterCount, wsky_Method0 function) {
  wsky_MethodDef def = {
    name, parameterCount, 0, function,
  };
  Function *f = wsky_Function_newFromC(name, &def);
  addValue(members, name, wsky_Value_fromObject((Object *)f));
}

static ReturnValue valueToFloat(Value value, wsky_float *result) {
  if (wsky_isFloat(value)) {
    *result = value.v.floatValue;
    wsky_RETURN_NULL;
  } else if (wsky_isInteger(value)) {
    *result = value.v.intValue;
    wsky_RETURN_NULL;
  }
  wsky_RETURN_NEW_PARAMETER_ERROR("Expected a number");
}

static ReturnValue toDegrees(Object *self, Value *radians_) {
  (void)self;

  wsky_float radians;
  ReturnValue rv = valueToFloat(*radians_, &radians);
  if (rv.exception)
    return rv;

  wsky_RETURN_FLOAT((radians / PI) * 180.0);
}

static ReturnValue toRadians(Object *self, Value *degrees_) {
  (void)self;

  wsky_float degrees;
  ReturnValue rv = valueToFloat(*degrees_, &degrees);
  if (rv.exception)
    return rv;

  wsky_RETURN_FLOAT((degrees / 180.0) * PI);
}

void wsky_math_init(void) {
  Dict members;
  wsky_Dict_init(&members);

  addValue(&members, "PI", wsky_Value_fromFloat(PI));

  addFunction(&members, "toDegrees", 1, (wsky_Method0)&toDegrees);
  addFunction(&members, "toRadians", 1, (wsky_Method0)&toRadians);

  wsky_MATH_MODULE = wsky_Module_new("math", &members, true, NULL);
}
