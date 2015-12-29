#ifndef OBJECT_H_
# define OBJECT_H_

# include <stdint.h>
# include "class.h"

/**
 * \defgroup objects objects
 * \addtogroup objects
 * @{
 *
 * \defgroup Object Object
 * \addtogroup Object
 * @{
 *
 */

/**
 * This macro must be included at the first line of any object structure.
 *
 * `gcMark`: Used by the garbage collector only.
 *
 * `gcNext` and `gcPrevious`: A linked list of all the objects allocaded
 * on the heap. Used by the garbage collector.
 *
 * `class`: The class of the object.
 *
 */
# define wsky_OBJECT_HEAD                                               \
                                                                        \
  /** Used by the garbage collector only. */                            \
  bool gcMark;                                                          \
                                                                        \
  /** Used by the garbage collector. Points to the next object. */      \
  wsky_Object *gcNext;                                                  \
                                                                        \
  /** Used by the garbage collector. Points to the previous object. */  \
  wsky_Object *gcPrevious;                                              \
                                                                        \
  /** The class of the object. */                                       \
  const wsky_Class *class;



struct wsky_Object_s {
  wsky_OBJECT_HEAD
};

/**
 * The superclass of every objects
 */
extern wsky_Class wsky_Object_CLASS;



wsky_ReturnValue wsky_Object_new(const wsky_Class *class,
                                 unsigned paramCount,
                                 wsky_Value *params);

/** Returns the `Method` associated with the given name or `NULL` */
const wsky_MethodDef *wsky_Object_findMethod(wsky_Object *object,
                                             const char *methodName);

/** Calls a method */
wsky_ReturnValue wsky_Object_callMethod(wsky_Object *object,
                                        const char *methodName,
                                        unsigned parameterCount,
                                        wsky_Value *parameters);

/** Calls a method without parameter */
wsky_ReturnValue wsky_Object_callMethod0(wsky_Object *object,
                                         const char *methodName);

/** Calls a method with one parameter */
wsky_ReturnValue wsky_Object_callMethod1(wsky_Object *object,
                                         const char *methodName,
                                         wsky_Value a);

/** Calls a method with two parameters */
wsky_ReturnValue wsky_Object_callMethod2(wsky_Object *object,
                                         const char *methodName,
                                         wsky_Value a,
                                         wsky_Value b);

/** Calls a method with three parameters */
wsky_ReturnValue wsky_Object_callMethod3(wsky_Object *object,
                                    const char *methodName,
                                    wsky_Value a,
                                    wsky_Value b,
                                    wsky_Value c);

/**
 * @}
 * @}
 */

#endif /* !OBJECT_H_ */
