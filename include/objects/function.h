#ifndef FUNCTION_H_
# define FUNCTION_H_

# include "object.h"
# include "ast.h"
# include "eval.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Function Function
 * @{
 */

extern wsky_Class wsky_Function_CLASS;


/**
 * A Whiskey function.
 */
typedef struct wsky_Function_s wsky_Function;
struct wsky_Function_s {
  wsky_OBJECT_HEAD

  /** The name of the function or NULL if anonymous */
  char *name;

  /** The 'external' scope where the fuction is defined */
  wsky_Scope *globalScope;

  /** The duplicated AST node of the function */
  wsky_FunctionNode *node;
};


/**
 * @param name The name or NULL if the function is anonymous
 */
wsky_Function *wsky_Function_new(const char *name,
                                 const wsky_FunctionNode *node,
                                 wsky_Scope *globalScope);

wsky_ReturnValue wsky_Function_call(wsky_Object *object,
                                    unsigned parameterCount,
                                    wsky_Value *parameters);

bool wsky_isFunction(const wsky_Value value);

/**
 * @}
 * @}
 */

#endif /* !FUNCTION_H_ */
