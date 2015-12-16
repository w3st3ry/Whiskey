#include "eval.h"

#include <stdlib.h>
#include "parser.h"
#include "exception.h"
#include "str.h"

typedef wsky_ASTNode Node;
typedef wsky_Scope Scope;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_Value Value;

#define TO_LITERAL_NODE(n) ((wsky_LiteralNode *) (n))

#define NodeType_INT wsky_ASTNodeType_INT
#define NodeType_FLOAT wsky_ASTNodeType_FLOAT
//#define NodeType_BOOL wsky_ASTNodeType_BOOL
#define NodeType_SEQUENCE wsky_ASTNodeType_SEQUENCE
#define NodeType_STRING wsky_ASTNodeType_STRING

#define NodeType_UNARY_OPERATOR wsky_ASTNodeType_UNARY_OPERATOR
#define NodeType_BINARY_OPERATOR wsky_ASTNodeType_BINARY_OPERATOR
#define NodeType_VAR wsky_ASTNodeType_VAR

#define IS_INT(value) ((value).type == wsky_Type_INT)
#define IS_FLOAT(value) ((value).type == wsky_Type_FLOAT)



#define EVAL_BIN_OP_TEMPLATE_CMP_INT(op, opName, left, right)   \
  if (IS_INT(left) && IS_INT(right)) {                          \
    int64_t l = (left).v.intValue;                              \
    int64_t r = (right).v.intValue;                             \
    wsky_RETURN_BOOL(l op r);                                   \
  } else {                                                      \
    wsky_RETURN_NEW_EXCEPTION("Unimplemented operator " #op);   \
  }

#define EVAL_BIN_OP_TEMPLATE(op, opName, left, right)           \
  if (IS_INT(left) && IS_INT(right)) {                          \
    int64_t l = (left).v.intValue;                              \
    int64_t r = (right).v.intValue;                             \
    wsky_RETURN_INT(l op r);                                    \
  } else if (IS_FLOAT(left) && IS_INT(right)) {                 \
    double l = (left).v.floatValue;                             \
    int64_t r = (right).v.intValue;                             \
    wsky_RETURN_FLOAT(l op r);                                  \
  } else if (IS_INT(left) && IS_FLOAT(right)) {                 \
    int64_t l = (left).v.intValue;                              \
    double r = (right).v.floatValue;                            \
    wsky_RETURN_FLOAT(l op r);                                  \
  } else if (IS_FLOAT(left) && IS_FLOAT(right)) {               \
    double l = (left).v.floatValue;                             \
    double r = (right).v.floatValue;                            \
    wsky_RETURN_FLOAT(l op r);                                  \
  } else {                                                      \
    wsky_RETURN_NEW_EXCEPTION("Unimplemented operator " #op);   \
  }

#define EVAL_BIN_OP_TEMPLATE_CMP(op, opName, left, right)       \
  if (IS_INT(left) && IS_INT(right)) {                          \
    int64_t l = (left).v.intValue;                              \
    int64_t r = (right).v.intValue;                             \
    wsky_RETURN_BOOL(l op r);                                   \
  } else if (IS_FLOAT(left) && IS_INT(right)) {                 \
    double l = (left).v.floatValue;                             \
    int64_t r = (right).v.intValue;                             \
    wsky_RETURN_BOOL(l op r);                                   \
  } else if (IS_INT(left) && IS_FLOAT(right)) {                 \
    int64_t l = (left).v.intValue;                              \
    double r = (right).v.floatValue;                            \
    wsky_RETURN_BOOL(l op r);                                   \
  } else if (IS_FLOAT(left) && IS_FLOAT(right)) {               \
    double l = (left).v.floatValue;                             \
    double r = (right).v.floatValue;                            \
    wsky_RETURN_BOOL(l op r);                                   \
  } else {                                                      \
    wsky_RETURN_NEW_EXCEPTION("Unimplemented operator " #op);   \
  }


static ReturnValue evalBinOperator(const Node *leftNode,
                                   wsky_Operator op,
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
  Value left = leftRV.v;
  Value right = rightRV.v;

#define CASE(op, opName)                                \
  case wsky_Operator_ ## opName: {                      \
    EVAL_BIN_OP_TEMPLATE(op, opName, left, right);      \
  }
#define CASE_CMP(op, opName)                            \
  case wsky_Operator_ ## opName: {                      \
    EVAL_BIN_OP_TEMPLATE_CMP(op, opName, left, right);  \
  }
#define CASE_CMP_INT(op, opName)                                \
  case wsky_Operator_ ## opName: {                              \
    EVAL_BIN_OP_TEMPLATE_CMP_INT(op, opName, left, right);      \
  }

  switch (op) {
    CASE(+, PLUS); CASE(-, MINUS);
    CASE(*, STAR); CASE(/, SLASH);

    CASE_CMP_INT(==, EQUALS); CASE_CMP_INT(!=, NOT_EQUALS);
    CASE_CMP(<, LT); CASE_CMP(>, GT);
    CASE_CMP_INT(<=, LT_EQ); CASE_CMP_INT(>=, GT_EQ);

  default:
    wsky_RETURN_NEW_EXCEPTION("Unimplemented operator");
  }
#undef CASE
#undef CASE_CMP
#undef CASE_CMP_INT
}


