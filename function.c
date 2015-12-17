#include "function.h"

#include <stdlib.h>
#include <string.h>



typedef wsky_Function Function;
typedef wsky_FunctionNode FunctionNode;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;



static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 wsky_Value *params);
static void destroy(wsky_Object *object);
static ReturnValue toString(wsky_Object *object);



wsky_Class wsky_Function_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Function",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(wsky_Function)
};


void wsky_Function_initClass(void) {
  wsky_MethodList *ml = (wsky_MethodList *) &wsky_Function_CLASS.methods;
  wsky_MethodList_init(ml, 10);

#define ADD(name, paramCount)                   \
  wsky_MethodList_addNew(ml, #name, paramCount, \
                         (void *) &name)

  ADD(toString, 0);

#undef ADD
}

void wsky_Function_freeClass(void) {
  wsky_MethodList *ml = (wsky_MethodList *) &wsky_Function_CLASS.methods;
  wsky_MethodList_free(ml);
}



Function *wsky_Function_new(const char *name,
                            const FunctionNode *node,
                            wsky_Scope *globalScope) {
  wsky_ReturnValue r = wsky_Object_new(&wsky_Function_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  wsky_Function *function = (wsky_Function *) r.v.v.objectValue;
  function->name = strdup(name);
  function->node = node;
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
}



static ReturnValue toString(wsky_Object *object) {
  wsky_RETURN_VALUE(wsky_buildValue("s", "<Function>"));
}
