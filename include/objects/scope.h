#ifndef SCOPE_H_
# define SCOPE_H_

# include "dict.h"
# include "value.h"
# include "object.h"
# include "class_def.h"

/**
 * @addtogroup objects
 * @{
 *
 * @defgroup Scope Scope
 * @{
 */

extern const wsky_ClassDef wsky_Scope_CLASS_DEF;

/** The Scope class */
extern wsky_Class *wsky_Scope_CLASS;


/**
 * Represents the scope of a name binding.
 * https://en.wikipedia.org/wiki/Scope_(computer_science)
 */
typedef struct wsky_Scope_s {
  wsky_OBJECT_HEAD

  /** The parent scope or NULL */
  struct wsky_Scope_s *parent;

  /** A dictionnary of the variables */
  wsky_Dict variables;

  /** The current object */
  wsky_Object *self;

} wsky_Scope;


/**
 * Creates a new Scope.
 */
wsky_Scope *wsky_Scope_new(wsky_Scope *parent, wsky_Object *self);

/**
 * Creates a root Scope.
 *
 * Its parent is NULL and it contains some builtins.
 */
wsky_Scope *wsky_Scope_newRoot(void);

/**
 * Deletes a scope.
 */
void wsky_Scope_delete(wsky_Scope *scope);

/**
 * Returns `true` if the given value is an object.
 */
bool wsky_isScope(const wsky_Value value);

/**
 * Prints the given scope for debugging purposes.
 */
void wsky_Scope_print(const wsky_Scope *scope);

/**
 * Adds a new variable to the scope.
 */
void wsky_Scope_addVariable(wsky_Scope *scope,
                            const char *name, wsky_Value value);

/**
 * Looks for a variable and return its value.
 */
wsky_Value wsky_Scope_getVariable(wsky_Scope *scope, const char *name);

/**
 * Sets the value of a variable.
 * Returns true on error (if the variable is undefined)
 */
bool wsky_Scope_setVariable(wsky_Scope *scope,
                            const char *name, wsky_Value value);

/**
 * Returns true if the scope or a parent scope contains a variable of the
 * given name.
 */
bool wsky_Scope_containsVariable(const wsky_Scope *scope, const char *name);

/**
 * Returns true if the scope contains a variable of the given name.
 * Does not search in the parent scopes.
 */
bool wsky_Scope_containsVariableLocally(const wsky_Scope *scope,
                                        const char *name);

/**
 * @}
 * @}
 */

#endif /* !SCOPE_H_ */
