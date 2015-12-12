#include "ast.h"

#include <stdlib.h>
#include <string.h>



static void LiteralNode_delete(wsky_LiteralNode *node);
static char *LiteralNode_toString(const wsky_LiteralNode *node);

static void OperatorNode_delete(wsky_OperatorNode *node);

static void ListNode_delete(wsky_ListNode *node);



char *wsky_ASTNode_toString(const wsky_ASTNode *node) {
  switch (node->type) {
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    return LiteralNode_toString((wsky_LiteralNode *) node);

  default:
    return strdup("Unknown node");
  }
}

void wsky_ASTNode_print(const wsky_ASTNode *node, FILE *output) {
  char *s = wsky_ASTNode_toString(node);
  fprintf(output, "%s", s);
  free(s);
}

void wsky_ASTNode_delete(wsky_ASTNode *node) {
  switch (node->type) {
  case wsky_ASTNodeType_INT:
  case wsky_ASTNodeType_FLOAT:
  case wsky_ASTNodeType_STRING:
    LiteralNode_delete((wsky_LiteralNode *) node);
    break;

  default:
    abort();
  }
  free(node);
}



wsky_LiteralNode *wsky_LiteralNode_new(wsky_Token *token) {
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

static void LiteralNode_delete(wsky_LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    free(node->v.stringValue);
  }
}

static char *LiteralNode_toString(const wsky_LiteralNode *node) {
  return strdup("LiteralNode");
}
