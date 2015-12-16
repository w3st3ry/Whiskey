#ifndef EVAL_H_
# define EVAL_H_

#include "ast.h"
#include "object.h"
#include "dict.h"

typedef struct wsky_Scope_s {

  struct wsky_Scope_s *parent;

  wsky_Dict variables;

  /* The current object */
  wsky_Object *this;

} wsky_Scope;

wsky_Scope *wsky_Scope_new(wsky_Scope *parent, wsky_Object *this);
void wsky_Scope_delete(wsky_Scope *scope);
void wsky_Scope_print(const wsky_Scope *scope);

void wsky_Scope_addVariable(wsky_Scope *scope,
                            const char *name, wsky_Value value);

wsky_Value wsky_Scope_getVariable(wsky_Scope *scope, const char *name);

/* Returns true on error (if the variable is undefined) */
bool wsky_Scope_setVariable(wsky_Scope *scope,
                            const char *name, wsky_Value value);
bool wsky_Scope_containsVariable(const wsky_Scope *scope, const char *name);
bool wsky_Scope_containsVariableLocally(const wsky_Scope *scope,
                                        const char *name);



wsky_ReturnValue wsky_evalNode(const wsky_ASTNode *node, wsky_Scope *scope);
wsky_ReturnValue wsky_evalString(const char *source);

#endif /* !EVAL_H_ */
