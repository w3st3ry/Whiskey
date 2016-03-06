#ifndef SCOPE_H_
# define SCOPE_H_

# include "dict.h"
# include "value.h"
# include "object.h"
# include "class_def.h"
# include "module.h"

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

  /**
   * The current class or NULL.
   * Don't mix up it with `class`. The class of a scope object is
   * always wsky_Scope_CLASS.
   */
  wsky_Class *defClass;

  /** The current object or NULL */
  wsky_Object *self;

  /**
   * The module if no parent scope, NULL otherwise.
   */
  wsky_Module *module;

} wsky_Scope;


/**
 * Creates a new Scope.
 */
wsky_Scope *wsky_Scope_new(wsky_Scope *parent, wsky_Class *class,
                           wsky_Object *self);

/**
 * Creates a new root scope.
 *
 * Its parent is NULL and it contains some builtins.
 *
 * @param module The module.
 */
wsky_Scope *wsky_Scope_newRoot(wsky_Module *module);

/**
 * Deletes a scope.
 */
void wsky_Scope_delete(wsky_Scope *scope);

/**
 * Returns `true` if the given value is a Scope.
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
 * Calls abort() if the variable is not found.
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
 * Returns the root scope.
 */
wsky_Scope *wsky_Scope_getRoot(wsky_Scope *scope);

/**
 * Returns the module of the file.
 */
wsky_Module *wsky_Scope_getModule(wsky_Scope *scope);

/**
 * @}
 * @}
 */

#endif /* !SCOPE_H_ */
