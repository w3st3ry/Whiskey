#include "ast.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "gc.h"


typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;
typedef wsky_Token Token;



#define D(name)                                                         \
  typedef wsky_##name##Node name##Node;                                 \
                                                                        \
  static void name##Node_copy(const name##Node *src, name##Node *new);  \
  static void name##Node_free(name##Node *node);                        \
  static char *name##Node_toString(const name##Node *node);

D(Literal)
D(Identifier)
D(Html)
D(TpltPrint)
D(Operator)
D(Sequence)
D(Function)
D(Var)
D(Assignment)
D(Call)

#undef D



Node *wsky_ASTNode_copy(const Node *source) {

# define R(T)                                           \
  {                                                     \
    T##Node *new = malloc(sizeof(T##Node));             \
    new->position = source->position;                   \
    new->type = source->type;                           \
    T##Node_copy((const T##Node *) source, new);        \
    return (Node *)new;                                 \
  }

# define CASE(type, name) case wsky_ASTNodeType_##type: R(name)

  switch (source->type) {

  case wsky_ASTNodeType_NULL:
  case wsky_ASTNodeType_BOOL:
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    R(Literal);

    CASE(IDENTIFIER, Identifier);
    CASE(HTML, Html);
    CASE(TPLT_PRINT, TpltPrint);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    R(Operator);

    CASE(SEQUENCE, Sequence);
    CASE(FUNCTION, Function);
    CASE(VAR, Var);
    CASE(ASSIGNMENT, Assignment);
    CASE(CALL, Call);

  default:
    return NULL;
  }

# undef CASE
# undef R
}



bool wsky_ASTNode_isAssignable(const Node *node) {
  return node->type == wsky_ASTNodeType_IDENTIFIER;
}



char *wsky_ASTNode_toString(const Node *node) {

# define R(T)                                           \
  return T##Node_toString((const T##Node *) node);
# define CASE(type, name) case wsky_ASTNodeType_##type: R(name)

  switch (node->type) {

  case wsky_ASTNodeType_NULL:
  case wsky_ASTNodeType_BOOL:
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    R(Literal)

    CASE(IDENTIFIER, Identifier);
    CASE(HTML, Html);
    CASE(TPLT_PRINT, TpltPrint);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    R(Operator);

    CASE(SEQUENCE, Sequence);
    CASE(FUNCTION, Function);
    CASE(VAR, Var);
    CASE(ASSIGNMENT, Assignment);
    CASE(CALL, Call);

  default:
    return wsky_STRDUP("Unknown node");
  }

# undef CASE
# undef R
}



void wsky_ASTNode_print(const Node *node, FILE *output) {
  char *s = wsky_ASTNode_toString(node);
  fprintf(output, "%s", s);
  wsky_FREE(s);
}



void wsky_ASTNode_delete(Node *node) {

# define R(T)                                   \
  T##Node_free((T##Node *) node); break;
# define CASE(type, name) case wsky_ASTNodeType_##type: R(name)

  switch (node->type) {

  case wsky_ASTNodeType_NULL:
  case wsky_ASTNodeType_BOOL:
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    R(Literal)

    CASE(IDENTIFIER, Identifier);
    CASE(HTML, Html);
    CASE(TPLT_PRINT, TpltPrint);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    R(Operator);

    CASE(SEQUENCE, Sequence);
    CASE(FUNCTION, Function);
    CASE(VAR, Var);
    CASE(ASSIGNMENT, Assignment);
    CASE(CALL, Call);

  default:
    abort();
  }
  wsky_FREE(node);

# undef CASE
# undef R
}



