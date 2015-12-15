#include "eval.h"

#include <stdlib.h>
#include "parser.h"
#include "exception.h"

typedef wsky_ASTNode Node;
typedef wsky_Scope Scope;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_Value Value;

#define TO_LITERAL_NODE(n) ((wsky_LiteralNode *) (n))

static ReturnValue evalSequence(const wsky_SequenceNode *n, Scope *scope) {
  wsky_ASTNodeList *child = n->children;
  ReturnValue last = wsky_ReturnValue_NULL;
  while (child) {
    last = wsky_evalNode(child->node, scope);
    child = child->next;
  }
  return last;
}

ReturnValue wsky_evalNode(const Node *node, Scope *scope) {
  switch (node->type) {
  case wsky_ASTNodeType_INT:
    return wsky_ReturnValue_fromInt(TO_LITERAL_NODE(node)->v.intValue);
  case wsky_ASTNodeType_FLOAT:
    return wsky_ReturnValue_fromFloat(TO_LITERAL_NODE(node)->v.floatValue);

  case wsky_ASTNodeType_SEQUENCE:
    return evalSequence((const wsky_SequenceNode *) node, scope);

  default:
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
