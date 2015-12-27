#include "object.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "return_value.h"
#include "objects/null.h"
#include "gc.h"


typedef wsky_Object Object;
typedef wsky_Class Class;


const Class wsky_Object_CLASS;



wsky_ReturnValue wsky_Object_new(const wsky_Class *class,
                                 unsigned paramCount,
                                 wsky_Value *params) {
  Object *object = wsky_MALLOC(class->objectSize);
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


#define GET_CLASS(object) ((object) ? object->class : &wsky_Null_CLASS)

const wsky_MethodDef *wsky_Object_findMethod(Object *object,
                                             const char *methodName) {
  const wsky_Class *class = GET_CLASS(object);
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


wsky_ReturnValue wsky_Object_callMethod(Object *object,
                                        const char *methodName,
                                        unsigned parameterCount,
                                        wsky_Value *parameters) {

  const wsky_MethodDef *method = wsky_Object_findMethod(object, methodName);
  const wsky_Class *class = GET_CLASS(object);

  if (!method) {
    char message[64];
    snprintf(message, 63, "%s class has no method %s",
             class->name, methodName);
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  if (method->parameterCount != -1 &&
      method->parameterCount != (int) parameterCount) {

    char message[64];
    snprintf(message, 63, "Invalid parameter count");
    wsky_RETURN_NEW_EXCEPTION(message);
  }

  return wsky_MethodDef_call(method, object, parameterCount, parameters);
}


wsky_ReturnValue wsky_Object_callMethod0(Object *object,
                                         const char *methodName) {
  return wsky_Object_callMethod(object, methodName, 0, NULL);
}

wsky_ReturnValue wsky_Object_callMethod1(Object *object,
                                         const char *methodName,
                                         wsky_Value a) {
  return wsky_Object_callMethod(object, methodName, 1, &a);
}