LiteralNode *wsky_LiteralNode_new(const Token *token) {
  if (!wsky_Token_isLiteral(token))
    return NULL;

  LiteralNode *node = wsky_MALLOC(sizeof(LiteralNode));
  if (!node)
    return NULL;

  node->position = token->begin;

  if (token->type == wsky_TokenType_KEYWORD) {
    wsky_Keyword keyword = token->v.keyword;

    if (keyword == wsky_Keyword_TRUE) {
      node->type = wsky_ASTNodeType_BOOL;
      node->v.boolValue = true;
    } else if (keyword == wsky_Keyword_FALSE) {
      node->type = wsky_ASTNodeType_BOOL;
      node->v.boolValue = false;
    } else if (keyword == wsky_Keyword_NULL) {
      node->type = wsky_ASTNodeType_NULL;
    } else {
      wsky_FREE(node);
      return NULL;
    }

  } else if (token->type == wsky_TokenType_INT) {
    node->type = wsky_ASTNodeType_INT;
    node->v.intValue = token->v.intValue;

  } else if (token->type == wsky_TokenType_FLOAT) {
    node->type = wsky_ASTNodeType_FLOAT;
    node->v.floatValue = token->v.floatValue;

  } else if (token->type == wsky_TokenType_STRING) {
    node->type = wsky_ASTNodeType_STRING;
    node->v.stringValue = wsky_STRDUP(token->v.stringValue);

  } else {
    wsky_FREE(node);
    return NULL;
  }

  return node;
}

void LiteralNode_copy(const LiteralNode *source, LiteralNode *new) {
  if (source->type == wsky_ASTNodeType_STRING) {
    new->v.stringValue = strdup(source->v.stringValue);
  } else {
    new->v = source->v;
  }
}

static void LiteralNode_free(LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    wsky_FREE(node->v.stringValue);
  }
}

static char *stringNodeToString(const LiteralNode *node) {
  return wsky_String_escapeCString(node->v.stringValue);
}

static char *intNodeToString(const LiteralNode *node) {
  char buffer[64];
  snprintf(buffer, 64, "%ld", (long) node->v.intValue);
  return wsky_STRDUP(buffer);
}

static char *floatNodeToString(const LiteralNode *node) {
  char buffer[64];
  snprintf(buffer, 64, "%g", node->v.floatValue);
  return wsky_STRDUP(buffer);
}

static char *LiteralNode_toString(const LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    return stringNodeToString(node);
  } else if (node->type == wsky_ASTNodeType_INT) {
    return intNodeToString(node);
  } else if (node->type == wsky_ASTNodeType_FLOAT) {
    return floatNodeToString(node);
  }
  return wsky_STRDUP("LiteralNode");
}



IdentifierNode *wsky_IdentifierNode_new(const Token *token) {
  if (token->type != wsky_TokenType_IDENTIFIER)
    return NULL;

  IdentifierNode *node = wsky_MALLOC(sizeof(IdentifierNode));
  node->type = wsky_ASTNodeType_IDENTIFIER;
  node->position = token->begin;
  node->name = wsky_STRDUP(token->string);
  return (node);
}

void IdentifierNode_copy(const IdentifierNode *source, IdentifierNode *new) {
  new->name = strdup(source->name);
}

static void IdentifierNode_free(IdentifierNode *node) {
  wsky_FREE(node->name);
}

static char *IdentifierNode_toString(const IdentifierNode *node) {
  return wsky_STRDUP(node->name);
}



HtmlNode *wsky_HtmlNode_new(const Token *token) {
  if (token->type != wsky_TokenType_HTML)
    return NULL;

  HtmlNode *node = wsky_MALLOC(sizeof(HtmlNode));
  node->type = wsky_ASTNodeType_HTML;
  node->position = token->begin;
  node->content = wsky_STRDUP(token->string);
  return (node);
}

void HtmlNode_copy(const HtmlNode *source, HtmlNode *new) {
  new->content = strdup(source->content);
}

static void HtmlNode_free(HtmlNode *node) {
  wsky_FREE(node->content);
}

static char *HtmlNode_toString(const HtmlNode *node) {
  char *s = wsky_MALLOC(strlen(node->content) + 20);
  sprintf(s, "HTML(%s)", node->content);
  return s;
}



