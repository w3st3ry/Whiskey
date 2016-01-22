#ifndef OBJECT_H_
# define OBJECT_H_

# include "return_value.h"
# include "dict.h"

/**
 * @defgroup objects objects
 * Whiskey objects
 * @addtogroup objects
 * @{
 *
 * @defgroup Object Object
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
  struct wsky_Object_s *gcNext;                                         \
                                                                        \
  /** Used by the garbage collector. Points to the previous object. */  \
  struct wsky_Object_s *gcPrevious;                                     \
                                                                        \
  /** The class of the object. */                                       \
  struct wsky_Class_s *class;


/**
 * A Whiskey object.
 *
 * Objects are always allocated on the heap with wsky_Object_new().
 * They must be used exclusively with pointers.
 *
 * Every Whiskey object extends `Object` and includes #wsky_OBJECT_HEAD.
 * You can cast every Whiskey object to a wsky_Object.
 *
 * Integers, booleans and float are not objects. They looks like objects
 * from Whiskey code, but they are allocated on the stack and represented
 * with `wsky_Value` struct.
 */
struct wsky_Object_s {
  wsky_OBJECT_HEAD

  /** The private fields */
  wsky_Dict fields;
};


struct wsky_ClassDef_s;


extern const struct wsky_ClassDef_s wsky_Object_CLASS_DEF;

/**
 * The superclass of every objects.
 */
extern wsky_Class *wsky_Object_CLASS;



/**
 * Creates a new Whiskey object of the given class.
 *
 * Calls the class constructor with the given parameters.
 */
wsky_ReturnValue wsky_Object_new(wsky_Class *class,
                                 unsigned parameterCount,
                                 wsky_Value *params);


/* Forward declaration */
extern wsky_Class *wsky_Null_CLASS;

/** Returns the class of the given object */
static inline wsky_Class *wsky_Object_getClass(wsky_Object *o) {
  return o ? o->class : wsky_Null_CLASS;
}

/** Returns the class name of the given object */
const char *wsky_Object_getClassName(wsky_Object *o);


struct wsky_Method_s;

/**
 * Finds a public method.
 *
 * Returns the wsky_MethodObject associated with the given name or `NULL`.
 */
struct wsky_Method_s *wsky_Object_findMethod(wsky_Object *o,
                                             const char *name);

/**
 * Finds a public method or getter.
 *
 * Returns the wsky_MethodObject associated with the given name or `NULL`.
 */
struct wsky_Method_s *wsky_Object_findMethodOrGetter(wsky_Object *object,
                                                     const char *name);

/**
 * Finds a public setter.
 *
 * Returns the wsky_MethodObject associated with the given name or `NULL`.
 */
struct wsky_Method_s *wsky_Object_findSetter(wsky_Object *object,
                                             const char *name);

/** Calls a getter */
wsky_ReturnValue wsky_Object_get(wsky_Object *object, const char *name);
wsky_ReturnValue wsky_Object_getPrivate(wsky_Object *object,
                                        const char *name);

/**
 * Calls a setter or sets a field directly.
 *
 * The behaviour of this function is the following:
 *
 * *When a set occurs inside of the class:*
 *
 * If a setter is defined, the setter is called.  Otherwise, the
 * field of the object is set or created.
 *
 * *When a set occurs out of the class:*
 *
 * The public setter is called. If there is no public setter,
 * an error is raised.
 *
 * *When a setter is called:*
 *
 * If the function of the setter is defined, this function is called.
 * Otherwise, the field of the object is set or created.
 *
 * @param privateAccess Allows access to the private fields. `true`
 * if the set occured inside of the class.
 */
wsky_ReturnValue wsky_Object_set(wsky_Object *object,
                                 const char *name,
                                 const wsky_Value *value,
                                 bool privateAccess);

/**
 * Calls a method with the given parameters.
 *
 * Raises an exception if the method does not exist.
 */
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

/** Returns a wsky_String or an exception */
wsky_ReturnValue wsky_Object_toString(wsky_Object *object);

/**
 * @}
 * @}
 */

#endif /* !OBJECT_H_ */
