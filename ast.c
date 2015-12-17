#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include "str.h"


typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;
typedef wsky_Token Token;



static void LiteralNode_free(wsky_LiteralNode *node);
static char *LiteralNode_toString(const wsky_LiteralNode *node);

static void IdentifierNode_free(wsky_IdentifierNode *node);
static char *IdentifierNode_toString(const wsky_IdentifierNode *node);

static void HtmlNode_free(wsky_HtmlNode *node);
static char *HtmlNode_toString(const wsky_HtmlNode *node);

static void TpltPrintNode_free(wsky_TpltPrintNode *node);
static char *TpltPrintNode_toString(const wsky_TpltPrintNode *node);

static void OperatorNode_free(wsky_OperatorNode *node);
static char *OperatorNode_toString(const wsky_OperatorNode *node);

static void SequenceNode_free(wsky_SequenceNode *node);
static char *SequenceNode_toString(const wsky_SequenceNode *node);

static void FunctionNode_free(wsky_FunctionNode *node);
static char *FunctionNode_toString(const wsky_FunctionNode *node);

static void VarNode_free(wsky_VarNode *node);
static char *VarNode_toString(const wsky_VarNode *node);

static void AssignmentNode_free(wsky_AssignmentNode *node);
static char *AssignmentNode_toString(const wsky_AssignmentNode *node);



bool wsky_ASTNode_isAssignable(const Node *node) {
  return node->type == wsky_ASTNodeType_IDENTIFIER;
}

char *wsky_ASTNode_toString(const Node *node) {
  switch (node->type) {
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    return LiteralNode_toString((const wsky_LiteralNode *) node);

  case wsky_ASTNodeType_IDENTIFIER:
    return IdentifierNode_toString((const wsky_IdentifierNode *) node);

  case wsky_ASTNodeType_HTML:
    return HtmlNode_toString((const wsky_HtmlNode *) node);

  case wsky_ASTNodeType_TPLT_PRINT:
    return TpltPrintNode_toString((const wsky_TpltPrintNode *) node);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    return OperatorNode_toString((const wsky_OperatorNode *) node);

  case wsky_ASTNodeType_SEQUENCE:
    return SequenceNode_toString((const wsky_SequenceNode *) node);

  case wsky_ASTNodeType_FUNCTION:
    return FunctionNode_toString((const wsky_FunctionNode *) node);

  case wsky_ASTNodeType_VAR:
    return VarNode_toString((const wsky_VarNode *) node);

  case wsky_ASTNodeType_ASSIGNMENT:
    return AssignmentNode_toString((wsky_AssignmentNode *) node);

  default:
    return strdup("Unknown node");
  }
}

void wsky_ASTNode_print(const Node *node, FILE *output) {
  char *s = wsky_ASTNode_toString(node);
  fprintf(output, "%s", s);
  free(s);
}

void wsky_ASTNode_delete(Node *node) {
  switch (node->type) {
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    LiteralNode_free((wsky_LiteralNode *) node);
    break;

  case wsky_ASTNodeType_IDENTIFIER:
    IdentifierNode_free((wsky_IdentifierNode *) node);
    break;

  case wsky_ASTNodeType_HTML:
    HtmlNode_free((wsky_HtmlNode *) node);
    break;

  case wsky_ASTNodeType_TPLT_PRINT:
    TpltPrintNode_free((wsky_TpltPrintNode *) node);
    break;

  case wsky_ASTNodeType_UNARY_OPERATOR:
  case wsky_ASTNodeType_BINARY_OPERATOR:
    OperatorNode_free((wsky_OperatorNode *) node);
    break;

  case wsky_ASTNodeType_SEQUENCE:
    SequenceNode_free((wsky_SequenceNode *) node);
    break;

  case wsky_ASTNodeType_FUNCTION:
    FunctionNode_free((wsky_FunctionNode *) node);
    break;

  case wsky_ASTNodeType_VAR:
    VarNode_free((wsky_VarNode *) node);
    break;

  case wsky_ASTNodeType_ASSIGNMENT:
    AssignmentNode_free((wsky_AssignmentNode *) node);
    break;

  default:
    abort();
  }
  free(node);
}



wsky_LiteralNode *wsky_LiteralNode_new(const wsky_Token *token) {
  if (!wsky_Token_isLiteral(token))
    return NULL;

  wsky_LiteralNode *node = malloc(sizeof(wsky_LiteralNode));
  if (!node)
    return NULL;

  node->token = *token;

  if (token->type == wsky_TokenType_INT) {
    node->type = wsky_ASTNodeType_INT;
    node->v.intValue = token->v.intValue;

  } else if (token->type == wsky_TokenType_FLOAT) {
    node->type = wsky_ASTNodeType_FLOAT;
    node->v.floatValue = token->v.floatValue;

  } else if (token->type == wsky_TokenType_STRING) {
    node->type = wsky_ASTNodeType_STRING;
    node->v.stringValue = strdup(token->v.stringValue);

  } else {
    free(node);
    return NULL;
  }

  return node;
}

