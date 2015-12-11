#ifndef OBJECT_H_
# define OBJECT_H_

#include <stdint.h>
#include "class.h"

# define wsky_OBJECT_HEAD \
  uint32_t gcReferenceCount; \
  const wsky_Class * class;



struct wsky_Object_s {
  wsky_OBJECT_HEAD
};

extern const wsky_Class wsky_Object_CLASS;



wsky_ReturnValue wsky_Object_new(const wsky_Class *class,
				   unsigned paramCount,
				   wsky_Value *params);

const wsky_Method *wsky_Object_findMethod(wsky_Object *object,
					    const char *methodName);

wsky_ReturnValue wsky_Object_callMethod(wsky_Object *object,
					  const char *methodName,
					  unsigned parameterCount,
					  wsky_Value *parameters);
wsky_ReturnValue wsky_Object_callMethod0(wsky_Object *object,
					   const char *methodName);
wsky_ReturnValue wsky_Object_callMethod1(wsky_Object *object,
					   const char *methodName,
					   wsky_Value a);

#endif /* !OBJECT_H_ */
