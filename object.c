#include "object.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "return_value.h"
#include "gc.h"


const wsky_Class wsky_Object_CLASS;



wsky_ReturnValue wsky_Object_new(const wsky_Class *class,
                                   unsigned paramCount,
                                   wsky_Value *params) {
  wsky_Object *object = wsky_MALLOC(class->objectSize);
  if (!object)
    return wsky_ReturnValue_NULL;
  object->class = class;
  wsky_Exception *exception = class->constructor(object, paramCount, params);
  if (exception) {
    wsky_RETURN_EXCEPTION(exception);
  }

  wsky_GC_register(object);

  wsky_RETURN_OBJECT(object);
}


const wsky_MethodDef *wsky_Object_findMethod(wsky_Object *object,
                                             const char *methodName) {
  const wsky_Class *class = object->class;
  const wsky_MethodList *methods = &class->methods;
  unsigned i;
  for (i = 0; i < methods->count; i++) {
    const wsky_MethodDef *method = methods->methods + i;
    if (strcmp(method->name, methodName) == 0) {
      return method;
    }
  }
  return NULL;
}

wsky_ReturnValue wsky_Object_callMethod(wsky_Object *object,
                                          const char *methodName,
                                          unsigned parameterCount,
                                          wsky_Value *parameters) {
  const wsky_MethodDef *method = wsky_Object_findMethod(object, methodName);
  if (!method) {
    fprintf(stderr,
            "wsky_Object_callMethod(): Unknow method %s\n",
            methodName);
    abort();
  }
  if (method->parameterCount != -1 &&
      method->parameterCount != (int) parameterCount) {
    fprintf(stderr,
            "wsky_Object_callMethod(): Invalid parameter count\n");
      abort();
  }
  return wsky_MethodDef_call(method, object, parameterCount, parameters);
}

wsky_ReturnValue wsky_Object_callMethod0(wsky_Object *object,
                                           const char *methodName) {
  return wsky_Object_callMethod(object, methodName, 0, NULL);
}

wsky_ReturnValue wsky_Object_callMethod1(wsky_Object *object,
                                           const char *methodName,
                                           wsky_Value a) {
  return wsky_Object_callMethod(object, methodName, 1, &a);
}
