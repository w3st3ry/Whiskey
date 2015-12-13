#include "ast.h"

#include <stdlib.h>
#include <string.h>



typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;



static void LiteralNode_free(wsky_LiteralNode *node);
static char *LiteralNode_toString(const wsky_LiteralNode *node);

static void IdentifierNode_free(wsky_IdentifierNode *node);
static char *IdentifierNode_toString(const wsky_IdentifierNode *node);

static void OperatorNode_free(wsky_OperatorNode *node);
static char *OperatorNode_toString(const wsky_OperatorNode *node);

static void ListNode_free(wsky_ListNode *node);
static char *ListNode_toString(const wsky_ListNode *node);



char *wsky_ASTNode_toString(const Node *node) {
  switch (node->type) {
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    return LiteralNode_toString((wsky_LiteralNode *) node);

  case wsky_ASTNodeType_IDENTIFIER:
    return IdentifierNode_toString((wsky_IdentifierNode *) node);

  case wsky_ASTNodeType_BINARY_OPERATOR:
  case wsky_ASTNodeType_UNARY_OPERATOR:
    return OperatorNode_toString((wsky_OperatorNode *) node);

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

  case wsky_ASTNodeType_UNARY_OPERATOR:
  case wsky_ASTNodeType_BINARY_OPERATOR:
    OperatorNode_free((wsky_OperatorNode *) node);
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

static void escapeChar(char *dest, char source) {
  /* TODO: Improve a lot, it’s crappy */

  switch (source) {
  case '\n':
    strcat(dest, "\\n");
    return;
  case '\r':
    strcat(dest, "\\r");
    return;
  case '\t':
    strcat(dest, "\\t");
    return;
  case '\0':
    strcat(dest, "\\0");
    return;
  case '\'':
    strcat(dest, "\\'");
    return;
  case '\"':
    strcat(dest, "\\\"");
    return;
  }

  size_t length = strlen(dest);
  dest[length] = source;
  dest[length + 1] = '\0';
}

static char *escapeString(const char *source) {
  size_t max_length = strlen(source) * 2 + 2;
  char *s = malloc(max_length + 1);
  s[0] = '\'';
  s[1] = '\0';
  while (*source) {
    escapeChar(s, *source);
    source++;
  }
  size_t length = strlen(s);
  s[length] = '\'';
  s[length + 1] = '\0';
  return (s);
}

static char *stringNodeToString(const wsky_LiteralNode *node) {
  return escapeString(node->v.stringValue);
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



static void ListNode_free(wsky_ListNode *node) {

}

static char *ListNode_toString(const wsky_ListNode *node) {
  return strdup("ListNode");
}
