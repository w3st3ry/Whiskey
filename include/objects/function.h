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

  /**
   * The 'external' scope where the function is defined, or NULL if
   * the function is written is C
   */
  wsky_Scope *globalScope;

  /**
   * The duplicated AST node of the function, or NULL if the function
   * is written in C
   */
  wsky_FunctionNode *node;

  /** The underlying C method definition */
  wsky_MethodDef cMethod;
} wsky_Function;


/**
 * Creates a new Whiskey function from Whiskey code.
 *
 * @param name The name or NULL if the function is anonymous
 * @param node The AST node of the function
 * @param globalScope The 'external' scope where the function is defined
 */
wsky_Function *wsky_Function_newFromWsky(const char *name,
                                         const wsky_FunctionNode *node,
                                         wsky_Scope *globalScope);

/**
 * Creates a new Whiskey function from C code.
 */
wsky_Function *wsky_Function_newFromC(const wsky_MethodDef *def);

/** Calls a function as a method */
wsky_ReturnValue wsky_Function_callSelf(wsky_Function *function,
                                        wsky_Class *class,
                                        wsky_Object *self,
                                        unsigned parameterCount,
                                        const wsky_Value *parameters);

/** Calls a function */
static inline wsky_ReturnValue wsky_Function_call(wsky_Function *function,
                                                  unsigned parameterCount,
                                                  const wsky_Value *params) {
  return wsky_Function_callSelf(function, NULL, NULL,
                                parameterCount, params);
}

/** Returns true if the given value is a function */
static inline bool wsky_isFunction(wsky_Value value) {
  return wsky_getClass(value) == wsky_Function_CLASS;
}


/**
 * @}
 * @}
 */

#endif /* !FUNCTION_H_ */
