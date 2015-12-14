#ifndef CLASS_H_
# define CLASS_H_

#include "ctor.h"
#include "method.h"
#include <stdint.h>

struct wsky_Class_s;
typedef struct wsky_Class_s wsky_Class;

struct wsky_Class_s {

  /** The super class or `NULL` */
  const struct wsky_Class_s *super;

  const char *name;

  wsky_Constructor constructor;
  wsky_Destructor destructor;

  /** The size of the instances of the class */
  uint32_t objectSize;

  wsky_MethodList methods;
};



/** Initializes Whiskey */
void wsky_init(void);

/** Stops Whiskey */
void wsky_free(void);

#endif /* !CLASS_H_ */
