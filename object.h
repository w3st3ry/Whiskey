#ifndef OBJECT_H_
# define OBJECT_H_

#include <stdint.h>
#include "class.h"

# define degat_OBJECT_HEAD \
  uint32_t gcReferenceCount; \
  const degat_Class * class;



struct degat_Object_s {
  degat_OBJECT_HEAD
};

extern const degat_Class degat_Object_CLASS;



degat_ReturnValue degat_Object_new(const degat_Class *class,
				   unsigned paramCount,
				   degat_Value *params);

const degat_Method *degat_Object_findMethod(degat_Object *object,
					    const char *methodName);

degat_ReturnValue degat_Object_callMethod(degat_Object *object,
					  const char *methodName,
					  unsigned parameterCount,
					  degat_Value *parameters);
degat_ReturnValue degat_Object_callMethod0(degat_Object *object,
					   const char *methodName);
degat_ReturnValue degat_Object_callMethod1(degat_Object *object,
					   const char *methodName,
					   degat_Value a);

#endif /* !OBJECT_H_ */
