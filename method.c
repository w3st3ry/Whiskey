#include "method.h"

#include <stdlib.h>
#include <stdio.h>

wsky_Method wsky_Method_create(const char *name,
                                 int parameterCount,
                                 void *function) {
  wsky_Method method = {
    .name = name,
    .parameterCount = parameterCount,
    .function = function
  };
  if (parameterCount < 0 && parameterCount != -1) {
    fprintf(stderr,
            "wsky_Method_create(): Invalid parameter count %d",
            parameterCount);
    abort();
  }
  return method;
}

wsky_ReturnValue wsky_Method_call(const wsky_Method *method,
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

void wsky_Method_printDebug(const wsky_Method *this) {
  printf("name: %s\n", this->name);
  printf("parameter count: %d\n", this->parameterCount);
  printf("address: %p\n", this->function);
}



void wsky_MethodList_init(wsky_MethodList *this, int maxCount) {
  this->methods = malloc(sizeof(wsky_Method) * (unsigned) maxCount);
  if (!this->methods)
    abort();
  this->count = 0;
  this->available = maxCount;
}

void wsky_MethodList_free(wsky_MethodList *this) {
  free(this->methods);
}

void wsky_MethodList_add(wsky_MethodList *this, wsky_Method *method) {
  if (this->count == this->available) {
    fprintf(stderr, "wsky_MethodList_add(): Cannot add\n");
    abort();
  }
  this->methods[this->count] = *method;
  this->count++;
}

void wsky_MethodList_addNew(wsky_MethodList *this,
                             const char *name,
                             int parameterCount,
                             void *function) {
  wsky_Method m = wsky_Method_create(name, parameterCount, function);
  wsky_MethodList_add(this, &m);
}

void wsky_MethodList_printDebug(const wsky_MethodList *this) {
  int i;
  printf("MethodList {\n");
  for (i = 0; i < this->count; i++) {
    wsky_Method_printDebug(this->methods + i);
    printf("\n");
  }
  printf("}\n");
}
