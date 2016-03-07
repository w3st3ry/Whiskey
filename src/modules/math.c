#include <math.h>

#include "modules/math.h"

#include "../return_value_private.h"
#include "objects/function.h"
#include "objects/float.h"
#include "objects/integer.h"
#include "objects/boolean.h"
#include "objects/parameter_error.h"
#include "gc.h"

typedef wsky_Value Value;
typedef wsky_Object Object;
typedef wsky_Dict Dict;
typedef wsky_Function Function;

wsky_Module *wsky_MATH_MODULE;


#define PI wsky_PI


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

static ReturnValue wsky_cos(Object *self, Value *radian)
{
  (void)self;
  wsky_float rad;
  ReturnValue rv = valueToFloat(*radian, &rad);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(cos(rad));
}

static ReturnValue wsky_sin(Object *self, Value *radian)
{
  (void)self;
  wsky_float rad;
  ReturnValue rv = valueToFloat(*radian, &rad);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(sin(rad));
}

static ReturnValue wsky_tan(Object *self, Value *radian)
{
  (void)self;
  wsky_float rad;
  ReturnValue rv = valueToFloat(*radian, &rad);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(tan(rad));
}

/* Return 1 if number is positive, -1 if not. */
static ReturnValue sign(Object *self, Value *number)
{
  (void)self;
  wsky_float nb;
  ReturnValue rv = valueToFloat(*number, &nb);
  if (rv.exception)
    return rv;
  RETURN_FLOAT((nb >= 0) - (nb < 0));
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
  addValue(m, "E", wsky_Value_fromFloat(wsky_E));

  addFunction(m, "toDegrees", 1, (wsky_Method0)toDegrees);
  addFunction(m, "toRadians", 1, (wsky_Method0)toRadians);
  addFunction(m, "cos", 1, (wsky_Method0)wsky_cos);
  addFunction(m, "sin", 1, (wsky_Method0)wsky_sin);
  addFunction(m, "tan", 1, (wsky_Method0)wsky_tan);
  addFunction(m, "sign", 1, (wsky_Method0)sign);
  addFunction(m, "max", -1, (wsky_Method0)max);
  addFunction(m, "min", -1, (wsky_Method0)min);
}
