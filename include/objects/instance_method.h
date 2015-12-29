#ifndef INSTANCE_METHOD_H_
# define INSTANCE_METHOD_H_

# include "object.h"

extern wsky_Class wsky_InstanceMethod_CLASS;

typedef struct wsky_InstanceMethod_s wsky_InstanceMethod;
struct wsky_InstanceMethod_s {
  wsky_OBJECT_HEAD

  const wsky_MethodDef *method;
  wsky_Object *self;
};


wsky_InstanceMethod *wsky_InstanceMethod_new(const wsky_MethodDef *method,
                                             wsky_Object *self);

bool wsky_isInstanceMethod(const wsky_Value value);


#endif /* !INSTANCE_METHOD_H_ */
