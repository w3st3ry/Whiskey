#include <assert.h>
#include <string.h>
#include "../return_value_private.h"


typedef wsky_Value Value;
typedef wsky_Class Class;
typedef wsky_Object Object;
typedef wsky_Function Function;
typedef wsky_FunctionNode FunctionNode;
typedef wsky_Scope Scope;
typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;



static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);

static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};


const wsky_ClassDef wsky_Function_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Function",
  .final = true,
  .constructor = NULL,
  .destructor = &destroy,
  .objectSize = sizeof(wsky_Function),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

wsky_Class *wsky_Function_CLASS;



Function *wsky_Function_newFromWsky(const char *name,
                                    const FunctionNode *node,
                                    wsky_Scope *globalScope) {
  ReturnValue r = wsky_Object_new(wsky_Function_CLASS, 0, NULL);
  if (r.exception)
    abort();
  Function *function = (wsky_Function *) r.v.v.objectValue;
  function->name = name ? wsky_strdup(name) : NULL;
  assert(node);
  function->node = (FunctionNode *)wsky_ASTNode_copy((const Node *)node);
  function->globalScope = globalScope;
  return function;
}

wsky_Function *wsky_Function_newFromC(const char *name,
                                      const wsky_MethodDef *def) {
  ReturnValue r = wsky_Object_new(wsky_Function_CLASS, 0, NULL);
  if (r.exception)
    abort();
  Function *function = (wsky_Function *) r.v.v.objectValue;
  function->name = name ? wsky_strdup(name) : NULL;
  function->node = NULL;
  assert(def);
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
  wsky_IdentifierNode *identifier = (wsky_IdentifierNode *) node;
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

  ReturnValue rv = wsky_ReturnValue_NULL;
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
