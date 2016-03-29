#include <assert.h>
#include <string.h>
#include "../whiskey_private.h"


static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);

static MethodDef methods[] = {
  {0, 0, 0, 0},
};


const ClassDef wsky_Function_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Function",
  .final = true,
  .constructor = NULL,
  .privateConstructor = true,
  .destructor = &destroy,
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

Class *wsky_Function_CLASS;



Function *wsky_Function_newFromWsky(const char *name,
                                    const FunctionNode *node,
                                    Scope *globalScope) {
  ReturnValue r = wsky_Object_new(wsky_Function_CLASS, 0, NULL);
  if (r.exception)
    abort();
  Function *function = (Function *) r.v.v.objectValue;
  function->name = name ? wsky_strdup(name) : NULL;
  assert(node);
  function->node = (FunctionNode *)wsky_ASTNode_copy((const Node *)node);
  function->globalScope = globalScope;
  return function;
}

Function *wsky_Function_newFromC(const MethodDef *def) {
  ReturnValue r = wsky_Object_new(wsky_Function_CLASS, 0, NULL);
  if (r.exception)
    abort();
  Function *function = (Function *) r.v.v.objectValue;
  function->name = wsky_strdup(def->name);
  function->node = NULL;
  function->cMethod = *def;
  function->globalScope = NULL;
  return function;
}



static ReturnValue destroy(Object *object) {
  Function *self = (Function *) object;
  if (self->name)
    wsky_free(self->name);
  if (self->node)
    wsky_ASTNode_delete((Node *)self->node);
  RETURN_NULL;
}


static void acceptGC(Object *object) {
  Function *self = (Function *) object;
  wsky_GC_visitObject(self->globalScope);
}



static void addVariable(Scope *scope, Node *node, const Value *value) {
  IdentifierNode *identifier = (IdentifierNode *) node;
  wsky_Scope_addVariable(scope, identifier->name, *value);
}

static void addVariables(Scope *scope,
                         NodeList *parameterNodes,
                         const Value *values) {
  NodeList *param = parameterNodes;
  while (param) {
    addVariable(scope, param->node, values);
    param = param->next;
    values++;
  }
}

static ReturnValue callNativeFunction(Function *function,
                                      Class *class,
                                      Object *self,
                                      unsigned parameterCount,
                                      const Value *parameters) {
  (void)class;
  return wsky_MethodDef_call(&function->cMethod,
                             self,
                             parameterCount,
                             parameters);
}

ReturnValue wsky_Function_callSelf(Function *function,
                                   Class *class,
                                   Object *self,
                                   unsigned parameterCount,
                                   const Value *parameters) {
  if (self)
    assert(class);

  if (!function->node)
    return callNativeFunction(function, class, self,
                              parameterCount, parameters);

  NodeList *params = function->node->parameters;
  unsigned wantedParamCount = wsky_ASTNodeList_getCount(params);
  if (wantedParamCount != parameterCount)
    RAISE_NEW_PARAMETER_ERROR("Invalid parameter count");

  Scope *innerScope = wsky_Scope_new(function->globalScope, class, self);
  wsky_eval_pushScope(innerScope);
  addVariables(innerScope, params, parameters);

  ReturnValue rv = ReturnValue_NULL;
  NodeList *child = function->node->children;
  while (child) {
    rv = wsky_evalNode(child->node, innerScope);
    if (rv.exception)
      break;
    child = child->next;
  }

  wsky_eval_popScope();
  return rv;
}
