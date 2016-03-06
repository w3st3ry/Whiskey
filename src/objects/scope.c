#include "objects/scope.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "objects/str.h"
#include "objects/class.h"
#include "gc.h"


typedef wsky_Scope Scope;
typedef wsky_Object Object;
typedef wsky_Class Class;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_Module Module;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_Scope_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Scope",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Scope),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

wsky_Class *wsky_Scope_CLASS;



Scope *wsky_Scope_new(Scope *parent, Class *class, Object *self) {
  ReturnValue rv = wsky_Object_new(wsky_Scope_CLASS, 0, NULL);
  if (rv.exception)
    return NULL;

  Scope *scope = (Scope *) rv.v.v.objectValue;

  if (class)
    assert(!class->native);
  scope->defClass = class;
  scope->parent = parent;
  scope->self = self;
  scope->module = NULL;
  wsky_Dict_init(&scope->variables);
  return scope;
}


static void addClass(Scope *scope, Class *class) {
  Value value = wsky_Value_fromObject((Object *)class);
  wsky_Scope_addVariable(scope, class->name, value);
}

Scope *wsky_Scope_newRoot(Module *module) {
  Scope *scope = wsky_Scope_new(NULL, NULL, NULL);

  const wsky_ClassArray *classes = wsky_getBuiltinClasses();
  for (size_t i = 0; i < classes->count; i++)
    addClass(scope, classes->classes[i]);

  assert(module);
  scope->module = module;

  return scope;
}


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  (void) object;
  (void) paramCount;
  (void) params;
  wsky_RETURN_NULL;
}

static void freeVariable(const char *name, void *valuePointer) {
  (void) name;
  wsky_free(valuePointer);
}

static ReturnValue destroy(Object *object) {
  Scope *scope = (Scope *) object;

  wsky_Dict_apply(&scope->variables, &freeVariable);
  wsky_Dict_free(&scope->variables);
  wsky_RETURN_NULL;
}

void wsky_Scope_delete(wsky_Scope *scope) {
  wsky_Dict_apply(&scope->variables, &freeVariable);
  wsky_Dict_free(&scope->variables);
}



static void visitVariable(const char *name, void *valuePointer) {
  (void) name;
  Value value = *(Value *) valuePointer;
  wsky_GC_visitValue(value);
}

static void acceptGC(wsky_Object *object) {
  // Do not visit the parent scope here, since the whole scope
  // stack is visited in eval.c
  Scope *scope = (Scope *) object;
  wsky_Dict_apply(&scope->variables, &visitVariable);
  if (scope->module)
    wsky_GC_visitObject(scope->module);
}



static void printVariable(const char *name, void *value_) {
  Value value = *((Value *) value_);
  ReturnValue rv = wsky_toString(value);
  if (rv.exception)
    abort();
  wsky_String *string = (wsky_String *) rv.v.v.objectValue;
  printf("%s = %s\n", name, string->string);
}


void wsky_Scope_print(const Scope *scope) {
  wsky_Dict_applyConst(&scope->variables, &printVariable);
  if (scope->parent) {
    printf("parent:\n");
    wsky_Scope_print(scope->parent);
  }
}


void wsky_Scope_addVariable(Scope *scope, const char *name, Value value) {
  Value *valuePointer = wsky_safeMalloc(sizeof(Value));
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


bool wsky_Scope_containsVariableLocally(const Scope *scope,
                                        const char *name) {
  return wsky_Dict_contains(&scope->variables, name);
}


Scope *wsky_Scope_getRoot(Scope *scope) {
  if (!scope->parent)
    return scope;
  return wsky_Scope_getRoot(scope->parent);
}

wsky_Module *wsky_Scope_getModule(Scope *scope) {
  Scope *root = wsky_Scope_getRoot(scope);
  assert(root->module);
  return root->module;
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
