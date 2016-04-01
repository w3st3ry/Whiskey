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
 * @param params A pointer to the list of the parameters
 */
typedef wsky_Result (*wsky_VariadicMethod)(wsky_Object *self,
                                           unsigned parameterCount,
                                           const wsky_Value *params);


# ifndef DOXYGEN
typedef wsky_Result (*wsky_Method0)(wsky_Object *o);
typedef wsky_Result (*wsky_Method1)(wsky_Object *o,
                                    wsky_Value *a);
typedef wsky_Result (*wsky_Method2)(wsky_Object *o,
                                    wsky_Value *a,
                                    wsky_Value *b);
typedef wsky_Result (*wsky_Method3)(wsky_Object *o,
                                    wsky_Value *a,
                                    wsky_Value *b,
                                    wsky_Value *c);
typedef wsky_Result (*wsky_Method4)(wsky_Object *o,
                                    wsky_Value *a,
                                    wsky_Value *b,
                                    wsky_Value *c,
                                    wsky_Value *d);
typedef wsky_Result (*wsky_Method5)(wsky_Object *o,
                                    wsky_Value *a,
                                    wsky_Value *b,
                                    wsky_Value *c,
                                    wsky_Value *d,
                                    wsky_Value *e);
typedef wsky_Result (*wsky_Method6)(wsky_Object *o,
                                    wsky_Value *a,
                                    wsky_Value *b,
                                    wsky_Value *c,
                                    wsky_Value *d,
                                    wsky_Value *e,
                                    wsky_Value *f);
typedef wsky_Result (*wsky_Method7)(wsky_Object *o,
                                    wsky_Value *a,
                                    wsky_Value *b,
                                    wsky_Value *c,
                                    wsky_Value *d,
                                    wsky_Value *e,
                                    wsky_Value *f,
                                    wsky_Value *g);
typedef wsky_Result (*wsky_Method8)(wsky_Object *o,
                                    wsky_Value *a,
                                    wsky_Value *b,
                                    wsky_Value *c,
                                    wsky_Value *d,
                                    wsky_Value *e,
                                    wsky_Value *f,
                                    wsky_Value *g,
                                    wsky_Value *h);
# endif

/** A bitfield describing the type of a method */
typedef int wsky_MethodFlags;

/** The default flag. A standard private method has this flag only. */
# define wsky_MethodFlags_DEFAULT       (0)

/** Marks a method as a public one. */
# define wsky_MethodFlags_PUBLIC        (1 << 0)

/** Marks a method as a getter. */
# define wsky_MethodFlags_GET           (1 << 1)

/** Marks a method as a setter. */
# define wsky_MethodFlags_SET           (1 << 2)

/**
 * The first argument of a method with this flag is a pointer to a
 * wsky_Value instead of a pointer to a wsky_Object. That's a hack,
 * but you should cast it.
 *
 * Used only in the classes of the primitive types (wsky_Integer,
 * wsky_Boolean, wsky_Float, ...), not in the classes of the real
 * objects allocated on the heap.
 */
# define wsky_MethodFlags_VALUE         (1 << 3)

/** Marks a method as an initializer (or constructor). */
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
  wsky_Method0 function;

} wsky_MethodDef;

/**
 * Calls a method.
 *
 * @param method The method to call
 * @param self The 'self' object
 * @param parameterCount The parameter count
 * @param parameters A pointer to an array of the parameters
 */
wsky_Result wsky_MethodDef_call(const wsky_MethodDef *method,
                                wsky_Object *self,
                                unsigned parameterCount,
                                const wsky_Value *parameters);

wsky_Result wsky_MethodDef_callValue(const wsky_MethodDef *method,
                                     wsky_Value self,
                                     unsigned parameterCount,
                                     const wsky_Value *parameters);

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