TpltPrintNode *wsky_TpltPrintNode_new(const Token *token,
                                      Node *child) {
  if (token->type != wsky_TokenType_WSKY_PRINT)
    return NULL;

  TpltPrintNode *node = wsky_MALLOC(sizeof(TpltPrintNode));
  node->type = wsky_ASTNodeType_TPLT_PRINT;
  node->position = token->begin;
  node->child = child;
  return (node);
}

void TpltPrintNode_copy(const TpltPrintNode *source, TpltPrintNode *new) {
  new->child = wsky_ASTNode_copy(source->child);
}

static void TpltPrintNode_free(TpltPrintNode *node) {
  wsky_ASTNode_delete(node->child);
}

static char *TpltPrintNode_toString(const TpltPrintNode *node) {
  char *childString =  wsky_ASTNode_toString(node->child);
  char *s = wsky_MALLOC(strlen(childString) + 20);
  sprintf(s, "TPLT_PRINT(%s)", childString);
  wsky_FREE(childString);
  return s;
}



OperatorNode *wsky_OperatorNode_new(const Token *token,
                                         Node *left,
                                         wsky_Operator operator,
                                         Node *right) {
  if (!left || !right || token->type != wsky_TokenType_OPERATOR)
    return NULL;

  OperatorNode *node = wsky_MALLOC(sizeof(OperatorNode));
  node->type = wsky_ASTNodeType_BINARY_OPERATOR;
  node->position = token->begin;
  node->left = left;
  node->operator = operator;
  node->right = right;
  return node;
}

OperatorNode *wsky_OperatorNode_newUnary(const Token *token,
                                              wsky_Operator operator,
                                              Node *right) {
  if (!right || token->type != wsky_TokenType_OPERATOR)
    return NULL;

  OperatorNode *node = wsky_MALLOC(sizeof(OperatorNode));
  node->type = wsky_ASTNodeType_UNARY_OPERATOR;
  node->position = token->begin;
  node->left = NULL;
  node->operator = operator;
  node->right = right;
  return node;
}

void OperatorNode_copy(const OperatorNode *source, OperatorNode *new) {
  new->operator = source->operator;
  new->right = wsky_ASTNode_copy(source->right);
  if (new->type == wsky_ASTNodeType_BINARY_OPERATOR) {
    new->left = wsky_ASTNode_copy(source->left);
  } else {
    new->left = NULL;
  }
}

static void OperatorNode_free(OperatorNode *node) {
  if (node->left)
    wsky_ASTNode_delete(node->left);
  wsky_ASTNode_delete(node->right);
}

static char *OperatorNode_toString(const OperatorNode *node) {
  char *left = NULL;
  if (node->left)
    left = wsky_ASTNode_toString(node->left);
  char *right = wsky_ASTNode_toString(node->right);

  const char *opString =  wsky_Operator_toString(node->operator);
  size_t length = strlen(opString) + 1 + strlen(right);
  if (left)
    length += strlen(left) + 1;
  length += 3;
  char *s = wsky_MALLOC(length);
  if (left) {
    snprintf(s, length, "(%s %s %s)", left, opString, right);
    wsky_FREE(left);
  } else {
    snprintf(s, length, "(%s%s)", opString, right);
  }
  wsky_FREE(right);
  return s;
}



NodeList *wsky_ASTNodeList_new(Node *node,
                               NodeList *next) {

  NodeList *list = wsky_MALLOC(sizeof(NodeList));
  list->node = node;
  list->next = next;
  return list;
}

NodeList *wsky_ASTNodeList_copy(const NodeList *source) {
  if (!source)
    return NULL;
  Node *newNode = wsky_ASTNode_copy(source->node);
  NodeList *next = wsky_ASTNodeList_copy(source->next);
  NodeList *new = wsky_ASTNodeList_new(newNode, next);
  return new;
}

