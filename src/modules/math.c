#include <math.h>
#include "../whiskey_private.h"


Module *wsky_MATH_MODULE;


#define PI wsky_PI


// TODO: Remove this function, and add Float.NAN
static ReturnValue getNaN(Object *self) {
  (void)self;
  wsky_RETURN_FLOAT(NAN);
}


static ReturnValue valueToFloat(Value value, wsky_float *result) {
  *result = 0.0f;
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

static ReturnValue integerSign(wsky_int integer) {
  if (integer < 0)
    RETURN_INT(-1);
  else if (integer == 0)
    RETURN_INT(0);
  else
    RETURN_INT(1);
}

/* Returns 1 if number is positive, -1 if not. */
static ReturnValue wsky_sign(Object *self, Value *number) {
  (void)self;

  if (wsky_isInteger(*number))
    return integerSign(number->v.intValue);

  wsky_float nb;
  ReturnValue rv = valueToFloat(*number, &nb);

  if (rv.exception)
    return rv;
  if (isnan(nb))
    RAISE_NEW_VALUE_ERROR("The parameter can't be NaN");
  if (nb < 0.0)
    RETURN_INT(-1);
  else if (nb == 0.0)
    RETURN_INT(0);
  RETURN_INT(1);
}

static ReturnValue absInteger(wsky_int n) {
  // I don't use abs() or labs() because a wsky_int is not
  // necessarly an int or a long. But maybe with some preprocessor
  // tricks...
  RETURN_INT(n < 0 ? -n : n);
}

static ReturnValue wsky_abs(Object *self, Value *number) {
  (void)self;

  if (wsky_isInteger(*number))
    return absInteger(number->v.intValue);

  wsky_float nb;
  ReturnValue rv = valueToFloat(*number, &nb);
  if (rv.exception)
    return rv;
  RETURN_FLOAT((wsky_float)fabs((double)nb));
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
#define addFunc(name, params, func)                             \
  wsky_Module_addFunction(m, #name, params, (wsky_Method0)func)


void wsky_math_init(void) {
  wsky_MATH_MODULE = wsky_Module_new("math", true, NULL);
  Module *m = wsky_MATH_MODULE;

  addValue(m, "PI", wsky_Value_fromFloat(PI));
  addValue(m, "E", wsky_Value_fromFloat(wsky_E));

  addFunc(_getNaN, 0, getNaN);

  addFunc(toDegrees, 1, toDegrees);
  addFunc(toRadians, 1, toRadians);
  addFunc(cos, 1, wsky_cos);
  addFunc(sin, 1, wsky_sin);
  addFunc(tan, 1, wsky_tan);
  addFunc(sign, 1, wsky_sign);
  addFunc(abs, 1, wsky_abs);
  addFunc(exp, 1, wsky_exp);
  addFunc(log, 1, wsky_log);
  addFunc(log2, 1, wsky_log2);
  addFunc(log10, 1, wsky_log10);
  addFunc(log1p, 1, wsky_log1p);
  addFunc(floor, 1, wsky_floor);
  addFunc(ceil, 1, wsky_ceil);
  addFunc(fmod, 2, wsky_fmod);
  addFunc(max, -1, max);
  addFunc(min, -1, min);
}
