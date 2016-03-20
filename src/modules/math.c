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

/* Returns 1 if number is positive, -1 if not. */
static ReturnValue wsky_sign(Object *self, Value *number) {
  (void)self;
  wsky_float nb;
  ReturnValue rv = valueToFloat(*number, &nb);
  if (rv.exception)
    return rv;
  RETURN_FLOAT((nb >= 0) - (nb < 0));
}

static ReturnValue wsky_fabs(Object *self, Value *number) {
  (void)self;
  wsky_float nb;
  ReturnValue rv = valueToFloat(*number, &nb);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(nb * ((nb >= 0) - (nb < 0)));
}

static ReturnValue wsky_cos(Object *self, Value *radian) {
  (void)self;
  wsky_float rad;
  ReturnValue rv = valueToFloat(*radian, &rad);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(cos(rad));
}

static ReturnValue wsky_sin(Object *self, Value *radian) {
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

static ReturnValue wsky_exp(Object *self, Value *number) {
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(exp(x));
}

static ReturnValue wsky_log(Object *self, Value *number) {
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log(x));
}

static ReturnValue wsky_log2(Object *self, Value *number) {
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log2(x));
}

static ReturnValue wsky_log10(Object *self, Value *number) {
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log10(x));
}

static ReturnValue wsky_log1p(Object *self, Value *number) {
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log1p(x));
}

static ReturnValue wsky_ceil(Object *self, Value *number) {
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(ceil(x));
}

static ReturnValue wsky_floor(Object *self, Value *number) {
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(floor(x));
}

static ReturnValue wsky_fmod(Object *self, Value *num1, Value *num2) {
  (void)self;
  wsky_float x;
  wsky_float y;
  ReturnValue rv1 = valueToFloat(*num1, &x);
  ReturnValue rv2 = valueToFloat(*num2, &y);
  if (rv1.exception)
    return rv1;
  if (rv2.exception)
    return rv2;
  RETURN_FLOAT(fmod(x, y));
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

#define FUNC(name, params, func)                        \
  addFunction(m, #name, params, (wsky_Method0)func);

  FUNC(toDegrees, 1, toDegrees);
  FUNC(toRadians, 1, toRadians);
  FUNC(cos, 1, wsky_cos);
  FUNC(sin, 1, wsky_sin);
  FUNC(tan, 1, wsky_tan);
  FUNC(sign, 1, wsky_sign);
  FUNC(abs, 1, wsky_fabs);
  FUNC(exp, 1, wsky_exp);
  FUNC(log, 1, wsky_log);
  FUNC(log2, 1, wsky_log2);
  FUNC(log10, 1, wsky_log10);
  FUNC(log1p, 1, wsky_log1p);
  FUNC(floor, 1, wsky_floor);
  FUNC(ceil, 1, wsky_ceil);
  FUNC(fmod, 2, wsky_fmod);
  FUNC(max, -1, max);
  FUNC(min, -1, min);
}
