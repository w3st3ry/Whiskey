#include "method_def.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "gc.h"


typedef wsky_MethodDef MethodDef;


wsky_ReturnValue wsky_MethodDef_call(const MethodDef *method,
                                     wsky_Object *object,
                                     unsigned parameterCount,
                                     wsky_Value *parameters) {
  void *m = method->function;

  if (method->parameterCount == -1) {
    return ((wsky_VariadicMethod) m)(object,
                                     parameterCount,
                                     parameters);
  }

  assert((int) parameterCount == method->parameterCount);

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

void wsky_MethodDef_printDebug(const MethodDef *self) {
  printf("name: %s\n", self->name);
  printf("parameter count: %d\n", self->parameterCount);
  printf("address: %p\n", self->function);
}



/*
void wsky_MethodList_init(MethodList *self, unsigned maxCount) {
  self->methods = wsky_MALLOC(sizeof(wsky_MethodDef) * maxCount);
  assert(self->methods);
  self->count = 0;
  self->available = maxCount;
}

void wsky_MethodList_free(MethodList *self) {
  wsky_FREE(self->methods);
}

void wsky_MethodList_add(MethodList *self, MethodDef *method) {
  assert(self->count < self->available);
  self->methods[self->count] = *method;
  self->count++;
}

void wsky_MethodList_printDebug(const MethodList *self) {
  unsigned i;
  printf("MethodList {\n");
  for (i = 0; i < self->count; i++) {
    wsky_MethodDef_printDebug(self->methods + i);
    printf("\n");
  }
  printf("}\n");
}
*/
