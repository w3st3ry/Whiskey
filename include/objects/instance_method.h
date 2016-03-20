#ifndef INSTANCE_METHOD_H_
# define INSTANCE_METHOD_H_

# include "object.h"
# include "class_def.h"
# include "objects/method.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup InstanceMethod InstanceMethod
 * @{
 */


extern const wsky_ClassDef wsky_InstanceMethod_CLASS_DEF;

extern wsky_Class *wsky_InstanceMethod_CLASS;


/**
 * A method linked to an instance of its class.
 */
typedef struct wsky_InstanceMethod_s wsky_InstanceMethod;
struct wsky_InstanceMethod_s {
  wsky_OBJECT_HEAD

  /**
   * The method
   */
  wsky_Method *method;

  /**
   * The `self` object, the instance of the class the method is defined in
   */
  wsky_Value self;
};


wsky_InstanceMethod *wsky_InstanceMethod_new(wsky_Method *method,
                                             wsky_Value self);

bool wsky_isInstanceMethod(const wsky_Value value);

/**
 * @}
 * @}
 */

#endif /* !INSTANCE_METHOD_H_ */
