#ifndef BOOLEAN_H_
# define BOOLEAN_H_

# include "object.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Boolean Boolean
 * @{
 */

extern const wsky_ClassDef wsky_Boolean_CLASS_DEF;

extern wsky_Class *wsky_Boolean_CLASS;

static inline bool wsky_isBoolean(const wsky_Value value) {
  return (value.type == wsky_Type_BOOL);
}

/**
 * @}
 * @}
 */

#endif /* !BOOLEAN_H_ */
