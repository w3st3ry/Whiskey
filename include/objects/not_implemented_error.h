#ifndef NOT_IMPLEMENTED_ERROR_H_
# define NOT_IMPLEMENTED_ERROR_H_

# include "exception.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup NotImplementedError NotImplementedError
 * @{
 */

extern const wsky_ClassDef wsky_NotImplementedError_CLASS_DEF;

extern wsky_Class *wsky_NotImplementedError_CLASS;

/**
 * A NotImplementedError.
 */
typedef struct wsky_NotImplementedError_s wsky_NotImplementedError;
struct wsky_NotImplementedError_s {
  wsky_Exception_HEAD
};


wsky_NotImplementedError *wsky_NotImplementedError_new(const char *message);

/**
 * @}
 * @}
 */

#endif /* !NOT_IMPLEMENTED_ERROR_H_ */
