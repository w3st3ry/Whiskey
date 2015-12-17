#include "eval.h"

#include <stdlib.h>

typedef wsky_Scope Scope;
typedef wsky_Object Object;
typedef wsky_Value Value;

Scope *wsky_Scope_new(Scope *parent, Object *this) {
  Scope *scope = malloc(sizeof(Scope));
  scope->parent = parent;
  scope->this = this;
  wsky_Dict_init(&scope->variables);
  return scope;
}

static void freeVariable(const char *name, void *value_) {
  (void) name;
  free(value_);
}

void wsky_Scope_delete(Scope *scope) {
  wsky_Dict_apply((wsky_Dict *) &scope->variables, &freeVariable);
  wsky_Dict_free(&scope->variables);
  free(scope);
}

static void printVariable(const char *name, void *value_) {
  Value value = *((Value *) value_);
  char *string = wsky_Value_toCString(value);
  printf("%s = %s\n", name, string);
  free(string);
}

void wsky_Scope_print(const Scope *scope) {
  if (scope->parent) {
    wsky_Scope_print(scope->parent);
  }
  wsky_Dict_apply((wsky_Dict *) &scope->variables, &printVariable);
}

void wsky_Scope_addVariable(Scope *scope,
                            const char *name, Value value) {
  Value *valuePointer = malloc(sizeof(Value));
  *valuePointer = value;
  wsky_Dict_set(&scope->variables, name, valuePointer);
}

bool wsky_Scope_setVariable(Scope *scope,
                            const char *name, Value value) {
  Value *valuePointer = (Value*) wsky_Dict_get(&scope->variables, name);
  if (valuePointer) {
    *valuePointer = value;
    return false;
  }
  if (!scope->parent)
    return true;
  return (wsky_Scope_setVariable(scope->parent, name, value));
}

bool wsky_Scope_containsVariable(const Scope *scope, const char *name) {
  if (wsky_Dict_contains(&scope->variables, name)) {
    return true;
  }
  if (!scope->parent)
    return false;
  return (wsky_Scope_containsVariable(scope->parent, name));
}

bool wsky_Scope_containsVariableLocally(const wsky_Scope *scope,
                                        const char *name) {
  return wsky_Dict_contains(&scope->variables, name);
}

Value wsky_Scope_getVariable(Scope *scope, const char *name) {
  Value *valuePointer = (Value*) wsky_Dict_get(&scope->variables, name);
  if (!valuePointer)
    abort();
  return *valuePointer;
}
