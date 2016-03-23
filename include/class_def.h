#ifndef CLASSDEF_H_
# define CLASSDEF_H_

# include "method_def.h"

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

/** Initializes the builtin classes - called by wsky_start() */
void wsky_initBuiltinClasses(void);

/** Frees the builtin classes - called by wsky_stop() */
void wsky_freeBuiltinClasses(void);

/**
 * @}
 */

#endif /* !CLASSDEF_H_ */
