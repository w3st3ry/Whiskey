#ifndef CTOR_H_
# define CTOR_H_

#include "return_value.h"

typedef void (*wsky_Destructor)(wsky_Object *object);
typedef wsky_Exception *(*wsky_Constructor)(wsky_Object *object,
                                              unsigned paramCount,
                                              wsky_Value *params);

#endif /* !CTOR_H_ */
