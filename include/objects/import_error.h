#ifndef IMPORT_ERROR_H
# define IMPORT_ERROR_H

# include "exception.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup ImportError ImportError
 * @{
 */


extern const wsky_ClassDef wsky_ImportError_CLASS_DEF;

extern wsky_Class *wsky_ImportError_CLASS;


typedef struct wsky_ImportError_s wsky_ImportError;

/** An ImportError */
struct wsky_ImportError_s {
  wsky_Exception_HEAD
};



wsky_ImportError *wsky_ImportError_new(const char *message);

/**
 * @}
 * @}
 */

#endif /* IMPORT_ERROR_H */
