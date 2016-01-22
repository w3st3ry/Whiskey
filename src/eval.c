#include "eval.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "gc.h"

#include "objects/class.h"
#include "objects/function.h"
#include "objects/str.h"
#include "objects/boolean.h"
#include "objects/integer.h"
#include "objects/float.h"
#include "objects/instance_method.h"
#include "objects/method.h"

#include "objects/attribute_error.h"
#include "objects/exception.h"
#include "objects/syntax_error_ex.h"
#include "objects/type_error.h"
#include "objects/not_implemented_error.h"


typedef wsky_Object Object;
typedef wsky_Class Class;
typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;
typedef wsky_Scope Scope;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_Value Value;
typedef wsky_LiteralNode LiteralNode;


#define TO_LITERAL_NODE(n) ((LiteralNode *) (n))

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

  wsky_RETURN_NEW_TYPE_ERROR(message);
}

static ReturnValue createUnsupportedUnaryOpError(const char *operator,
                                                 const char *rightClass) {
  char message[128];
  snprintf(message, 127,
           "Unsupported class for unary %s: %s",
           operator,
           rightClass);

  wsky_RETURN_NEW_TYPE_ERROR(message);
}


/*
 * Returns a new `NotImplementedException`
 */
#define RETURN_NOT_IMPL(operator)                               \
  wsky_RETURN_EXCEPTION(wsky_NotImplementedError_new(operator))


/*
 * Returns true if the given object is not null and is a
 * `NotImplementedException`
 */
#define IS_NOT_IMPLEMENTED_ERROR(e)                     \
  ((e) && (e)->class == wsky_NotImplementedError_CLASS)


#include "eval_int.c"
#include "eval_float.c"
#include "eval_bool.c"


static const char *getBinOperatorMethodName(wsky_Operator operator,
                                            bool right) {
  static char buffer[64];
  snprintf(buffer, 63, "operator %s%s",
           right ? "r" : "",
           wsky_Operator_toString(operator));
  return buffer;
}

static ReturnValue evalBinOperatorValues(Value left,
                                         wsky_Operator operator,
                                         Value right,
                                         bool reverse) {
  switch (left.type) {
  case wsky_Type_BOOL:
    return evalBinOperatorBool(left.v.boolValue, operator, right);

  case wsky_Type_INT:
    return evalBinOperatorInt(left.v.intValue, operator, right);

  case wsky_Type_FLOAT:
    return evalBinOperatorFloat(left.v.floatValue, operator, right);

  case wsky_Type_OBJECT: {
    const char *method = getBinOperatorMethodName(operator, reverse);
    Object *object = left.v.objectValue;
    return wsky_Object_callMethod1(object, method, right);
  }
  }
}


static ReturnValue evalUnaryOperatorValues(wsky_Operator operator,
                                           Value right) {

  switch (right.type) {
  case wsky_Type_BOOL:
    return evalUnaryOperatorBool(operator, right.v.boolValue);

  case wsky_Type_INT:
    return evalUnaryOperatorInt(operator, right.v.intValue);

  case wsky_Type_FLOAT:
    return evalUnaryOperatorFloat(operator, right.v.floatValue);

  default:
    return createUnsupportedUnaryOpError(wsky_Operator_toString(operator),
                                         wsky_getClassName(right));
  }
}

static ReturnValue evalBinOperator(const Node *leftNode,
                                   wsky_Operator operator,
                                   const Node *rightNode,
                                   Scope *scope) {
  ReturnValue leftRV = wsky_evalNode(leftNode, scope);
  if (leftRV.exception) {
    return leftRV;
  }
  ReturnValue rightRV = wsky_evalNode(rightNode, scope);
  if (rightRV.exception) {
    return rightRV;
  }

  ReturnValue rv = evalBinOperatorValues(leftRV.v, operator, rightRV.v,
                                         false);
  if (!IS_NOT_IMPLEMENTED_ERROR(rv.exception)) {
    return rv;
  }

  ReturnValue rev;
  rev = evalBinOperatorValues(rightRV.v, operator, leftRV.v, true);
  if (!IS_NOT_IMPLEMENTED_ERROR(rev.exception)) {
    if (rev.exception) {
      printf("! %s\n", rev.exception->class->name);
    }
    return rev;
  }

  rev = evalBinOperatorValues(rightRV.v, operator, leftRV.v, false);
  if (!IS_NOT_IMPLEMENTED_ERROR(rev.exception)) {
    return rev;
  }

  return createUnsupportedBinOpError(wsky_getClassName(leftRV.v),
                                     wsky_Operator_toString(operator),
                                     rightRV.v);
}


