#include "method.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "gc.h"


typedef wsky_MethodDef MethodDef;
typedef wsky_MethodList MethodList;


wsky_ReturnValue wsky_MethodDef_call(const MethodDef *method,
                                     wsky_Object *object,
                                     unsigned parameterCount,
                                     wsky_Value *parameters) {
  void *m = method->function;
  switch (method->parameterCount) {
  case -1:
    return ((wsky_VariadicMethod) m)(object,
                                     parameterCount,
                                     parameters);
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

void wsky_MethodDef_printDebug(const MethodDef *this) {
  printf("name: %s\n", this->name);
  printf("parameter count: %d\n", this->parameterCount);
  printf("address: %p\n", this->function);
}



void wsky_MethodList_init(MethodList *this, unsigned maxCount) {
  this->methods = wsky_MALLOC(sizeof(wsky_MethodDef) * maxCount);
  assert(this->methods);
  this->count = 0;
  this->available = maxCount;
}

void wsky_MethodList_free(MethodList *this) {
  wsky_FREE(this->methods);
}

void wsky_MethodList_add(MethodList *this, MethodDef *method) {
  assert(this->count < this->available);
  this->methods[this->count] = *method;
  this->count++;
}

void wsky_MethodList_printDebug(const MethodList *this) {
  unsigned i;
  printf("MethodList {\n");
  for (i = 0; i < this->count; i++) {
    wsky_MethodDef_printDebug(this->methods + i);
    printf("\n");
  }
  printf("}\n");
}