static ReturnValue evalUnaryOperator(wsky_Operator op,
                                     const Node *rightNode,
                                     Scope *scope) {
  ReturnValue rightRV = wsky_evalNode(rightNode, scope);
  if (rightRV.exception) {
    return rightRV;
  }
  Value right = rightRV.v;

#define CASE(op, opName)                        \
  case wsky_Operator_ ## opName:                \
    if (IS_FLOAT(right)) {                      \
      wsky_RETURN_FLOAT(op right.v.floatValue); \
    } else if (IS_INT(right)) {                 \
      wsky_RETURN_INT(op right.v.intValue);     \
    }

  switch (op) {
    CASE(+, PLUS);
    CASE(-, MINUS);

  default:
    wsky_RETURN_NEW_EXCEPTION("Unimplemented unary operator");
  }

#undef CASE
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


static ReturnValue evalSequence(const wsky_SequenceNode *n, Scope *scope) {
  wsky_ASTNodeList *child = n->children;
  ReturnValue last = wsky_ReturnValue_NULL;
  while (child) {
    last = wsky_evalNode(child->node, scope);
    child = child->next;
  }
  return last;
}


static ReturnValue evalVar(const wsky_VarNode *n, Scope *scope) {
  ReturnValue rv = wsky_evalNode(n->right, scope);
  if (rv.exception) {
    return rv;
  }
  wsky_Scope_addVariable(scope, n->name, rv.v);
  return rv;
}


ReturnValue wsky_evalNode(const Node *node, Scope *scope) {
  switch (node->type) {
  case NodeType_INT:
    wsky_RETURN_INT(TO_LITERAL_NODE(node)->v.intValue);

  case NodeType_FLOAT:
    wsky_RETURN_FLOAT(TO_LITERAL_NODE(node)->v.floatValue);

  case NodeType_SEQUENCE:
    return evalSequence((const wsky_SequenceNode *) node, scope);

  case NodeType_STRING:
    wsky_RETURN_CSTRING(TO_LITERAL_NODE(node)->v.stringValue);

  case NodeType_UNARY_OPERATOR:
  case NodeType_BINARY_OPERATOR:
    return evalOperator((const wsky_OperatorNode *) node, scope);

  case NodeType_VAR:
    return evalVar((const wsky_VarNode *) node, scope);

  default:
    fprintf(stderr,
            "wsky_evalNode(): Unsupported node type %d\n",
            node->type);
    abort();
  }
}


wsky_ReturnValue wsky_evalString(const char *source) {
  wsky_TokenList *tokens;
  wsky_ParserResult pr = wsky_parseString(source, &tokens);
  if (!pr.success) {
    const char *msg = wsky_SyntaxError_toString(&pr.syntaxError);
    wsky_RETURN_NEW_EXCEPTION(msg);
  }
  Scope *scope = wsky_Scope_new(NULL, NULL);
  ReturnValue v = wsky_evalNode(pr.node, scope);
  wsky_Scope_delete(scope);
  wsky_ASTNode_delete(pr.node);
  wsky_TokenList_delete(tokens);
  return v;
}
