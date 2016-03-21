#include <ctype.h>
#include <assert.h>
#include <string.h>
#include "whiskey_private.h"


typedef struct {
  Scope **scopes;
  size_t capacity;
  size_t length;
} ScopeStack;

static ScopeStack scopeStack = {
  .scopes = NULL,
  .capacity = 0,
  .length = 0,
};

static void growScopeStack(void) {
  assert(scopeStack.length == scopeStack.capacity);
  scopeStack.capacity *= 2;
  if (scopeStack.capacity == 0)
    scopeStack.capacity = 1;
  scopeStack.scopes = wsky_realloc(scopeStack.scopes,
                                   scopeStack.capacity * sizeof(Scope *));
  if (!scopeStack.scopes)
    abort();
}

static void freeScopeStack(void) {
  assert(scopeStack.length == 0);
  wsky_free(scopeStack.scopes);
  scopeStack.scopes = NULL;
  scopeStack.capacity = 0;
}

void wsky_eval_pushScope(Scope *scope) {
  if (scopeStack.capacity == scopeStack.length)
    growScopeStack();

  scopeStack.scopes[scopeStack.length++] = scope;
}

Scope *wsky_eval_popScope(void) {
  if (scopeStack.length == 0)
    abort();
  scopeStack.length--;
  Scope *scope = scopeStack.scopes[scopeStack.length];
  if (scopeStack.length == 0)
    freeScopeStack();
  return scope;
}

void wsky_eval_visitScopeStack(void) {
  for (size_t i = 0; i < scopeStack.length; i++)
    wsky_GC_visitObject(scopeStack.scopes[i]);
}



#define TO_LITERAL_NODE(n) ((const LiteralNode *) (n))

#define isBool(value) wsky_isBoolean(value)
#define isInt(value) wsky_isInteger(value)
#define isFloat(value) wsky_isFloat(value)


static ReturnValue createUnsupportedBinOpError(const char *leftClass,
                                               const char *operator,
                                               Value right) {
  char message[128];
  snprintf(message, 127,
           "Unsupported classes for %s: %s and %s",
           operator,
           leftClass,
           wsky_getClassName(right));

  RAISE_NEW_TYPE_ERROR(message);
}

static ReturnValue createUnsupportedUnaryOpError(const char *operator,
                                                 const char *rightClass) {
  char message[128];
  snprintf(message, 127,
           "Unsupported class for unary %s: %s",
           operator,
           rightClass);

  RAISE_NEW_TYPE_ERROR(message);
}


/*
 * Returns a new `NotImplementedException`
 */
#define RETURN_NOT_IMPL(operator)               \
  RAISE_NEW_NOT_IMPLEMENTED_ERROR(operator)


/*
 * Returns true if the given object is not null and is a
 * `NotImplementedException`
 */
#define IS_NOT_IMPLEMENTED_ERROR(e)                     \
  ((e) && (e)->class == wsky_NotImplementedError_CLASS)


#include "eval_int.c"
#include "eval_float.c"
#include "eval_bool.c"


static const char *getBinOperatorMethodName(Operator operator,
                                            bool right) {
  static char buffer[64];
  snprintf(buffer, 63, "operator %s%s",
           right ? "r" : "",
           wsky_Operator_toString(operator));
  return buffer;
}

static ReturnValue evalBinOperatorValues(Value left,
                                         Operator operator,
                                         Value right,
                                         bool reverse) {
  switch (left.type) {
  case Type_BOOL:
    return evalBinOperatorBool(left.v.boolValue, operator, right);

  case Type_INT:
    return evalBinOperatorInt(left.v.intValue, operator, right);

  case Type_FLOAT:
    return evalBinOperatorFloat(left.v.floatValue, operator, right);

  case Type_OBJECT: {
    const char *method = getBinOperatorMethodName(operator, reverse);
    Object *object = left.v.objectValue;
    return wsky_Object_callMethod1(object, method, right);
  }
  }
  abort();
}