static void LiteralNode_free(wsky_LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    free(node->v.stringValue);
  }
}

static char *stringNodeToString(const wsky_LiteralNode *node) {
  return wsky_String_escapeCString(node->v.stringValue);
}

static char *intNodeToString(const wsky_LiteralNode *node) {
  char buffer[64];
  snprintf(buffer, 64, "%ld", (long) node->v.intValue);
  return strdup(buffer);
}

static char *floatNodeToString(const wsky_LiteralNode *node) {
  char buffer[64];
  snprintf(buffer, 64, "%g", node->v.floatValue);
  return strdup(buffer);
}

static char *LiteralNode_toString(const wsky_LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    return stringNodeToString(node);
  } else if (node->type == wsky_ASTNodeType_INT) {
    return intNodeToString(node);
  } else if (node->type == wsky_ASTNodeType_FLOAT) {
    return floatNodeToString(node);
  }
  return strdup("LiteralNode");
}



wsky_IdentifierNode *wsky_IdentifierNode_new(const wsky_Token *token) {
  if (token->type != wsky_TokenType_IDENTIFIER)
    return NULL;

  wsky_IdentifierNode *node = malloc(sizeof(wsky_IdentifierNode));
  node->type = wsky_ASTNodeType_IDENTIFIER;
  node->token = *token;
  node->name = strdup(token->string);
  return (node);
}

static void IdentifierNode_free(wsky_IdentifierNode *node) {
  free(node->name);
}

static char *IdentifierNode_toString(const wsky_IdentifierNode *node) {
  return strdup(node->token.string);
}



wsky_HtmlNode *wsky_HtmlNode_new(const wsky_Token *token) {
  if (token->type != wsky_TokenType_HTML)
    return NULL;

  wsky_HtmlNode *node = malloc(sizeof(wsky_HtmlNode));
  node->type = wsky_ASTNodeType_HTML;
  node->token = *token;
  node->content = strdup(token->string);
  return (node);
}

static void HtmlNode_free(wsky_HtmlNode *node) {
  free(node->content);
}

static char *HtmlNode_toString(const wsky_HtmlNode *node) {
  char *s = malloc(strlen(node->content) + 20);
  sprintf(s, "HTML(%s)", node->content);
  return s;
}



wsky_TpltPrintNode *wsky_TpltPrintNode_new(const wsky_Token *token,
                                           Node *child) {
  if (token->type != wsky_TokenType_WSKY_PRINT)
    return NULL;

  wsky_TpltPrintNode *node = malloc(sizeof(wsky_TpltPrintNode));
  node->type = wsky_ASTNodeType_TPLT_PRINT;
  node->token = *token;
  node->child = child;
  return (node);
}

static void TpltPrintNode_free(wsky_TpltPrintNode *node) {
  wsky_ASTNode_delete(node->child);
}

static char *TpltPrintNode_toString(const wsky_TpltPrintNode *node) {
  char *childString =  wsky_ASTNode_toString(node->child);
  char *s = malloc(strlen(childString) + 20);
  sprintf(s, "TPLT_PRINT(%s)", childString);
  free(childString);
  return s;
}



wsky_OperatorNode *wsky_OperatorNode_new(const wsky_Token *token,
                                         Node *left,
                                         wsky_Operator operator,
                                         Node *right) {
  if (!left || !right || token->type != wsky_TokenType_OPERATOR)
    return NULL;

  wsky_OperatorNode *node = malloc(sizeof(wsky_OperatorNode));
  node->type = wsky_ASTNodeType_BINARY_OPERATOR;
  node->token = *token;
  node->left = left;
  node->operator = operator;
  node->right = right;
  return node;
}

wsky_OperatorNode *wsky_OperatorNode_newUnary(const wsky_Token *token,
                                              wsky_Operator operator,
                                              Node *right) {
  if (!right || token->type != wsky_TokenType_OPERATOR)
    return NULL;

  wsky_OperatorNode *node = malloc(sizeof(wsky_OperatorNode));
  node->type = wsky_ASTNodeType_UNARY_OPERATOR;
  node->token = *token;
  node->left = NULL;
  node->operator = operator;
  node->right = right;
  return node;
}

static void OperatorNode_free(wsky_OperatorNode *node) {
  if (node->left)
    wsky_ASTNode_delete(node->left);
  wsky_ASTNode_delete(node->right);
}

static char *OperatorNode_toString(const wsky_OperatorNode *node) {
  char *left = NULL;
  if (node->left)
    left = wsky_ASTNode_toString(node->left);
  char *right = wsky_ASTNode_toString(node->right);

  size_t length = strlen(node->token.string) + 1 + strlen(right);
  if (left)
    length += strlen(left) + 1;
  length += 3;
  char *s = malloc(length);
  if (left) {
    snprintf(s, length, "(%s %s %s)", left, node->token.string, right);
    free(left);
  } else {
    snprintf(s, length, "(%s%s)", node->token.string, right);
  }
  free(right);
  return s;
}



