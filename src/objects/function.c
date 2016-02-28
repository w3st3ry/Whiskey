#include "objects/function.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "eval.h"
#include "gc.h"
#include "objects/parameter_error.h"
#include "objects/str.h"
#include "return_value.h"
#include "string_utils.h"



typedef wsky_Value Value;
typedef wsky_Class Class;
typedef wsky_Object Object;
typedef wsky_Function Function;
typedef wsky_FunctionNode FunctionNode;
typedef wsky_Scope Scope;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;



static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);

static ReturnValue destroy(Object *object);

static void acceptGC(Object *object);

static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};


const wsky_ClassDef wsky_Function_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Function",
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(wsky_Function),
  .methodDefs = methods,
  .gcAcceptFunction = acceptGC,
};

wsky_Class *wsky_Function_CLASS;



Function *wsky_Function_new(const char *name,
                            const FunctionNode *node,
                            wsky_Scope *globalScope) {
  ReturnValue r = wsky_Object_new(wsky_Function_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  Function *function = (wsky_Function *) r.v.v.objectValue;
  function->name = name ? wsky_strdup(name) : NULL;
  function->node = (FunctionNode *)wsky_ASTNode_copy((const Node *)node);
  function->globalScope = globalScope;
  return function;
}



static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  (void) paramCount;
  (void) params;
  Function *self = (Function *) object;
  self->name = NULL;
  self->node = NULL;
  wsky_RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  Function *self = (Function *) object;
  if (self->name)
    wsky_free(self->name);
  wsky_ASTNode_delete((Node *)self->node);
  wsky_RETURN_NULL;
}


static void acceptGC(Object *object) {
  Function *self = (Function *) object;
  wsky_GC_VISIT(self->globalScope);
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

ReturnValue wsky_Function_call(Function *function,
                               Class *class,
                               Object *self,
                               unsigned parameterCount,
                               const Value *parameters) {
  assert(function->node);
  NodeList *params = function->node->parameters;
  unsigned wantedParamCount = wsky_ASTNodeList_getCount(params);
  if (wantedParamCount != parameterCount)
    wsky_RETURN_NEW_PARAMETER_ERROR("Invalid parameter count");

  Scope *scope = wsky_Scope_new(function->globalScope, class, self);
  addVariables(scope, params, parameters);

  ReturnValue rv = wsky_ReturnValue_NULL;
  NodeList *child = function->node->children;
  while (child) {
    rv = wsky_evalNode(child->node, scope);
    if (rv.exception)
      break;
    child = child->next;
  }
  return rv;
}
