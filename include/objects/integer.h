#ifndef INTEGER_H_
# define INTEGER_H_

# include "object.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Integer Integer
 * @{
 */

extern const wsky_ClassDef wsky_Integer_CLASS_DEF;

extern wsky_Class *wsky_Integer_CLASS;

static inline bool wsky_isInteger(const wsky_Value value) {
  return value.type == wsky_Type_INT;
}

/**
 * @}
 * @}
 */

#endif /* !INTEGER_H_ */