static ReturnValue evalUnaryOperatorValues(Operator operator,
                                           Value right) {

  switch (right.type) {
  case Type_BOOL:
    return evalUnaryOperatorBool(operator, right.v.boolValue);

  case Type_INT:
    return evalUnaryOperatorInt(operator, right.v.intValue);

  case Type_FLOAT:
    return evalUnaryOperatorFloat(operator, right.v.floatValue);

  default:
    return createUnsupportedUnaryOpError(wsky_Operator_toString(operator),
                                         wsky_getClassName(right));
  }
}


ReturnValue wsky_doBinaryOperation(Value left,
                                   Operator operator,
                                   Value right) {
  ReturnValue rv = evalBinOperatorValues(left, operator, right, false);
  if (!IS_NOT_IMPLEMENTED_ERROR(rv.exception))
    return rv;

  ReturnValue rev;
  rev = evalBinOperatorValues(right, operator, left, true);
  if (!IS_NOT_IMPLEMENTED_ERROR(rev.exception)) {
    if (rev.exception)
      printf("evalBinOperator() %s %s %s\n", rev.exception->class->name,
             wsky_getClassName(left),
             wsky_getClassName(right));
    return rev;
  }

  rev = evalBinOperatorValues(right, operator, left, false);
  if (!IS_NOT_IMPLEMENTED_ERROR(rev.exception))
    return rev;

  return createUnsupportedBinOpError(wsky_getClassName(left),
                                     wsky_Operator_toString(operator),
                                     right);
}


static ReturnValue evalBinOperator(const Node *leftNode,
                                   Operator operator,
                                   const Node *rightNode,
                                   Scope *scope) {
  ReturnValue leftRV = wsky_evalNode(leftNode, scope);
  if (leftRV.exception)
    return leftRV;

  ReturnValue rightRV = wsky_evalNode(rightNode, scope);
  if (rightRV.exception)
    return rightRV;

  return wsky_doBinaryOperation(leftRV.v, operator, rightRV.v);
}


static ReturnValue evalUnaryOperator(Operator operator,
                                     const Node *rightNode,
                                     Scope *scope) {
  ReturnValue rightRV = wsky_evalNode(rightNode, scope);
  if (rightRV.exception) {
    return rightRV;
  }
  return evalUnaryOperatorValues(operator, rightRV.v);
}

static ReturnValue evalOperator(const OperatorNode *n, Scope *scope) {
  Operator op = n->operator;
  Node *leftNode = n->left;
  Node *rightNode = n->right;
  if (leftNode) {
    return evalBinOperator(leftNode, op, rightNode, scope);
  }
  return evalUnaryOperator(op, rightNode, scope);
}


ReturnValue wsky_evalSequence(const SequenceNode *node,
                              Scope *innerScope) {
  NodeList *child = node->children;
  ReturnValue rv = ReturnValue_NULL;
  while (child) {
    rv = wsky_evalNode(child->node, innerScope);
    if (rv.exception)
      break;
    child = child->next;
  }
  return rv;
}

static ReturnValue evalSequence(const SequenceNode *node,
                                Scope *parentScope) {
  Scope *innerScope = wsky_Scope_new(parentScope,
                                     parentScope->defClass,
                                     parentScope->self);
  wsky_eval_pushScope(innerScope);
  ReturnValue rv = wsky_evalSequence(node, innerScope);
  wsky_eval_popScope();
  return rv;
}

static ReturnValue createAlreadyDeclaredNameError(const char *name) {
  char *message = wsky_asprintf("Identifier '%s' already declared", name);
  Exception *e = (Exception *)wsky_NameError_new(message);
  free(message);
  RAISE_EXCEPTION(e);
}

static ReturnValue declareVariable(const char *name, Value value,
                                   Scope *scope) {
  if (wsky_Scope_containsVariableLocally(scope, name))
    return createAlreadyDeclaredNameError(name);

  wsky_Scope_addVariable(scope, name, value);
  RETURN_VALUE(value);
}