static ReturnValue evalUnaryOperator(wsky_Operator operator,
                                     const Node *rightNode,
                                     Scope *scope) {
  ReturnValue rightRV = wsky_evalNode(rightNode, scope);
  if (rightRV.exception) {
    return rightRV;
  }
  return evalUnaryOperatorValues(operator, rightRV.v);
}

static ReturnValue evalOperator(const wsky_OperatorNode *n, Scope *scope) {
  wsky_Operator op = n->operator;
  Node *leftNode = n->left;
  Node *rightNode = n->right;
  if (leftNode) {
    return evalBinOperator(leftNode, op, rightNode, scope);
  }
  return evalUnaryOperator(op, rightNode, scope);
}


static ReturnValue evalSequence(const wsky_SequenceNode *n,
                                Scope *parentScope) {
  Scope *scope = wsky_Scope_new(parentScope, NULL);
  NodeList *child = n->children;
  ReturnValue rv = wsky_ReturnValue_NULL;
  while (child) {
    rv = wsky_evalNode(child->node, scope);
    if (rv.exception)
      return rv;
    child = child->next;
  }
  return rv;
}

static ReturnValue createAlreadyDeclaredNameError(const char *name) {
  char *message = malloc(40 + strlen(name));
  sprintf(message, "Identifier '%s' already declared", name);
  wsky_Exception *e = wsky_Exception_new(message, NULL);
  free(message);
  wsky_RETURN_EXCEPTION(e);
}

static ReturnValue evalVar(const wsky_VarNode *n, Scope *scope) {
  if (wsky_Scope_containsVariableLocally(scope, n->name))
    return createAlreadyDeclaredNameError(n->name);

  Value value = wsky_Value_NULL;
  if (n->right) {
    ReturnValue rv = wsky_evalNode(n->right, scope);
    if (rv.exception) {
      return rv;
    }
    value = rv.v;
  }
  wsky_Scope_addVariable(scope, n->name, value);
  wsky_RETURN_VALUE(value);
}


static ReturnValue raiseUndeclaredNameError(const char *name) {
  char *message = malloc(40 + strlen(name));
  sprintf(message, "Use of undeclared identifier '%s'", name);
  wsky_Exception *e = wsky_Exception_new(message, NULL);
  free(message);
  wsky_RETURN_EXCEPTION(e);
}

static ReturnValue evalIdentifier(const wsky_IdentifierNode *n,
                                  Scope *scope) {
  const char *name = n->name;
  if (*name == '@') {
    if (!scope->self)
      wsky_RETURN_NEW_EXCEPTION("'@' used outside of a class");
    wsky_RETURN_OBJECT(scope->self);
  }
  if (!wsky_Scope_containsVariable(scope, name)) {
    return raiseUndeclaredNameError(name);
  }
  wsky_RETURN_VALUE(wsky_Scope_getVariable(scope, name));
}


static ReturnValue assignToVariable(Value right,
                                    const char *name,
                                    Scope *scope) {

  if (!wsky_Scope_containsVariable(scope, name))
    return raiseUndeclaredNameError(name);

  wsky_Scope_setVariable(scope, name, right);
  return wsky_ReturnValue_fromValue(right);
}

static wsky_Exception *createImmutableObjectError(Value value) {
    const char *className = wsky_getClassName(value);
    char *message = malloc(40 + strlen(className));
    sprintf(message, "'%s' objects are immutables", className);
    wsky_TypeError *e = wsky_TypeError_new(message);
    free(message);
    return (wsky_Exception *)e;
}

