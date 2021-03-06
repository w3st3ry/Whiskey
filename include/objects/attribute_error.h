#ifndef ATTRIBUTE_ERROR_H
# define ATTRIBUTE_ERROR_H

# include "exception.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup AttributeError AttributeError
 * @{
 */

extern const wsky_ClassDef wsky_AttributeError_CLASS_DEF;

extern wsky_Class *wsky_AttributeError_CLASS;

/**
 * A AttributeError.
 */
typedef struct wsky_AttributeError_s {
  wsky_Exception_HEAD
} wsky_AttributeError;


/**
 * Creates an AttributeError with a custom message.
 */
wsky_AttributeError *wsky_AttributeError_new(const char *message);

/**
 * When an attribute is not found in a class.
 */
wsky_AttributeError *wsky_AttributeError_newNoAttr(const char *className,
                                                   const char *attribute);

/**
 * Like wsky_AttributeError_newNoAttr(), but returns a
 * wsky_Result with the error raised.
 */
wsky_Result wsky_AttributeError_raiseNoAttr(const char *className,
                                                 const char *name);

/**
 * @}
 * @}
 */

#endif /* ATTRIBUTE_ERROR_H */
