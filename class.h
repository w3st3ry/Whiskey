#ifndef CLASS_H_
# define CLASS_H_

#include "ctor.h"
#include "method.h"
#include <stdint.h>

struct wsky_Class_s;
typedef struct wsky_Class_s wsky_Class;

struct wsky_Class_s
{
  const struct wsky_Class_s *super;
  const char *name;
  wsky_Constructor constructor;
  wsky_Destructor destructor;
  uint32_t objectSize;
  wsky_MethodList methods;
};


void wsky_init(void);
void wsky_free(void);

#endif /* !CLASS_H_ */
