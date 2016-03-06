#ifndef FLOAT_H_
# define FLOAT_H_

# include "object.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Float Float
 * @{
 */

extern const wsky_ClassDef wsky_Float_CLASS_DEF;

extern wsky_Class *wsky_Float_CLASS;

static inline bool wsky_isFloat(const wsky_Value value) {
  return value.type == wsky_Type_FLOAT;
}

/**
 * @}
 * @}
 */

#endif /* !FLOAT_H_ */
