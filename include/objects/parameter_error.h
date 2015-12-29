#ifndef PARAMETER_ERROR_H_
# define PARAMETER_ERROR_H_

# include "exception.h"

/**
 * \addtogroup objects
 * @{
 *
 * \defgroup ParameterError
 * \addtogroup ParameterError
 * @{
 */

extern wsky_Class wsky_ParameterError_CLASS;

typedef struct wsky_ParameterError_s wsky_ParameterError;

struct wsky_ParameterError_s {
  wsky_Exception_HEAD
};



wsky_ParameterError *wsky_ParameterError_new(const char *message);

/**
 * @}
 * @}
 */

#endif /* !PARAMETER_ERROR_H_ */
