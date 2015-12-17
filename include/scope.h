#ifndef SCOPE_H_
# define SCOPE_H_

#include "dict.h"
#include "value.h"
#include "object.h"


extern wsky_Class wsky_Scope_CLASS;

void wsky_Scope_initClass(void);
void wsky_Scope_freeClass(void);



typedef struct wsky_Scope_s {
  wsky_OBJECT_HEAD

  struct wsky_Scope_s *parent;

  wsky_Dict variables;

  /* The current object */
  wsky_Object *this;

} wsky_Scope;



wsky_Scope *wsky_Scope_new(wsky_Scope *parent, wsky_Object *this);

bool wsky_isScope(const wsky_Value value);

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


#endif /* !SCOPE_H_ */
