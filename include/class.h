#ifndef CLASS_H_
# define CLASS_H_

# include "ctor.h"
# include "method.h"
# include <stdint.h>

/**
 * @defgroup Class Class
 * @{
 */

/**
 * A function which accepts the garbage collector - visitor pattern.
 */
typedef void (*wsky_GCAcceptFunction)(wsky_Object *);


/**
 * The class of a Whiskey object
 */
struct wsky_Class_s {

  /** The super class or `NULL` */
  const struct wsky_Class_s *super;

  const char *name;

  wsky_Constructor constructor;
  wsky_Destructor destructor;

  /** The size of the instances of the class */
  uint32_t objectSize;

  wsky_MethodDef *methodDefs;
  wsky_MethodList methods;

  wsky_GCAcceptFunction gcAcceptFunction;
};


const wsky_MethodDef *wsky_Class_findLocalMethod(const wsky_Class *class,
                                                 const char *methodName);

const wsky_MethodDef *wsky_Class_findMethod(const wsky_Class *class,
                                            const char *methodName);

/**
 * @}
 */

/** Starts Whiskey */
void wsky_start(void);

/** Stops Whiskey */
void wsky_stop(void);

#endif /* !CLASS_H_ */
