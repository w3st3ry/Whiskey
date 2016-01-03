#ifndef METHOD_OBJECT_H
# define METHOD_OBJECT_H

# include "object.h"
# include "objects/function.h"
# include "method_def.h"


extern const wsky_ClassDef wsky_MethodObject_CLASS_DEF;

extern wsky_Class *wsky_MethodObject_CLASS;


typedef struct wsky_MethodObject_s {
  wsky_OBJECT_HEAD

  /** The name of the method. */
  char *name;

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
                                         wsky_Object *self);

wsky_ReturnValue wsky_MethodObject_call1(wsky_MethodObject *method,
                                         wsky_Object *self,
                                         wsky_Value a);


#endif /* METHOD_OBJECT_H */