static ReturnValue assignToMember(Node *leftNode,
                                  const char *memberName,
                                  Value right,
                                  Scope *scope) {
  ReturnValue rv = wsky_evalNode(leftNode, scope);
  if (rv.exception)
    return rv;

  if (rv.v.type != wsky_Type_OBJECT)
    wsky_RETURN_EXCEPTION(createImmutableObjectError(rv.v));

  Object *object = rv.v.v.objectValue;

  if (object->class->native)
    wsky_RETURN_EXCEPTION(createImmutableObjectError(rv.v));

  bool privateAccess = object == scope->self;
  return wsky_Object_set(object, memberName, &right, privateAccess);
}

static ReturnValue evalAssignement(const wsky_AssignmentNode *n,
                                   Scope *scope) {

  Node *leftNode = n->left;

  ReturnValue right = wsky_evalNode(n->right, scope);
  if (right.exception)
    return right;

  if (leftNode->type == wsky_ASTNodeType_IDENTIFIER) {
    wsky_IdentifierNode *id = (wsky_IdentifierNode *) leftNode;
    return assignToVariable(right.v, id->name, scope);
  }
  if (leftNode->type == wsky_ASTNodeType_MEMBER_ACCESS) {
    wsky_MemberAccessNode *member = (wsky_MemberAccessNode *) leftNode;
    return assignToMember(member->left, member->name, right.v, scope);
  }

  wsky_RETURN_NEW_EXCEPTION("Not assignable expression");
}


static ReturnValue evalFunction(const wsky_FunctionNode *n, Scope *scope) {
  wsky_Function *function = wsky_Function_new(n->name, n, scope);
  wsky_RETURN_OBJECT((wsky_Object *) function);
}


static Value *evalParameters(const NodeList *nodes,
                             wsky_Exception **exceptionPointer,
                             Scope *scope) {
  unsigned paramCount = wsky_ASTNodeList_getCount(nodes);
  Value *values = wsky_safeMalloc(sizeof(Value) * paramCount);
  unsigned i;
  for (i = 0; i < paramCount; i++) {
    ReturnValue rv = wsky_evalNode(nodes->node, scope);
    if (rv.exception) {
      *exceptionPointer = rv.exception;
      wsky_free(values);
      return NULL;
    }
    values[i] = rv.v;
    nodes = nodes->next;
  }
  return values;
}


static ReturnValue callMethod(Object *instanceMethod_,
                              unsigned parameterCount,
                              Value *parameters) {
  wsky_InstanceMethod *instanceMethod;
  instanceMethod = (wsky_InstanceMethod *) instanceMethod_;
  wsky_Method *method = instanceMethod->method;

  Value *self = &instanceMethod->self;

  if (self->type == wsky_Type_OBJECT && self->v.objectValue) {
    return wsky_Method_call(method,
                            self->v.objectValue,
                            parameterCount,
                            parameters);
  }

  return wsky_Method_callValue(method,
                               *self,
                               parameterCount,
                               parameters);
}

static ReturnValue callFunction(wsky_Function *function,
                                unsigned parameterCount,
                                Value *parameters) {
  return wsky_Function_call(function, NULL, parameterCount, parameters);
}

static inline ReturnValue callClass(wsky_Class *class,
                                    unsigned parameterCount,
                                    Value *parameters) {
  return wsky_Class_construct(class, parameterCount, parameters);
}

static wsky_Exception *createNotCallableError(Value value) {
  const char *className = wsky_getClassName(value);
  char *message = wsky_safeMalloc(strlen(className) + 40);
  sprintf(message, "A %s is not callable", className);
  return wsky_Exception_new(message, NULL);
}

