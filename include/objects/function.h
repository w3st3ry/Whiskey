#ifndef FUNCTION_H_
# define FUNCTION_H_

# include "object.h"
# include "class_def.h"
# include "ast.h"
# include "eval.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Function Function
 * @{
 */


extern const wsky_ClassDef wsky_Function_CLASS_DEF;

extern wsky_Class *wsky_Function_CLASS;


/**
 * A Whiskey function.
 */
typedef struct wsky_Function_s {
  wsky_OBJECT_HEAD

  /** The name of the function or NULL if anonymous */
  char *name;

  /** The 'external' scope where the function is defined */
  wsky_Scope *globalScope;

  /** The duplicated AST node of the function */
  wsky_FunctionNode *node;
} wsky_Function;


/**
 * Creates a new function.
 * @param name The name or NULL if the function is anonymous
 * @param node The AST node of the function
 * @param globalScope The 'external' scope where the function is defined
 */
wsky_Function *wsky_Function_new(const char *name,
                                 const wsky_FunctionNode *node,
                                 wsky_Scope *globalScope);

wsky_ReturnValue wsky_Function_call(wsky_Function *function,
                                    wsky_Class *class,
                                    wsky_Object *self,
                                    unsigned parameterCount,
                                    const wsky_Value *parameters);

static inline bool wsky_isFunction(const wsky_Value value) {
  if (value.type != wsky_Type_OBJECT)
    return false;
  return value.v.objectValue->class == wsky_Function_CLASS;
}

/**
 * @}
 * @}
 */

#endif /* !FUNCTION_H_ */
