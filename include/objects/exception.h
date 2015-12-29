#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include "object.h"

/**
 * \addtogroup objects
 * @{
 *
 * \defgroup Exception Exception
 * @{
 */

extern wsky_Class wsky_Exception_CLASS;



# define wsky_Exception_HEAD                    \
  wsky_OBJECT_HEAD                              \
  char *message;                                \
  wsky_Exception *cause;

struct wsky_Exception_s {
  wsky_Exception_HEAD
};



wsky_Exception *wsky_Exception_new(const char *message,
                                   wsky_Exception *cause);


void wsky_Exception_print(const wsky_Exception *self);

/**
 * @}
 * @}
 */

#endif /* !EXCEPTION_H_ */
