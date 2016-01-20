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


# ifndef DOXYGEN
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
# endif


typedef int wsky_MethodFlags;
# define wsky_MethodFlags_DEFAULT       (0)
# define wsky_MethodFlags_PUBLIC        (1 << 0)
# define wsky_MethodFlags_GET           (1 << 1)
# define wsky_MethodFlags_SET           (1 << 2)
# define wsky_MethodFlags_VALUE         (1 << 3)
# define wsky_MethodFlags_INIT          (1 << 4)


/**
 * A native method definition.
 */
typedef struct wsky_MethodDef_s {

  /** The name of the method. */
  const char *name;

  /** The parameter count, or -1 if variable parameter count. */
  int parameterCount;

  /** The flags */
  wsky_MethodFlags flags;

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
 * @param self The 'self' object
 * @param parameterCount The parameter count
 * @param parameters A pointer to an array of the parameters
 */
wsky_ReturnValue wsky_MethodDef_call(const wsky_MethodDef *method,
                                     wsky_Object *self,
                                     unsigned parameterCount,
                                     wsky_Value *parameters);

wsky_ReturnValue wsky_MethodDef_callValue(const wsky_MethodDef *method,
                                          wsky_Value self,
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
 */

#endif /* !METHOD_H_ */
