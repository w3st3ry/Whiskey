#include "method.h"

#include <stdlib.h>
#include <stdio.h>

degat_Method degat_Method_create(const char *name,
				 int parameterCount,
				 void *function) {
  degat_Method method = {
    .name = name,
    .parameterCount = parameterCount,
    .function = function
  };
  if (parameterCount < 0 && parameterCount != -1) {
    fprintf(stderr,
	    "degat_Method_create(): Invalid parameter count %d",
	    parameterCount);
    abort();
  }
  return (method);
}

degat_ReturnValue degat_Method_call(const degat_Method *method,
				    degat_Object *object,
				    unsigned parameterCount,
				    degat_Value *parameters) {
  void *m = method->function;
  switch (method->parameterCount) {
  case -1:
    return ((degat_VariadicMethod) m)(object,
				      parameterCount,
				      parameters);
  case 0:
    return ((degat_Method0) m)(object);
  case 1:
    return ((degat_Method1) m)(object,
			       parameters);
  case 2:
    return ((degat_Method2) m)(object,
			       parameters,
			       parameters + 1);
  case 3:
    return ((degat_Method3) m)(object,
			       parameters,
			       parameters + 1,
			       parameters + 2);
  case 4:
    return ((degat_Method4) m)(object,
			       parameters,
			       parameters + 1,
			       parameters + 2,
			       parameters + 3);
  case 5:
    return ((degat_Method5) m)(object,
			       parameters,
			       parameters + 1,
			       parameters + 2,
			       parameters + 3,
			       parameters + 4);
  default:
    fprintf(stderr, "degat_Method_call(): Too many parameters\n");
    abort();
  }
}

void degat_Method_printDebug(const degat_Method *this) {
  printf("name: %s\n", this->name);
  printf("parameter count: %d\n", this->parameterCount);
  printf("address: %p\n", this->function);
}



void degat_MethodList_init(degat_MethodList *this, int maxCount) {
  this->methods = malloc(sizeof(degat_Method) * (unsigned) maxCount);
  if (!this->methods)
    abort();
  this->count = 0;
  this->available = maxCount;
}

void degat_MethodList_free(degat_MethodList *this) {
  free(this->methods);
}

void degat_MethodList_add(degat_MethodList *this, degat_Method *method) {
  if (this->count == this->available) {
    fprintf(stderr, "degat_MethodList_add(): Cannot add\n");
    abort();
  }
  this->methods[this->count] = *method;
  this->count++;
}

void degat_MethodList_addNew(degat_MethodList *this,
			     const char *name,
			     int parameterCount,
			     void *function) {
  degat_Method m = degat_Method_create(name, parameterCount, function);
  degat_MethodList_add(this, &m);
}

void degat_MethodList_printDebug(const degat_MethodList *this) {
  int i;
  printf("MethodList {\n");
  for (i = 0; i < this->count; i++) {
    degat_Method_printDebug(this->methods + i);
    printf("\n");
  }
  printf("}\n");
}
