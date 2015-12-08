#include "object.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "return_value.h"

const degat_Class degat_Object_CLASS;



degat_ReturnValue degat_Object_new(const degat_Class *class,
				   unsigned paramCount,
				   degat_Value *params) {
  degat_Object *object = malloc(class->objectSize);
  if (!object)
    return degat_ReturnValue_NULL;
  object->gcReferenceCount = 1;
  object->class = class;
  degat_Exception *exception = class->constructor(object, paramCount, params);
  if (exception) {
    degat_RETURN_EXCEPTION(exception);
  }
  degat_RETURN_OBJECT(object);
}


const degat_Method *degat_Object_findMethod(degat_Object *object,
					    const char *methodName) {
  const degat_Class *class = object->class;
  const degat_MethodList *methods = &class->methods;
  int i;
  for (i = 0; i < methods->count; i++) {
    const degat_Method *method = methods->methods + i;
    if (strcmp(method->name, methodName) == 0) {
      return method;
    }
  }
  return NULL;
}

degat_ReturnValue degat_Object_callMethod(degat_Object *object,
					  const char *methodName,
					  unsigned parameterCount,
					  degat_Value *parameters) {
  const degat_Method *method = degat_Object_findMethod(object, methodName);
  if (!method) {
    fprintf(stderr,
	    "degat_Object_callMethod(): Unknow method %s\n",
	    methodName);
    abort();
  }
  if (method->parameterCount != -1 &&
      method->parameterCount != (int) parameterCount) {
    fprintf(stderr,
	    "degat_Object_callMethod(): Invalid parameter count\n");
      abort();
  }
  return degat_Method_call(method, object, parameterCount, parameters);
}

degat_ReturnValue degat_Object_callMethod0(degat_Object *object,
					   const char *methodName) {
  return degat_Object_callMethod(object, methodName, 0, NULL);
}

degat_ReturnValue degat_Object_callMethod1(degat_Object *object,
					   const char *methodName,
					   degat_Value a) {
  return degat_Object_callMethod(object, methodName, 1, &a);
}