static ReturnValue evalCall(const wsky_CallNode *callNode, Scope *scope) {
  ReturnValue rv = wsky_evalNode(callNode->left, scope);
  if (rv.exception)
    return rv;

  wsky_Exception *exception;
  Value *parameters = evalParameters(callNode->children, &exception, scope);
  if (!parameters) {
    wsky_RETURN_EXCEPTION(exception);
  }
  unsigned paramCount = wsky_ASTNodeList_getCount(callNode->children);

  if (rv.v.type != wsky_Type_OBJECT) {
    wsky_RETURN_NEW_EXCEPTION("Only methods and functions are callable");
  }

  if (wsky_isFunction(rv.v)) {
    Object *function = rv.v.v.objectValue;
    rv = callFunction((wsky_Function *) function, paramCount, parameters);

  } else if (wsky_isInstanceMethod(rv.v)) {
    Object *instMethod = rv.v.v.objectValue;
    rv = callMethod(instMethod, paramCount, parameters);

  } else if (wsky_isClass(rv.v)) {
    Class *class = (Class *)rv.v.v.objectValue;
    rv = callClass(class, paramCount, parameters);

  } else {
    rv = wsky_ReturnValue_fromException(createNotCallableError(rv.v));
  }

  wsky_free(parameters);
  return rv;
}

static ReturnValue getMemberOfNativeClass(const Value *self,
                                          const char *name) {
  Class *class = wsky_getClass(*self);

  wsky_Method *method = wsky_Class_findMethodOrGetter(class, name);
  if (!method)
    return wsky_AttributeError_raiseNoAttr(class->name, name);
  if (method->flags & wsky_MethodFlags_GET) {
    if (method->flags & wsky_MethodFlags_VALUE)
      return wsky_Method_callValue0(method, *self);
    else
      return wsky_Method_call0(method, self->v.objectValue);
  }

  wsky_InstanceMethod *im = wsky_InstanceMethod_new(method, self);
  wsky_RETURN_OBJECT((Object *)im);
}

static ReturnValue evalMemberAccess(const wsky_MemberAccessNode *dotNode,
                                    Scope *scope) {
  ReturnValue rv = wsky_evalNode(dotNode->left, scope);
  if (rv.exception)
    return rv;

  if (rv.v.type != wsky_Type_OBJECT)
    return getMemberOfNativeClass(&rv.v, dotNode->name);

  Object *object = rv.v.v.objectValue;

  if (wsky_Object_getClass(object)->native)
    return getMemberOfNativeClass(&rv.v, dotNode->name);

  bool privateAccess = object == scope->self;
  if (privateAccess)
    return wsky_Object_getPrivate(object, dotNode->name);
  else
    return wsky_Object_get(object, dotNode->name);
}


static ReturnValue evalClassMember(const wsky_ClassMemberNode *memberNode,
                                   Scope *scope) {
  wsky_Function *right = NULL;
  if (memberNode->right) {
    ReturnValue rv = wsky_evalNode(memberNode->right, scope);
    if (rv.exception)
      return rv;
    assert(wsky_isFunction(rv.v));
    right = (wsky_Function *)rv.v.v.objectValue;
  } else {
    assert((memberNode->flags & wsky_MethodFlags_SET) ||
           (memberNode->flags & wsky_MethodFlags_GET));

    wsky_Method *method = wsky_Method_newFromWskyDefault(memberNode->name,
                                                         memberNode->flags);
    wsky_RETURN_OBJECT((Object *)method);
  }

  wsky_Method *method = wsky_Method_newFromWsky(right, memberNode->flags);
  wsky_RETURN_OBJECT((Object *)method);
}


static void addMethodToClass(Class *class, wsky_Method *method) {
  wsky_MethodFlags flags = method->flags;

  assert(!class->native);

  if (flags & wsky_MethodFlags_INIT)
    class->constructor = method;
  else if (flags & wsky_MethodFlags_SET)
    wsky_Dict_set(class->setters, method->name, method);
  else
    wsky_Dict_set(class->methods, method->name, method);
}


static ReturnValue defaultConstructor(void) {
  return wsky_ReturnValue_NULL;
}

