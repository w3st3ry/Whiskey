#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include "object.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Exception Exception
 * @{
 */


extern const wsky_ClassDef wsky_Exception_CLASS_DEF;
extern wsky_Class *wsky_Exception_CLASS;



/**
 * The base of the wsky_Exception class.
 *
 * The subclasses of wsky_Exception should include this macro.
 */
# define wsky_Exception_HEAD                            \
  wsky_OBJECT_HEAD                                      \
                                                        \
  /** A message describing the exception or NULL */     \
  char *message;                                        \
                                                        \
  /** The cause of the exception or NULL */             \
  wsky_Exception *cause;


/**
 * An exception
 */
struct wsky_Exception_s {
  wsky_Exception_HEAD
};


/**
 * Creates a new exception.
 *
 * @param message The message or NULL
 * @param cause The cause or NULL
 */
wsky_Exception *wsky_Exception_new(const char *message,
                                   wsky_Exception *cause);

/**
 * Prints an exception to the standard output
 */
void wsky_Exception_print(const wsky_Exception *self);

/**
 * @}
 * @}
 */

#endif /* !EXCEPTION_H_ */
