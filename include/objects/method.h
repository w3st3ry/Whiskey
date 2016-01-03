#ifndef METHOD_H
# define METHOD_H

# include "object.h"
# include "objects/function.h"
# include "method_def.h"


typedef struct wsky_MethodObject_s {

  /** The name of the method. */
  const char *name;

  /** The flags */
  wsky_MethodFlags flags;

  /** The underlying Whiskey function or NULL */
  wsky_Function *wskyMethod;

  /** A pointer to the underlying C method definition or NULL */
  wsky_MethodDef *cMethod;

} wsky_MethodObject;


wsky_MethodObject *wsky_MethodObject_newFromC(wsky_MethodDef *cMethod);

wsky_MethodObject *wsky_MethodObject_newFromWsky(wsky_Function *wskyMethod,
                                                 wsky_MethodFlags flags);

wsky_ReturnValue wsky_MethodObject_call(wsky_MethodObject *method,
                                        wsky_Object *self,
                                        unsigned parameterCount,
                                        wsky_Value *parameters);

wsky_ReturnValue wsky_MethodObject_call0(wsky_MethodObject *method,
                                         wsky_Object *self,
                                         wsky_Value a);


#endif /* METHOD_H */