NodeList *wsky_ASTNodeList_getLast(NodeList *list) {
  if (!list) {
    return NULL;
  }
  if (!list->next) {
    return list;
  }
  return wsky_ASTNodeList_getLast(list->next);
}

Node *wsky_ASTNodeList_getLastNode(NodeList *list) {
  NodeList *last = wsky_ASTNodeList_getLast(list);
  if (!last)
    return NULL;
  return last->node;
}

void wsky_ASTNodeList_add(NodeList **listPointer,
                          NodeList *new) {
  if (!*listPointer) {
    *listPointer = new;
  } else {
    NodeList *last = wsky_ASTNodeList_getLast(*listPointer);
    last->next = new;
  }
}

void wsky_ASTNodeList_addNode(NodeList **listPointer,
                              Node *node) {
  NodeList *new = wsky_ASTNodeList_new(node, NULL);
  wsky_ASTNodeList_add(listPointer, new);
}

void wsky_ASTNodeList_delete(NodeList *list) {
  if (!list)
    return;
  wsky_ASTNodeList_delete(list->next);
  wsky_ASTNode_delete(list->node);
  wsky_FREE(list);
}

char *wsky_ASTNodeList_toString(NodeList *list, const char *separator) {
  char *s = NULL;
  size_t length = 0;
  while (list) {
    char *nodeString = wsky_ASTNode_toString(list->node);
    s = wsky_REALLOC(s, length + strlen(nodeString) + 4);
    s[length] = '\0';
    strcat(s, nodeString);
    if (list->next)
      strcat(s, separator);
    wsky_FREE(nodeString);
    length = strlen(s);
    list = list->next;
  }
  if (!s)
    s = wsky_STRDUP("");
  return s;
}



SequenceNode *wsky_SequenceNode_new(const Token *token,
                                    NodeList *children) {
  SequenceNode *node = wsky_MALLOC(sizeof(SequenceNode));
  node->type = wsky_ASTNodeType_SEQUENCE;
  node->children = children;
  node->position = token->begin;
  return node;
}

void SequenceNode_copy(const SequenceNode *source, SequenceNode *new) {
  new->children = wsky_ASTNodeList_copy(source->children);
}

static void SequenceNode_free(SequenceNode *node) {
  wsky_ASTNodeList_delete(node->children);
}

static char *SequenceNode_toString(const SequenceNode *node) {
  char *list =  wsky_ASTNodeList_toString(node->children, "; ");
  char *s = wsky_MALLOC(strlen(list) + 4);
  sprintf(s, "(%s)", list);
  wsky_FREE(list);
  return s;
}



FunctionNode *wsky_FunctionNode_new(const Token *token,
                                         NodeList *parameters,
                                         NodeList *children) {
  FunctionNode *node = wsky_MALLOC(sizeof(FunctionNode));
  node->type = wsky_ASTNodeType_FUNCTION;
  node->position = token->begin;
  node->children = children;
  node->parameters = parameters;
  return node;
}

void FunctionNode_copy(const FunctionNode *source, FunctionNode *new) {
  new->children = wsky_ASTNodeList_copy(source->children);
  new->parameters = wsky_ASTNodeList_copy(source->parameters);
}

static void FunctionNode_free(FunctionNode *node) {
  wsky_ASTNodeList_delete(node->children);
  wsky_ASTNodeList_delete(node->parameters);
}

static char *FunctionNode_toString(const FunctionNode *node) {
  char *childrenString =  wsky_ASTNodeList_toString(node->children, "; ");
  char *paramString =  wsky_ASTNodeList_toString(node->parameters, ", ");
  int hasParameters = node->parameters != NULL;
  char *s = wsky_MALLOC(strlen(childrenString) + strlen(paramString) + 10);
  if (hasParameters)
    sprintf(s, "{ %s : %s}", paramString, childrenString);
  else
    sprintf(s, "{%s}", childrenString);
  wsky_FREE(paramString);
  wsky_FREE(childrenString);
  return s;
}



