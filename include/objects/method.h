#ifndef METHOD_OBJECT_H
# define METHOD_OBJECT_H

# include "object.h"
# include "objects/function.h"
# include "method_def.h"


extern const wsky_ClassDef wsky_Method_CLASS_DEF;

extern wsky_Class *wsky_Method_CLASS;

/** A method */
typedef struct wsky_Method_s {
  wsky_OBJECT_HEAD

  /** The name of the method. */
  char *name;

  /** The class where the method is defined. */
  wsky_Class *defClass;

  /** The flags */
  wsky_MethodFlags flags;

  /**
   * The underlying function or NULL if the method is an empty getter
   * and setter
   */
  wsky_Function *function;

} wsky_Method;

/** Creates a new method from a C function */
wsky_Method *wsky_Method_newFromC(const wsky_MethodDef *cMethod,
                                  wsky_Class *class);

/** Creates a new method from a Whiskey function */
wsky_Method *wsky_Method_newFromWsky(wsky_Function *wskyMethod,
                                     wsky_MethodFlags flags,
                                     wsky_Class *class);

/** Creates a default getter or setter */
wsky_Method *wsky_Method_newFromWskyDefault(const char *name,
                                            wsky_MethodFlags flags,
                                            wsky_Class *class);

/**
 * Returns true if the method is a default getter or setter
 */
static inline bool wsky_Method_isDefault(const wsky_Method *method) {
  return !method->function;
}

wsky_ReturnValue wsky_Method_call(wsky_Method *method,
                                  wsky_Object *self,
                                  unsigned parameterCount,
                                  const wsky_Value *parameters);

wsky_ReturnValue wsky_Method_call0(wsky_Method *method,
                                   wsky_Object *self);

wsky_ReturnValue wsky_Method_call1(wsky_Method *method,
                                   wsky_Object *self,
                                   wsky_Value a);


wsky_ReturnValue wsky_Method_callValue(wsky_Method *method,
                                       wsky_Value self,
                                       unsigned parameterCount,
                                       const wsky_Value *parameters);

wsky_ReturnValue wsky_Method_callValue0(wsky_Method *method,
                                        wsky_Value self);

wsky_ReturnValue wsky_Method_callValue1(wsky_Method *method,
                                        wsky_Value self,
                                        wsky_Value a);


#endif /* METHOD_OBJECT_H */
