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

static char *LiteralNode_toString(const wsky_LiteralNode *node) {
  if (node->type == wsky_ASTNodeType_STRING) {
    return stringNodeToString(node);
  }
  return strdup("LiteralNode");
}
