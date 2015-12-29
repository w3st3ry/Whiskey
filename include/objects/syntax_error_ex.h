#ifndef SYNTAX_ERROR_EX_H_
# define SYNTAX_ERROR_EX_H_


# include "exception.h"
# include "syntax_error.h"

/*
 * The following structure is called `wsky_SyntaxErrorEx` because
 * there is already another structure called `SyntaxError`.
 *
 * `wsky_SyntaxError` is a plain C structure without garbage-collection,
 * this one is a garbage-collected exception.
 *
 * The "real" class name of `wsky_SyntaxErrorEx` (the class name visible
 * from Whiskey code) is `SyntaxError`. The trailing "Ex" is in C code
 * only.
 *
 */

extern wsky_Class wsky_SyntaxErrorEx_CLASS;

typedef struct wsky_SyntaxErrorEx_s wsky_SyntaxErrorEx;

struct wsky_SyntaxErrorEx_s {
  wsky_Exception_HEAD
  wsky_SyntaxError syntaxError;
};


wsky_SyntaxErrorEx *wsky_SyntaxErrorEx_new(wsky_SyntaxError *syntaxError);


#endif /* !SYNTAX_ERROR_EX_H_ */