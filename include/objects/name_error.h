#ifndef NAME_ERROR_H
#define NAME_ERROR_H

# include "exception.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup NameError NameError
 * @{
 */

extern const wsky_ClassDef wsky_NameError_CLASS_DEF;

extern wsky_Class *wsky_NameError_CLASS;


/** A NameError */
typedef struct wsky_NameError_s {
  wsky_Exception_HEAD
} wsky_NameError;


wsky_NameError *wsky_NameError_new(const char *message);

/**
 * @}
 * @}
 */

#endif /* NAME_ERROR_H */
