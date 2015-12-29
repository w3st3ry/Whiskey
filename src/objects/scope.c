#include "objects/scope.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"

typedef wsky_Scope Scope;
typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;


static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 wsky_Value *params);
static void destroy(wsky_Object *object);

static void acceptGC(wsky_Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0},
};

wsky_Class wsky_Scope_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Scope",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Scope),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};



Scope *wsky_Scope_new(Scope *parent, Object *self) {
  ReturnValue rv = wsky_Object_new(&wsky_Scope_CLASS, 0, NULL);
  if (rv.exception)
    return NULL;
  Scope *scope = (Scope *) rv.v.v.objectValue;

  scope->parent = parent;
  scope->self = self;
  wsky_Dict_init(&scope->variables);
  return scope;
}

static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 Value *params) {
  (void) object;
  (void) paramCount;
  (void) params;
  return NULL;
}

static void freeVariable(const char *name, void *valuePointer) {
  (void) name;
  wsky_FREE(valuePointer);
}

static void destroy(wsky_Object *object) {
  Scope *scope = (Scope *) object;

  wsky_Dict_apply(&scope->variables, &freeVariable);
  wsky_Dict_free(&scope->variables);
}

void wsky_Scope_delete(wsky_Scope *scope) {
  wsky_Dict_apply(&scope->variables, &freeVariable);
  wsky_Dict_free(&scope->variables);
}



static void visitVariable(const char *name, void *valuePointer) {
  (void) name;
  Value value = *(Value *) valuePointer;
  wsky_GC_VISIT_VALUE(value);
}

static void acceptGC(wsky_Object *object) {
  Scope *scope = (Scope *) object;

  wsky_Dict_apply(&scope->variables, &visitVariable);
}



static void printVariable(const char *name, void *value_) {
  Value value = *((Value *) value_);
  char *string = wsky_Value_toCString(value);
  printf("%s = %s\n", name, string);
  wsky_FREE(string);
}


void wsky_Scope_print(const Scope *scope) {
  wsky_Dict_apply((wsky_Dict *) &scope->variables, &printVariable);
  if (scope->parent) {
    printf("parent:\n");
    wsky_Scope_print(scope->parent);
  }
}


void wsky_Scope_addVariable(Scope *scope, const char *name, Value value) {
  Value *valuePointer = wsky_MALLOC(sizeof(Value));
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
  if (valuePointer)
    return *valuePointer;
  if (!scope->parent) {
    fprintf(stderr, "wsky_Scope_getVariable(): error\n");
    wsky_Scope_print(scope);
    abort();
  }
  return wsky_Scope_getVariable(scope->parent, name);
}
