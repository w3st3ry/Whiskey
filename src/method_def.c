#include "method_def.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "return_value_private.h"
#include "objects/parameter_error.h"


typedef wsky_MethodDef MethodDef;
typedef wsky_Value Value;
typedef wsky_Object Object;


static ReturnValue wsky_MethodDef_callImpl(const MethodDef *method,
                                           Object *object,
                                           unsigned parameterCount,
                                           const Value *constParams) {
  Value parameters[64];
  memcpy(parameters, constParams, sizeof(Value) * parameterCount);

  wsky_Method0 m = (wsky_Method0)method->function;

  if (method->parameterCount == -1) {
    return ((wsky_VariadicMethod) m)(object,
                                     parameterCount,
                                     parameters);
  } else {
    if ((int) parameterCount != method->parameterCount) {
      RAISE_NEW_PARAMETER_ERROR("Invalid parameter count");
    }
  }

  switch (method->parameterCount) {
  case 0:
    return ((wsky_Method0) m)(object);
  case 1:
    return ((wsky_Method1) m)(object,
                              parameters);
  case 2:
    return ((wsky_Method2) m)(object,
                              parameters,
                              parameters + 1);
  case 3:
    return ((wsky_Method3) m)(object,
                              parameters,
                              parameters + 1,
                              parameters + 2);
  case 4:
    return ((wsky_Method4) m)(object,
                              parameters,
                              parameters + 1,
                              parameters + 2,
                              parameters + 3);
  case 5:
    return ((wsky_Method5) m)(object,
                              parameters,
                              parameters + 1,
                              parameters + 2,
                              parameters + 3,
                              parameters + 4);
  default:
    fprintf(stderr, "wsky_Method_call(): Too many parameters\n");
    abort();
  }
}


ReturnValue wsky_MethodDef_call(const MethodDef *method,
                                Object *self,
                                unsigned parameterCount,
                                const Value *parameters) {

  if (method->flags & wsky_MethodFlags_VALUE) {
    Value v = wsky_Value_fromObject(self);
    return wsky_MethodDef_callImpl(method, (Object *) &v,
                                   parameterCount, parameters);
  } else {
    return wsky_MethodDef_callImpl(method, self,
                                   parameterCount, parameters);
  }
}


ReturnValue wsky_MethodDef_callValue(const MethodDef *method,
                                     Value self,
                                     unsigned parameterCount,
                                     const Value *parameters) {

  if (method->flags & wsky_MethodFlags_VALUE) {
    return wsky_MethodDef_callImpl(method, (Object *) &self,
                                   parameterCount, parameters);
  } else {
    abort();
  }
}


void wsky_MethodDef_printDebug(const MethodDef *self) {
  printf("name: %s\n", self->name);
  printf("parameter count: %d\n", self->parameterCount);
}