static ReturnValue evalVar(const VarNode *n, Scope *scope) {
  Value value = Value_NULL;
  if (n->right) {
    ReturnValue rv = wsky_evalNode(n->right, scope);
    if (rv.exception)
      return rv;
    value = rv.v;
  }
  return declareVariable(n->name, value, scope);
}


static ReturnValue raiseUndeclaredNameError(const char *name) {
  char *message = wsky_asprintf("Use of undeclared identifier '%s'", name);
  Exception *e = (Exception *)wsky_NameError_new(message);
  free(message);
  RAISE_EXCEPTION(e);
}


static ReturnValue evalIdentifier(const IdentifierNode *n, Scope *scope) {
  const char *name = n->name;
  if (!wsky_Scope_containsVariable(scope, name))
    return raiseUndeclaredNameError(name);

  RETURN_VALUE(wsky_Scope_getVariable(scope, name));
}

static ReturnValue evalSelf(Scope *scope) {
  if (!scope->self)
    RAISE_NEW_EXCEPTION("'@' used outside of a class");
  RETURN_OBJECT(scope->self);
}

static ReturnValue evalSuper(Scope *scope) {
  (void)scope;
  abort();
  RAISE_NEW_EXCEPTION("'super' used outside of a member access");
}

static ReturnValue evalSuperclass(Scope *scope) {
  (void)scope;
    if (!scope->self)
      RAISE_NEW_EXCEPTION("'superclass' used outside of a class");
    RETURN_OBJECT((Object *)scope->self->class->super);
}


static ReturnValue assignToVariable(Value right,
                                    const char *name,
                                    Scope *scope) {

  if (!wsky_Scope_containsVariable(scope, name))
    return raiseUndeclaredNameError(name);

  wsky_Scope_setVariable(scope, name, right);
  RETURN_VALUE(right);
}

static Exception *createImmutableObjectError(Value value) {
  const char *className = wsky_getClassName(value);
  char *message = wsky_asprintf("'%s' objects are immutables", className);
  TypeError *e = wsky_TypeError_new(message);
  free(message);
  return (Exception *)e;
}

/** Returns true if the given object is mutable */
static bool isMutableObject(Object *object) {
  if (!object)
    return false;
  if (object->class == wsky_Structure_CLASS)
    return true;
  return !object->class->native;
}

static ReturnValue assignToObject(Object *object,
                                  const char *attribute,
                                  Value right,
                                  Scope *scope) {
  if (!isMutableObject(object)) {
    Exception *e = createImmutableObjectError(Value_fromObject(object));
    RAISE_EXCEPTION(e);
  }

  if (object->class == wsky_Structure_CLASS)
    return wsky_Structure_set((Structure *)object, attribute, right);

  bool privateAccess = object == scope->self;
  if (object && privateAccess)
    return wsky_Class_setPrivate(scope->defClass, object,
                                 attribute, right);
  else
    return wsky_Class_set(wsky_Object_getClass(object), object,
                          attribute, right);
}

static ReturnValue assignToMember(Node *leftNode,
                                  const char *attribute,
                                  Value right,
                                  Scope *scope) {
  if (leftNode->type == wsky_ASTNodeType_SUPER) {
    if (!scope->defClass)
      RAISE_NEW_EXCEPTION("'super' used outside of a class");
    if (!scope->defClass->super)
      RAISE_NEW_EXCEPTION("No superclass");
    Object *object = scope->self;
    return wsky_Class_set(scope->defClass->super, object,
                          attribute, right);
  }

  ReturnValue rv = wsky_evalNode(leftNode, scope);
  if (rv.exception)
    return rv;

  if (rv.v.type != Type_OBJECT)
    RAISE_EXCEPTION(createImmutableObjectError(rv.v));

  Object *object = rv.v.v.objectValue;

  return assignToObject(object, attribute, right, scope);
}