static ReturnValue evalClass(const wsky_ClassNode *classNode, Scope *scope) {
  Class *class = wsky_Class_new(classNode->name, wsky_Object_CLASS);
  if (!class)
    wsky_RETURN_NEW_EXCEPTION("Class creation failed");

  for (NodeList *list = classNode->children; list; list = list->next) {
    Node *node = list->node;
    assert(node->type == wsky_ASTNodeType_CLASS_MEMBER);
    wsky_ClassMemberNode *member = (wsky_ClassMemberNode *)node;
    ReturnValue rv = evalClassMember(member, scope);
    if (rv.exception)
      return rv;
    addMethodToClass(class, (wsky_Method *)rv.v.v.objectValue);
  }

  if (!class->constructor) {
    wsky_MethodDef def = {
      "init",
      0,
      wsky_MethodFlags_PUBLIC,
      (void *)&defaultConstructor,
    };
    class->constructor = wsky_Method_newFromC(&def);
  }

  if (wsky_Scope_containsVariableLocally(scope, class->name)) {
    return createAlreadyDeclaredNameError(class->name);
  }

  Value classValue = wsky_Value_fromObject((Object *)class);
  wsky_Scope_addVariable(scope, class->name, classValue);

  wsky_RETURN_OBJECT((Object *)class);
}


ReturnValue wsky_evalNode(const Node *node, Scope *scope) {
#define CASE(type) case wsky_ASTNodeType_ ## type
  switch (node->type) {

  CASE(NULL):
    return wsky_ReturnValue_NULL;

  CASE(BOOL):
    wsky_RETURN_BOOL(TO_LITERAL_NODE(node)->v.boolValue);

  CASE(INT):
    wsky_RETURN_INT(TO_LITERAL_NODE(node)->v.intValue);

  CASE(FLOAT):
    wsky_RETURN_FLOAT(TO_LITERAL_NODE(node)->v.floatValue);

  CASE(SEQUENCE):
    return evalSequence((const wsky_SequenceNode *) node, scope);

  CASE(STRING):
    wsky_RETURN_CSTRING(TO_LITERAL_NODE(node)->v.stringValue);

  CASE(UNARY_OPERATOR):
  CASE(BINARY_OPERATOR):
    return evalOperator((const wsky_OperatorNode *) node, scope);

  CASE(VAR):
    return evalVar((const wsky_VarNode *) node, scope);

  CASE(IDENTIFIER):
    return evalIdentifier((const wsky_IdentifierNode *) node, scope);

  CASE(ASSIGNMENT):
    return evalAssignement((const wsky_AssignmentNode *) node, scope);

  CASE(FUNCTION):
    return evalFunction((const wsky_FunctionNode *) node, scope);

  CASE(CALL):
    return evalCall((const wsky_CallNode *) node, scope);

  CASE(MEMBER_ACCESS):
    return evalMemberAccess((const wsky_MemberAccessNode *) node, scope);

  CASE(CLASS):
    return evalClass((const wsky_ClassNode *) node, scope);

  default:
    fprintf(stderr,
            "wsky_evalNode(): Unsupported node type %d\n",
            node->type);
    abort();
  }
#undef CASE
}


wsky_ReturnValue wsky_evalString(const char *source) {
  wsky_ParserResult pr = wsky_parseString(source);
  if (!pr.success) {
    char *msg = wsky_SyntaxError_toString(&pr.syntaxError);
    wsky_SyntaxErrorEx *e = wsky_SyntaxErrorEx_new(&pr.syntaxError);
    wsky_SyntaxError_free(&pr.syntaxError);
    wsky_free(msg);
    wsky_RETURN_EXCEPTION(e);
  }
  Scope *scope = wsky_Scope_newRoot();
  ReturnValue v = wsky_evalNode(pr.node, scope);
  wsky_ASTNode_delete(pr.node);

  wsky_GC_unmarkAll();
  wsky_GC_visitBuiltins();
  if (v.exception)
    wsky_GC_VISIT(v.exception);
  else
    wsky_GC_VISIT_VALUE(v.v);
  wsky_GC_collect();

  return v;
}
