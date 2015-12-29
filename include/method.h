#ifndef METHOD_H_
# define METHOD_H_

# include "return_value.h"

/**
 * @defgroup MethodDef MethodDef
 * @addtogroup MethodDef
 * @{
 *
 */

/**
 * A method with a variable parameter count.
 *
 * @param self The object of the method
 * @param parameterCount The parameter count
 * @param parameters A pointer to the list of the parameters
 */
typedef wsky_ReturnValue (*wsky_VariadicMethod)(wsky_Object *self,
                                                unsigned parameterCount,
                                                wsky_Value *parameters);

typedef wsky_ReturnValue (*wsky_Method0)(wsky_Object *o);
typedef wsky_ReturnValue (*wsky_Method1)(wsky_Object *o,
                                         wsky_Value *a);
typedef wsky_ReturnValue (*wsky_Method2)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b);
typedef wsky_ReturnValue (*wsky_Method3)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c);
typedef wsky_ReturnValue (*wsky_Method4)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d);
typedef wsky_ReturnValue (*wsky_Method5)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e);
typedef wsky_ReturnValue (*wsky_Method6)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e,
                                         wsky_Value *f);
typedef wsky_ReturnValue (*wsky_Method7)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e,
                                         wsky_Value *f,
                                         wsky_Value *g);
typedef wsky_ReturnValue (*wsky_Method8)(wsky_Object *o,
                                         wsky_Value *a,
                                         wsky_Value *b,
                                         wsky_Value *c,
                                         wsky_Value *d,
                                         wsky_Value *e,
                                         wsky_Value *f,
                                         wsky_Value *g,
                                         wsky_Value *h);

/**
 * A method definition.
 */
typedef struct wsky_MethodDef_s {

  /** The name of the method. */
  const char *name;

  /** The parameter count, or -1 if variable parameter count. */
  int parameterCount;

  /**
   * A pointer to the underlying C function.
   *
   * When the method is called, it will be casted in ::wsky_VariadicMethod
   * or wsky_Method<i>N</i>, where *N* is the parameter count.
   */
  void *function;
} wsky_MethodDef;

/**
 * Calls a method.
 *
 * @param method The method to call
 * @param object The 'self' object
 * @param parameterCount The parameter count
 * @param parameters A pointer to an array of the parameters
 */
wsky_ReturnValue wsky_MethodDef_call(const wsky_MethodDef *method,
                                     wsky_Object *object,
                                     unsigned parameterCount,
                                     wsky_Value *parameters);

/**
 * Prints a method for debugging purposes.
 *
 * @param self The method to print
 */
void wsky_MethodDef_printDebug(const wsky_MethodDef *self);


/**
 * @}
 *
 * @defgroup MethodList MethodList
 * @addtogroup MethodList
 * @{
 *
 */


/**
 * A list of @link wsky_MethodDef_s wsky_MethodDef @endlink.
 */
typedef struct wsky_MethodList_s {

  /** A malloc'd array of the methods */
  wsky_MethodDef *methods;

  /** The method count */
  unsigned count;

  /** The maximum method count (the malloc'd size) */
  unsigned available;
} wsky_MethodList;

/**
 * Initializes a wsky_MethodList.
 *
 * @param maxCount The maximum method count
 */
void wsky_MethodList_init(wsky_MethodList *self, unsigned maxCount);

/**
 * Frees a wsky_MethodList.
 */
void wsky_MethodList_free(wsky_MethodList *self);

/**
 * Adds a method to the given list.
 */
void wsky_MethodList_add(wsky_MethodList *self, wsky_MethodDef *method);

/**
 * Prints the list for debugging purposes.
 */
void wsky_MethodList_printDebug(const wsky_MethodList *self);

/**
 * @}
 */

#endif /* !METHOD_H_ */