static ReturnValue evalAssignment(const AssignmentNode *n,
                                  Scope *scope) {
  Node *leftNode = n->left;

  ReturnValue right = wsky_evalNode(n->right, scope);

  if (right.exception)
    return right;

  if (leftNode->type == wsky_ASTNodeType_IDENTIFIER) {
    IdentifierNode *id = (IdentifierNode *) leftNode;
    return assignToVariable(right.v, id->name, scope);
  }
  if (leftNode->type == wsky_ASTNodeType_MEMBER_ACCESS) {
    MemberAccessNode *member = (MemberAccessNode *) leftNode;
    return assignToMember(member->left, member->name, right.v, scope);
  }

  RAISE_NEW_EXCEPTION("Not assignable expression");
}


static ReturnValue evalFunction(const FunctionNode *n, Scope *scope) {
  Function *function = wsky_Function_newFromWsky(n->name, n, scope);
  RETURN_OBJECT((Object *) function);
}


static ReturnValue evalParameters(Value *values,
                                  unsigned valueCount,
                                  const NodeList *nodes,
                                  Scope *scope) {
  unsigned paramCount = wsky_ASTNodeList_getCount(nodes);
  if (paramCount > valueCount)
    RAISE_NEW_EXCEPTION("Too many parameters");

  for (unsigned i = 0; i < paramCount; i++) {
    ReturnValue rv = wsky_evalNode(nodes->node, scope);
    if (rv.exception)
      return rv;
    values[i] = rv.v;
    nodes = nodes->next;
  }

  RETURN_NULL;
}


static ReturnValue callMethod(Object *instanceMethod_,
                              unsigned parameterCount,
                              Value *parameters) {
  InstanceMethod *instanceMethod;
  instanceMethod = (InstanceMethod *) instanceMethod_;
  Method *method = instanceMethod->method;

  Value self = instanceMethod->self;

  if (self.type == Type_OBJECT && self.v.objectValue) {
    return wsky_Method_call(method,
                            self.v.objectValue,
                            parameterCount,
                            parameters);
  }

  return wsky_Method_callValue(method,
                               self,
                               parameterCount,
                               parameters);
}

static inline ReturnValue callClass(Class *class,
                                    unsigned parameterCount,
                                    Value *parameters) {
  return wsky_Class_construct(class, parameterCount, parameters);
}

static Exception *createNotCallableError(Value value) {
  const char *className = wsky_getClassName(value);

  char *message = wsky_asprintf("A %s is not callable", className);
  Exception *e = (Exception *)wsky_NameError_new(message);
  free(message);
  return e;
}

static ReturnValue evalSuperCall(const CallNode *callNode, Scope *scope) {
    Class *class = scope->defClass;
    if (!class)
      RAISE_NEW_EXCEPTION("'super' used outside of a class");
    if (!class->super)
      RAISE_NEW_EXCEPTION("No superclass");

    Value parameters[32];

    ReturnValue rv = evalParameters(parameters, 32,
                                    callNode->children, scope);
    if (rv.exception)
      return rv;

    unsigned paramCount = wsky_ASTNodeList_getCount(callNode->children);

    Object *self = scope->self;
    rv = wsky_Method_call(class->super->constructor, self,
                                      paramCount, parameters);
    if (rv.exception)
      return rv;
    RETURN_OBJECT(self);
}

