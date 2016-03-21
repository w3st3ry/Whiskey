#ifndef ZERO_DIVISION_ERROR_H
# define ZERO_DIVISION_ERROR_H

# include "exception.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup ZeroDivisionError ZeroDivisionError
 * @{
 */

extern const wsky_ClassDef wsky_ZeroDivisionError_CLASS_DEF;

extern wsky_Class *wsky_ZeroDivisionError_CLASS;


/** A ZeroDivisionError */
typedef struct wsky_ZeroDivisionError_s {
  wsky_Exception_HEAD
} wsky_ZeroDivisionError;


wsky_ZeroDivisionError *wsky_ZeroDivisionError_new(void);

/**
 * @}
 * @}
 */

#endif /* ZERO_DIVISION_ERROR_H */
