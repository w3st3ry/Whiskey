#ifndef CLASSDEF_H_
# define CLASSDEF_H_

# include "method_def.h"
# include "whiskey.h"
# include  <stddef.h>

/**
 * @defgroup ClassDef ClassDef
 * @{
 */

/**
 * A function which accepts the garbage collector - visitor pattern.
 */
typedef void (*wsky_GCAcceptFunction)(wsky_Object *);


/**
 * A class definition, for a class written in C.
 */
typedef struct wsky_ClassDef_s {

  /** The super class definition or `NULL` */
  const struct wsky_ClassDef_s *super;

  /** The name of the class */
  const char *name;

  /** False if another class can extend this class */
  bool final;

  /** The constructor - never call it directly */
  wsky_VariadicMethod constructor;

  /** The destructor - never call it directly */
  wsky_Method0 destructor;

  /** The size of the instances of the class */
  size_t objectSize;

  /** A null-terminated array of the methods */
  wsky_MethodDef *methodDefs;

  /** A function which lets the garbage collector to visit the object */
  wsky_GCAcceptFunction gcAcceptFunction;
} wsky_ClassDef;


/** A list of the classes */
typedef struct {
  wsky_Class **classes;

  /** The class count */
  size_t count;
} wsky_ClassArray;

/** Returns a pointer to a list of the builtin classes */
const wsky_ClassArray *wsky_getBuiltinClasses(void);

void wsky_initBuiltinClasses(void);
void wsky_freeBuiltinClasses(void);

/**
 * @}
 */

#endif /* !CLASSDEF_H_ */