static ReturnValue evalCall(const CallNode *callNode, Scope *scope) {
  if (callNode->left->type == wsky_ASTNodeType_SUPER)
    return evalSuperCall(callNode, scope);

  ReturnValue rv = wsky_evalNode(callNode->left, scope);
  if (rv.exception)
    return rv;

  Value parameters[32];

  ReturnValue prv = evalParameters(parameters, 32,
                                   callNode->children, scope);
  if (prv.exception)
    return prv;

  unsigned paramCount = wsky_ASTNodeList_getCount(callNode->children);

  if (rv.v.type != Type_OBJECT) {
    char s[64];
    snprintf(s, 64, "'%s' objects are not callable",
             wsky_getClassName(rv.v));
    RAISE_NEW_TYPE_ERROR(s);
  }

  if (wsky_isFunction(rv.v)) {
    Object *function = rv.v.v.objectValue;
    rv = wsky_Function_call((Function *) function,
                            paramCount, parameters);

  } else if (wsky_isInstanceMethod(rv.v)) {
    Object *instMethod = rv.v.v.objectValue;
    rv = callMethod(instMethod, paramCount, parameters);

  } else if (wsky_isClass(rv.v)) {
    Class *class = (Class *)rv.v.v.objectValue;
    rv = callClass(class, paramCount, parameters);

  } else {
    rv = ReturnValue_fromException(createNotCallableError(rv.v));
  }

  return rv;
}

static ReturnValue getFallbackMember(Class *class, Value self,
                                     const char *attribute) {
  if (class == wsky_Module_CLASS) {
    assert(self.type == Type_OBJECT);

    Module *module = (Module *)self.v.objectValue;
    Value *member = wsky_Dict_get(&module->members, attribute);
    if (member)
      RETURN_VALUE(*member);
  } else if (class == wsky_Structure_CLASS) {
    return wsky_Structure_get((Structure *)self.v.objectValue,
                              attribute);
  }

  return wsky_AttributeError_raiseNoAttr(class->name, attribute);
}

static ReturnValue getMemberOfNativeClass(Value self,
                                          const char *attribute) {
  Class *class = wsky_getClass(self);

  Method *method = wsky_Class_findMethodOrGetter(class, attribute);
  if (!method)
    return getFallbackMember(class, self, attribute);

  if (method->flags & wsky_MethodFlags_GET) {
    if (method->flags & wsky_MethodFlags_VALUE)
      return wsky_Method_callValue0(method, self);
    else
      return wsky_Method_call0(method, self.v.objectValue);
  }

  InstanceMethod *im = wsky_InstanceMethod_new(method, self);
  RETURN_OBJECT((Object *)im);
}

static ReturnValue getAttribute(Object *object, const char *attribute,
                                Scope *scope) {
  bool privateAccess = object == scope->self;
  if (object && privateAccess)
    return wsky_Class_getPrivate(scope->defClass, object, attribute);
  else
    return wsky_Class_get(wsky_Object_getClass(object), object,
                          attribute);
}

static ReturnValue evalMemberAccess(const MemberAccessNode *dotNode,
                                    Scope *scope) {
  if (dotNode->left->type == wsky_ASTNodeType_SUPER) {
    if (!scope->defClass)
      RAISE_NEW_EXCEPTION("'super' used outside of a class");
    if (!scope->defClass->super)
      RAISE_NEW_EXCEPTION("No superclass");
    Object *object = scope->self;
    return wsky_Class_get(scope->defClass->super, object, dotNode->name);
  }

  ReturnValue rv = wsky_evalNode(dotNode->left, scope);
  if (rv.exception)
    return rv;

  if (rv.v.type != Type_OBJECT)
    return getMemberOfNativeClass(rv.v, dotNode->name);

  Object *object = rv.v.v.objectValue;

  if (wsky_Object_getClass(object)->native)
    return getMemberOfNativeClass(rv.v, dotNode->name);

  return getAttribute(object, dotNode->name, scope);
}


static ReturnValue evalClassMember(Class *class,
                                   const ClassMemberNode *memberNode,
                                   Scope *scope) {
  Function *right = NULL;

  if (memberNode->right) {
    ReturnValue rv = wsky_evalNode(memberNode->right, scope);
    if (rv.exception)
      return rv;
    assert(wsky_isFunction(rv.v));
    right = (Function *)rv.v.v.objectValue;
  } else {
    assert((memberNode->flags & wsky_MethodFlags_SET) ||
           (memberNode->flags & wsky_MethodFlags_GET));

    Method *method = wsky_Method_newFromWskyDefault(memberNode->name,
                                                    memberNode->flags,
                                                    class);
    RETURN_OBJECT((Object *)method);
  }

  Method *method = wsky_Method_newFromWsky(right, memberNode->flags,
                                           class);
  RETURN_OBJECT((Object *)method);
}


