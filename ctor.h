#ifndef CTOR_H_
# define CTOR_H_

#include "return_value.h"

typedef void (*degat_Destructor)(degat_Object *object);
typedef degat_Exception *(*degat_Constructor)(degat_Object *object,
					      unsigned paramCount,
					      degat_Value *params);

#endif /* !CTOR_H_ */
