#ifndef SYNTAX_ERROR_EX_H_
# define SYNTAX_ERROR_EX_H_

# include "exception.h"
# include "syntax_error.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup SyntaxErrorEx SyntaxErrorEx
 * @{
 */


extern const wsky_ClassDef wsky_SyntaxErrorEx_CLASS_DEF;

extern wsky_Class *wsky_SyntaxErrorEx_CLASS;


/**
 * Represents a SyntaxError.
 *
 * The following structure is called @link wsky_SyntaxErrorEx_s
 * wsky_SyntaxErrorEx @endlink because there is already another
 * structure called @link wsky_SyntaxError_s wsky_SyntaxError@endlink.
 *
 * `wsky_SyntaxError` is a plain C structure without garbage-collection,
 * this one is a garbage-collected exception.
 *
 * The "real" class name of `wsky_SyntaxErrorEx` (the class name visible
 * from Whiskey code) is `SyntaxError`. The trailing "Ex" is in C code
 * only.
 *
 * @sa syntax_error.h @link wsky_SyntaxError_s wsky_SyntaxError @endlink
 */
typedef struct wsky_SyntaxErrorEx_s {
  wsky_Exception_HEAD

  /** The underlying wsky_SyntaxError */
  wsky_SyntaxError syntaxError;

} wsky_SyntaxErrorEx;


wsky_SyntaxErrorEx *wsky_SyntaxErrorEx_new(const wsky_SyntaxError *e);

/**
 * @}
 * @}
 */

#endif /* !SYNTAX_ERROR_EX_H_ */
