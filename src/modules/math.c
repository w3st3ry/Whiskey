#include "modules/math.h"

#include "../return_value_private.h"
#include "objects/function.h"
#include "objects/float.h"
#include "objects/integer.h"
#include "objects/boolean.h"
#include "objects/parameter_error.h"
#include "memory.h"

typedef wsky_Value Value;
typedef wsky_Object Object;
typedef wsky_Dict Dict;
typedef wsky_Function Function;

wsky_Module *wsky_MATH_MODULE;


#define PI 3.14159265358979323846


static ReturnValue valueToFloat(Value value, wsky_float *result) {
  if (wsky_isFloat(value)) {
    *result = value.v.floatValue;
    RETURN_NULL;
  } else if (wsky_isInteger(value)) {
    *result = value.v.intValue;
    RETURN_NULL;
  }
  RAISE_NEW_PARAMETER_ERROR("Expected a number");
}

static ReturnValue toDegrees(Object *self, Value *radians_) {
  (void)self;

  wsky_float radians;
  ReturnValue rv = valueToFloat(*radians_, &radians);
  if (rv.exception)
    return rv;

  RETURN_FLOAT((radians / PI) * 180.0);
}

static ReturnValue toRadians(Object *self, Value *degrees_) {
  (void)self;

  wsky_float degrees;
  ReturnValue rv = valueToFloat(*degrees_, &degrees);
  if (rv.exception)
    return rv;

  RETURN_FLOAT((degrees / 180.0) * PI);
}

static ReturnValue max(Object *self,
                       unsigned parameterCount, Value *parameters) {
  (void)self;

  if (parameterCount == 0)
    RAISE_NEW_PARAMETER_ERROR("Expected at least one parameter");

  Value largest = parameters[0];

  for (unsigned i = 1; i < parameterCount; i++) {
    Value value = parameters[i];
    ReturnValue rv = wsky_doBinaryOperation(value,
                                            wsky_Operator_GT,
                                            largest);
    if (rv.exception)
      return rv;
    if (wsky_isBoolean(rv.v) && rv.v.v.boolValue)
      largest = value;
  }

  RETURN_VALUE(largest);
}

static ReturnValue min(Object *self,
                       unsigned parameterCount, Value *parameters) {
  (void)self;

  if (parameterCount == 0)
    RAISE_NEW_PARAMETER_ERROR("Expected at least one parameter");

  Value smallest = parameters[0];

  for (unsigned i = 1; i < parameterCount; i++) {
    Value value = parameters[i];
    ReturnValue rv = wsky_doBinaryOperation(value,
                                            wsky_Operator_LT,
                                            smallest);
    if (rv.exception)
      return rv;
    if (wsky_isBoolean(rv.v) && rv.v.v.boolValue)
      smallest = value;
  }

  RETURN_VALUE(smallest);
}


#define addValue wsky_Module_addValue
#define addFunction wsky_Module_addFunction

void wsky_math_init(void) {
  wsky_MATH_MODULE = wsky_Module_new("math", true, NULL);
  wsky_Module *m = wsky_MATH_MODULE;

  addValue(m, "PI", wsky_Value_fromFloat(PI));

  addFunction(m, "toDegrees", 1, (wsky_Method0)&toDegrees);
  addFunction(m, "toRadians", 1, (wsky_Method0)&toRadians);
  addFunction(m, "max", -1, (wsky_Method0)&max);
  addFunction(m, "min", -1, (wsky_Method0)&min);
}
