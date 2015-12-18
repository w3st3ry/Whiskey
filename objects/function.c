#include "function.h"

#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "ast.h"
#include "gc.h"
#include "return_value.h"



typedef wsky_Function Function;
typedef wsky_FunctionNode FunctionNode;
typedef wsky_Value Value;
typedef wsky_Scope Scope;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;



static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 wsky_Value *params);
static void destroy(wsky_Object *object);
static ReturnValue toString(wsky_Object *object);



#define M(name, paramCount)                     \
  {#name, paramCount, (void *) &name}

static wsky_MethodDef methods[] = {
  M(toString, 0),
  {0, 0, 0},
};

#undef M

wsky_Class wsky_Function_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Function",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(wsky_Function),
  .methodDefs = methods,
};



Function *wsky_Function_new(const char *name,
                            const FunctionNode *node,
                            wsky_Scope *globalScope) {
  wsky_ReturnValue r = wsky_Object_new(&wsky_Function_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  wsky_Function *function = (wsky_Function *) r.v.v.objectValue;
  function->name = strdup(name);
  function->node = node;
  wsky_INCREF(globalScope);
  function->globalScope = globalScope;
  return function;
}

bool wsky_isFunction(const wsky_Value value) {
  if (value.type != wsky_Type_OBJECT)
    return NULL;
  return value.v.objectValue->class == &wsky_Function_CLASS;
}

static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 wsky_Value *params) {
  (void) paramCount;
  (void) params;
  Function *this = (Function *) object;
  this->name = NULL;
  this->node = NULL;
  return NULL;
}

static void destroy(wsky_Object *object) {
  Function *this = (Function *) object;
  if (this->name)
    free(this->name);
  wsky_DECREF(this->globalScope);
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

ReturnValue wsky_Function_call(wsky_Object *object,
                               unsigned parameterCount,
                               Value *parameters) {
  Function *this = (Function *) object;
  NodeList *params = this->node->parameters;
  unsigned wantedParamCount = wsky_ASTNodeList_getCount(params);
  if (wantedParamCount != parameterCount) {
    wsky_RETURN_NEW_EXCEPTION("Invalid parameter count");
  }
  Scope *scope = wsky_Scope_new(this->globalScope, NULL);
  addVariables(scope, params, parameters);

  ReturnValue rv = wsky_ReturnValue_NULL;
  NodeList *child = this->node->children;
  while (child) {
    rv = wsky_evalNode(child->node, scope);
    if (rv.exception)
      break;
    child = child->next;
  }
  wsky_DECREF(scope);
  return rv;
}



static ReturnValue toString(wsky_Object *object) {
  Function *this = (Function *) object;
  (void) this;
  wsky_RETURN_VALUE(wsky_buildValue("s", "<Function>"));
}
