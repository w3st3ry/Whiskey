#ifndef ATTRIBUTE_ERROR_H
#define ATTRIBUTE_ERROR_H

# include "exception.h"
# include "class_def.h"

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


wsky_AttributeError *wsky_AttributeError_new(const char *message);

wsky_AttributeError *wsky_AttributeError_newNoAttr(const char *className,
                                                   const char *attribute);

wsky_ReturnValue wsky_AttributeError_raiseNoAttr(const char *className,
                                                 const char *name);

/**
 * @}
 * @}
 */

#endif /* ATTRIBUTE_ERROR_H */