VarNode *wsky_VarNode_new(const Token *token,
                          const char *name,
                          Node *right) {
  VarNode *node = wsky_MALLOC(sizeof(VarNode));
  node->type = wsky_ASTNodeType_VAR;
  node->position = token->begin;
  node->name = wsky_STRDUP(name);
  node->right = right;
  return node;
}

void VarNode_copy(const VarNode *source, VarNode *new) {
  new->name = wsky_STRDUP(source->name);
  if (source->right)
    new->right = wsky_ASTNode_copy(source->right);
}

static void VarNode_free(VarNode *node) {
  if (node->right)
    wsky_ASTNode_delete(node->right);
  wsky_FREE(node->name);
}

unsigned wsky_ASTNodeList_getCount(const NodeList *list) {
  if (!list) {
    return 0;
  }
  return wsky_ASTNodeList_getCount(list->next) + 1;
}

static char *VarNode_toString(const VarNode *node) {
  char *rightString = NULL;
  size_t length = strlen(node->name) + 10;
  if (node->right) {
    rightString = wsky_ASTNode_toString(node->right);
    length += strlen(rightString);
  }
  char *s = wsky_MALLOC(length);
  if (node->right) {
    sprintf(s, "var %s = %s", node->name, rightString);
    wsky_FREE(rightString);
  } else {
    sprintf(s, "var %s", node->name);
  }
  return s;
}



AssignmentNode *wsky_AssignmentNode_new(const Token *token,
                                        Node *left,
                                        Node *right) {
  assert(wsky_ASTNode_isAssignable(left));
  AssignmentNode *node = wsky_MALLOC(sizeof(AssignmentNode));
  node->type = wsky_ASTNodeType_ASSIGNMENT;
  node->position = token->begin;
  node->left = left;
  node->right = right;
  return node;
}

void AssignmentNode_copy(const AssignmentNode *source, AssignmentNode *new) {
  new->left = wsky_ASTNode_copy(source->left);
  new->right = wsky_ASTNode_copy(source->right);
}

static void AssignmentNode_free(AssignmentNode *node) {
  wsky_ASTNode_delete(node->left);
  wsky_ASTNode_delete(node->right);
}

static char *AssignmentNode_toString(const AssignmentNode *node) {
  char *leftString = wsky_ASTNode_toString(node->left);
  char *rightString = wsky_ASTNode_toString(node->right);
  size_t length = strlen(leftString) + 10 + strlen(rightString);
  char *s = wsky_MALLOC(length);
  sprintf(s, "(%s = %s)", leftString, rightString);
  wsky_FREE(leftString);
  wsky_FREE(rightString);
  return s;
}



CallNode *wsky_CallNode_new(const Token *token,
                            Node *left,
                            NodeList *children) {
  CallNode *node = wsky_MALLOC(sizeof(CallNode));
  node->type = wsky_ASTNodeType_CALL;
  node->position = token->begin;
  node->left = left;
  node->children = children;
  return node;
}

void CallNode_copy(const CallNode *source, CallNode *new) {
  new->left = wsky_ASTNode_copy(source->left);
  new->children = wsky_ASTNodeList_copy(source->children);
}

static void CallNode_free(wsky_CallNode *node) {
  wsky_ASTNodeList_delete(node->children);
  wsky_ASTNode_delete(node->left);
}

static char *CallNode_toString(const CallNode *node) {
  char *paramString =  wsky_ASTNodeList_toString(node->children, ", ");
  char *leftString =  wsky_ASTNode_toString(node->left);
  char *s = wsky_MALLOC(strlen(leftString) + strlen(paramString) + 10);
  sprintf(s, "%s(%s)", leftString, paramString);
  wsky_FREE(leftString);
  wsky_FREE(paramString);
  return s;
}