static void addMethodToClass(Class *class, Method *method) {
  MethodFlags flags = method->flags;

  assert(!class->native);

  if (flags & wsky_MethodFlags_INIT)
    class->constructor = method;
  else if (flags & wsky_MethodFlags_SET)
    wsky_Dict_set(class->setters, method->name, method);
  else
    wsky_Dict_set(class->methods, method->name, method);
}


static ReturnValue defaultConstructor(void) {
  RETURN_NULL;
}

static Method *createDefaultConstructor(Class *class) {
  MethodDef def = {
    "init",
    0,
    wsky_MethodFlags_PUBLIC | wsky_MethodFlags_INIT,
    (wsky_Method0)&defaultConstructor,
  };
  return wsky_Method_newFromC(&def, class);
}

static ReturnValue getSuperclass(const ClassNode *classNode,
                                 Scope *scope) {
  if (!classNode->superclass)
    RETURN_OBJECT((Object *)wsky_Object_CLASS);

  return wsky_evalNode(classNode->superclass, scope);
}

/** Returns a new class, without methods */
static ReturnValue createClass(const ClassNode *classNode,
                               Scope *scope) {
  ReturnValue rv = getSuperclass(classNode, scope);
  if (rv.exception)
    return rv;
  if (!wsky_isClass(rv.v))
    RAISE_NEW_PARAMETER_ERROR("Invalid superclass");

  Class *super = (Class *)rv.v.v.objectValue;
  if (super->final)
    RAISE_NEW_PARAMETER_ERROR("Cannot extend a final class");

  Class *class = wsky_Class_new(classNode->name, super);
  if (!class)
    RAISE_NEW_EXCEPTION("Class creation failed");

  RETURN_OBJECT((Object *)class);
}

static ReturnValue evalClass(const ClassNode *classNode, Scope *scope) {
  ReturnValue rv = createClass(classNode, scope);
  if (rv.exception)
    return rv;
  Class *class = (Class *)rv.v.v.objectValue;

  for (NodeList *list = classNode->children; list; list = list->next) {
    Node *node = list->node;
    assert(node->type == wsky_ASTNodeType_CLASS_MEMBER);
    ClassMemberNode *member = (ClassMemberNode *)node;
    rv = evalClassMember(class, member, scope);
    if (rv.exception)
      return rv;
    addMethodToClass(class, (Method *)rv.v.v.objectValue);
  }

  if (!class->constructor)
    class->constructor = createDefaultConstructor(class);

  Value classValue = Value_fromObject((Object *)class);
  return declareVariable(class->name, classValue, scope);
}


/** Returns NULL if there is no builtin module with this name */
static Module *getBuiltinModuleFromName(const char *name) {
  ModuleList *modules = wsky_Module_getModules();

  while (modules) {
    Module *module = modules->module;
    if (module->builtin && strcmp(module->name, name) == 0)
      return module;
    modules = modules->next;
  }
  return NULL;
}

static char *getAbsoluteModuleFilePath(unsigned level, const char *name,
                                       const char *currentDirAbsPath) {
  assert(level != 0);
  assert(currentDirAbsPath[0] == '/');

  if (level == 1) {
    char *left = wsky_path_concat(currentDirAbsPath, name);
    const char *extension = ".wsky";
    char *s = wsky_asprintf("%s%s", left, extension);
    wsky_free(left);
    return s;
  }

  char *parent = wsky_path_getDirectoryPath(currentDirAbsPath);
  if (!parent)
    return NULL;
  char *filePath = getAbsoluteModuleFilePath(level, name, parent);
  wsky_free(parent);
  return filePath;
}

