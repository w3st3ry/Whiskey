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

/* Return 1 if number is positive, -1 if not. */
static ReturnValue wsky_sign(Object *self, Value *number)
{
  (void)self;
  wsky_float nb;
  ReturnValue rv = valueToFloat(*number, &nb);
  if (rv.exception)
    return rv;
  RETURN_FLOAT((nb >= 0) - (nb < 0));
}

static ReturnValue wsky_fabs(Object *self, Value *number)
{
  (void)self;
  wsky_float nb;
  ReturnValue rv = valueToFloat(*number, &nb);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(nb * ((nb >= 0) - (nb < 0)));
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

static ReturnValue wsky_exp(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(exp(x));
}

static ReturnValue wsky_log(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log(x));
}

static ReturnValue wsky_log2(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log2(x));
}

static ReturnValue wsky_log10(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log10(x));
}

static ReturnValue wsky_log1p(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(log1p(x));
}

static ReturnValue wsky_ceil(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(ceil(x));
}

static ReturnValue wsky_floor(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(floor(x));
}

static ReturnValue wsky_isinf(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(isinf(x));
}

static ReturnValue wsky_isnan(Object *self, Value *number)
{
  (void)self;
  wsky_float x;
  ReturnValue rv = valueToFloat(*number, &x);
  if (rv.exception)
    return rv;
  RETURN_FLOAT(isnan(x));
}

static ReturnValue wsky_fmod(Object *self, Value *num1, Value *num2)
{
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

  addFunction(m, "toDegrees", 1, (wsky_Method0)toDegrees);
  addFunction(m, "toRadians", 1, (wsky_Method0)toRadians);
  addFunction(m, "cos", 1, (wsky_Method0)wsky_cos);
  addFunction(m, "sin", 1, (wsky_Method0)wsky_sin);
  addFunction(m, "tan", 1, (wsky_Method0)wsky_tan);
  addFunction(m, "sign", 1, (wsky_Method0)wsky_sign);
  addFunction(m, "fabs", 1, (wsky_Method0)wsky_fabs);
  addFunction(m, "exp", 1, (wsky_Method0)wsky_exp);
  addFunction(m, "log", 1, (wsky_Method0)wsky_log);
  addFunction(m, "log2", 1, (wsky_Method0)wsky_log2);
  addFunction(m, "log10", 1, (wsky_Method0)wsky_log10);
  addFunction(m, "log1p", 1, (wsky_Method0)wsky_log1p);
  addFunction(m, "floor", 1, (wsky_Method0)wsky_floor);
  addFunction(m, "ceil", 1, (wsky_Method0)wsky_ceil);
  addFunction(m, "isinf", 1, (wsky_Method0)wsky_isinf);
  addFunction(m, "isnan", 1, (wsky_Method0)wsky_isnan);
  addFunction(m, "fmod", 2, (wsky_Method0)wsky_fmod);
  addFunction(m, "max", -1, (wsky_Method0)max);
  addFunction(m, "min", -1, (wsky_Method0)min);
}
