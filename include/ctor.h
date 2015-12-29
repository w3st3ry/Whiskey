#ifndef CTOR_H_
# define CTOR_H_

#include "return_value.h"

struct wsky_Object_s;
# define wsky_Object struct wsky_Object_s

typedef void (*wsky_Destructor)(wsky_Object *object);
typedef wsky_Exception *(*wsky_Constructor)(wsky_Object *object,
                                              unsigned paramCount,
                                              wsky_Value *params);

# undef wsky_Object

#endif /* !CTOR_H_ */