static Module *getCachedModuleFromPath(const char *targetPath,
                                       Module *currentModule) {
  ModuleList *modules = wsky_Module_getModules();

  int i = 0;
  while (modules) {
    Module *module = modules->module;
    if (!module->builtin && module != currentModule) {
      const char *path = module->file->absolutePath;
      if (!module->builtin && strcmp(targetPath, path) == 0)
        return module;
    }
    i++;
    modules = modules->next;
  }
  return NULL;
}

static Module *getCachedModule(const char *targetPath,
                               Module *currentModule) {
  if (!targetPath)
    return NULL;
  Module *module = getCachedModuleFromPath(targetPath, currentModule);
  return module;
}

static ReturnValue raiseNoModuleNamed(const char *name) {
  char *s = wsky_asprintf("No module named '%s'", name);
  ImportError *e = wsky_ImportError_new(s);
  wsky_free(s);
  RAISE_EXCEPTION((Exception *)e);
}

static ReturnValue evalImport(const ImportNode *node, Scope *scope) {
  Module *module;

  if (node->level == 0) {

    module = getBuiltinModuleFromName(node->name);

  } else {

    ProgramFile *file = node->position.file;
    char *targetPath = getAbsoluteModuleFilePath(node->level, node->name,
                                                 file->directoryPath);
    module = getCachedModule(targetPath, wsky_Scope_getModule(scope));

    if (!module) {
      ReturnValue rv = wsky_evalModuleFile(targetPath);
      if (rv.exception) {
        wsky_free(targetPath);
        return rv;
      }
      module = (Module *)rv.v.v.objectValue;
    }
    wsky_free(targetPath);

  }

  if (!module)
    return raiseNoModuleNamed(node->name);

  return declareVariable(module->name,
                         Value_fromObject((Object *)module),
                         scope);
}


static ReturnValue evalExport(const ExportNode *node, Scope *scope) {
  Value value;

  if (node->right) {
    ReturnValue rv = wsky_evalNode(node->right, scope);
    if (rv.exception)
      return rv;
    value = rv.v;
    declareVariable(node->name, value, scope);
  } else {
    if (!wsky_Scope_containsVariable(scope, node->name))
      return raiseUndeclaredNameError(node->name);
    value = wsky_Scope_getVariable(scope, node->name);
  }

  Module *module = wsky_Scope_getModule(scope);
  wsky_Module_addValue(module, node->name, value);
  RETURN_VALUE(value);
}


static ReturnValue evalIf(const IfNode *node, Scope *scope) {
  NodeList *tests = node->tests;
  NodeList *expressions = node->expressions;
  while (tests) {
    assert(expressions);

    ReturnValue rv = wsky_evalNode(tests->node, scope);
    if (rv.exception)
      return rv;
    if (!wsky_isBoolean(rv.v))
      RAISE_NEW_TYPE_ERROR("Expected a boolean");
    if (rv.v.v.boolValue)
      return wsky_evalNode(expressions->node, scope);

    expressions = expressions->next;
    tests = tests->next;
  }

  if (node->elseNode)
    return wsky_evalNode(node->elseNode, scope);

  RETURN_NULL;
}