NodeList *wsky_ASTNodeList_new(Node *node,
                               NodeList *next) {

  NodeList *list = malloc(sizeof(NodeList));
  list->node = node;
  list->next = next;
  return list;
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
  free(list);
}

char *wsky_ASTNodeList_toString(NodeList *list, const char *separator) {
  char *s = NULL;
  size_t length = 0;
  while (list) {
    char *nodeString = wsky_ASTNode_toString(list->node);
    s = realloc(s, length + strlen(nodeString) + 4);
    s[length] = '\0';
    strcat(s, nodeString);
    if (list->next)
      strcat(s, separator);
    free(nodeString);
    length = strlen(s);
    list = list->next;
  }
  if (!s)
    s = strdup("");
  return s;
}



wsky_SequenceNode *wsky_SequenceNode_new(const Token *token,
                                         NodeList *children) {
  wsky_SequenceNode *node = malloc(sizeof(wsky_SequenceNode));
  node->type = wsky_ASTNodeType_SEQUENCE;
  node->children = children;
  node->token = *token;
  return node;
}

static void SequenceNode_free(wsky_SequenceNode *node) {
  wsky_ASTNodeList_delete(node->children);
}

static char *SequenceNode_toString(const wsky_SequenceNode *node) {
  char *list =  wsky_ASTNodeList_toString(node->children, "; ");
  char *s = malloc(strlen(list) + 4);
  sprintf(s, "(%s)", list);
  free(list);
  return s;
}



wsky_FunctionNode *wsky_FunctionNode_new(const Token *token,
                                         NodeList *parameters,
                                         NodeList *children) {
  wsky_FunctionNode *node = malloc(sizeof(wsky_FunctionNode));
  node->type = wsky_ASTNodeType_FUNCTION;
  node->token = *token;
  node->children = children;
  node->parameters = parameters;
  return node;
}

static void FunctionNode_free(wsky_FunctionNode *node) {
  wsky_ASTNodeList_delete(node->children);
  wsky_ASTNodeList_delete(node->parameters);
}

static char *FunctionNode_toString(const wsky_FunctionNode *node) {
  char *childrenString =  wsky_ASTNodeList_toString(node->children, "; ");
  char *paramString =  wsky_ASTNodeList_toString(node->parameters, ", ");
  int hasParameters = node->parameters != NULL;
  char *s = malloc(strlen(childrenString) + strlen(paramString) + 10);
  if (hasParameters)
    sprintf(s, "{ %s : %s}", paramString, childrenString);
  else
    sprintf(s, "{%s}", childrenString);
  free(paramString);
  free(childrenString);
  return s;
}



wsky_VarNode *wsky_VarNode_new(const wsky_Token *token,
                               const char *name,
                               wsky_ASTNode *right) {
  wsky_VarNode *node = malloc(sizeof(wsky_VarNode));
  node->type = wsky_ASTNodeType_VAR;
  node->token = *token;
  node->name = strdup(name);
  node->right = right;
  return node;
}

static void VarNode_free(wsky_VarNode *node) {
  if (node->right)
    wsky_ASTNode_delete(node->right);
  free(node->name);
}

unsigned wsky_ASTNodeList_getCount(const wsky_ASTNodeList *list) {
  if (!list) {
    return 0;
  }
  return wsky_ASTNodeList_getCount(list->next) + 1;
}

static char *VarNode_toString(const wsky_VarNode *node) {
  char *rightString = NULL;
  size_t length = strlen(node->name) + 10;
  if (node->right) {
    rightString = wsky_ASTNode_toString(node->right);
    length += strlen(rightString);
  }
  char *s = malloc(length);
  if (node->right) {
    sprintf(s, "var %s = %s", node->name, rightString);
    free(rightString);
  } else {
    sprintf(s, "var %s", node->name);
  }
  return s;
}



wsky_AssignmentNode *wsky_AssignmentNode_new(const wsky_Token *token,
                                             wsky_ASTNode *left,
                                             wsky_ASTNode *right) {
  if (!wsky_ASTNode_isAssignable(left)) {
    abort();
  }
  wsky_AssignmentNode *node = malloc(sizeof(wsky_AssignmentNode));
  node->type = wsky_ASTNodeType_ASSIGNMENT;
  node->token = *token;
  node->left = left;
  node->right = right;
  return node;
}

static void AssignmentNode_free(wsky_AssignmentNode *node) {
  wsky_ASTNode_delete(node->left);
  wsky_ASTNode_delete(node->right);
}

static char *AssignmentNode_toString(const wsky_AssignmentNode *node) {
  char *leftString = wsky_ASTNode_toString(node->left);
  char *rightString = wsky_ASTNode_toString(node->right);
  size_t length = strlen(leftString) + 10 + strlen(rightString);
  char *s = malloc(length);
  sprintf(s, "%s = %s", leftString, rightString);
  return s;
}
