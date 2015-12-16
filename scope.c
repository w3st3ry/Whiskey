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
  if (wsky_Dict_contains(&scope->variables, name)) {
    wsky_Scope_addVariable(scope, name, value);
    return false;
  }
  if (!scope->parent)
    return true;
  return (wsky_Scope_setVariable(scope->parent, name, value));
}