ReturnValue wsky_evalNode(const Node *node, Scope *scope) {
#define CASE(type) case wsky_ASTNodeType_ ## type
  switch (node->type) {

  CASE(NULL):
    RETURN_NULL;

  CASE(BOOL):
    RETURN_BOOL(TO_LITERAL_NODE(node)->v.boolValue);

  CASE(INT):
    RETURN_INT(TO_LITERAL_NODE(node)->v.intValue);

  CASE(FLOAT):
    RETURN_FLOAT(TO_LITERAL_NODE(node)->v.floatValue);

  CASE(SEQUENCE):
    return evalSequence((const SequenceNode *) node, scope);

  CASE(STRING):
    RETURN_C_STRING(TO_LITERAL_NODE(node)->v.stringValue);

  CASE(UNARY_OPERATOR):
  CASE(BINARY_OPERATOR):
    return evalOperator((const OperatorNode *) node, scope);

  CASE(VAR):
    return evalVar((const VarNode *) node, scope);

  CASE(IDENTIFIER):
    return evalIdentifier((const IdentifierNode *) node, scope);

  CASE(SELF):
    return evalSelf(scope);

  CASE(SUPER):
    return evalSuper(scope);

  CASE(SUPERCLASS):
    return evalSuperclass(scope);

  CASE(ASSIGNMENT):
    return evalAssignment((const AssignmentNode *) node, scope);

  CASE(FUNCTION):
    return evalFunction((const FunctionNode *) node, scope);

  CASE(CALL):
    return evalCall((const CallNode *) node, scope);

  CASE(MEMBER_ACCESS):
    return evalMemberAccess((const MemberAccessNode *) node, scope);

  CASE(CLASS):
    return evalClass((const ClassNode *) node, scope);

  CASE(IMPORT):
    return evalImport((const ImportNode *) node, scope);

  CASE(EXPORT):
    return evalExport((const ExportNode *) node, scope);

  CASE(IF):
    return evalIf((const IfNode *) node, scope);

  default:
    fprintf(stderr,
            "wsky_evalNode(): Unsupported node type %d\n",
            node->type);
    abort();
  }
#undef CASE
}


static ReturnValue raiseSyntaxError(ParserResult pr) {
  char *msg = wsky_SyntaxError_toString(&pr.syntaxError);
  SyntaxErrorEx *e = wsky_SyntaxErrorEx_new(&pr.syntaxError);
  wsky_SyntaxError_free(&pr.syntaxError);
  wsky_free(msg);
  RAISE_EXCEPTION((Exception *)e);
}

/**
 * @param pr The parser result
 * @param scope The scope or NULL
 */
static ReturnValue evalFromParserResult(ParserResult pr, Scope *scope) {
  if (!pr.success)
    return raiseSyntaxError(pr);

  if (!scope)
    scope = wsky_Scope_newRoot(wsky_Module_newMain());

  wsky_eval_pushScope(scope);

  ReturnValue rv = wsky_evalNode(pr.node, scope);
  wsky_ASTNode_delete(pr.node);

  wsky_eval_popScope();

  return rv;
}


ReturnValue wsky_evalString(const char *source) {
  return evalFromParserResult(wsky_parseString(source), NULL);
}


ReturnValue wsky_evalFile(const char *filePath) {
  ReturnValue rv = wsky_ProgramFile_new(filePath);
  if (rv.exception)
    return rv;

  ProgramFile *file = (ProgramFile *)rv.v.v.objectValue;
  return evalFromParserResult(wsky_parseFile(file), NULL);
}

static bool isIdentifierStartChar(char c) {
  return isalpha(c) || isdigit(c);
}

static bool isIdentifierChar(char c) {
  return isIdentifierStartChar(c) || c == '_';
}

static bool isValidIdentifier(const char *string) {
  if (!isIdentifierStartChar(*string))
    return false;
  string++;
  while (isIdentifierChar(*string))
    string++;
  return *string == '\0';
}

ReturnValue wsky_evalModuleFile(const char *filePath) {
  ReturnValue rv = wsky_ProgramFile_new(filePath);
  if (rv.exception)
    return rv;

  ProgramFile *file = (ProgramFile *)rv.v.v.objectValue;
  char *name = wsky_path_removeExtension(file->name);
  if (!isValidIdentifier(name)) {
    wsky_free(name);
    RAISE_NEW_EXCEPTION("Invalid module file name");
  }

  Module *module = wsky_Module_new(name, false, file);
  wsky_free(name);

  Scope *scope = wsky_Scope_newRoot(module);
  rv = evalFromParserResult(wsky_parseFile(file), scope);
  if (rv.exception)
    return rv;

  RETURN_OBJECT((Object *)module);
}
