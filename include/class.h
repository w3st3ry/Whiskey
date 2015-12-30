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

  /** The name of the class */
  const char *name;

  /** The constructor - never call it directly */
  wsky_Constructor constructor;

  /** The destructor - never call it directly */
  wsky_Destructor destructor;

  /** The size of the instances of the class */
  uint32_t objectSize;

  /** A null-terminated array of the methods */
  wsky_MethodDef *methodDefs;

  /** A list of the methods */
  wsky_MethodList methods;

  /** A function which lets the garbage collector to visit the object */
  wsky_GCAcceptFunction gcAcceptFunction;
};


/**
 * Finds a method in a class, does not search in the superclasses.
 * Returns NULL if not found.
 */
const wsky_MethodDef *wsky_Class_findLocalMethod(const wsky_Class *class,
                                                 const char *methodName);

/**
 * Finds a method in a class, searches in the superclasses.
 * Returns NULL if not found.
 */
const wsky_MethodDef *wsky_Class_findMethod(const wsky_Class *class,
                                            const char *methodName);

/**
 * @}
 */

/**
 * @defgroup whiskey whiskey
 * @{
 */

/** Starts Whiskey */
void wsky_start(void);

/** Stops Whiskey */
void wsky_stop(void);

/**
 * @}
 */

#endif /* !CLASS_H_ */
