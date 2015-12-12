#include "ast.h"

#include <stdlib.h>
#include <string.h>

char *wsky_ASTNode_toString(const wsky_ASTNode *node) {
  switch (node->type) {
  default:
    return strdup("Unknown node");
  }
}

void wsky_ASTNode_print(const wsky_ASTNode *node, FILE *output) {
  char *s = wsky_ASTNode_toString(node);
  fprintf(output, "%s", s);
  free(s);
}



wsky_LiteralNode *wsky_LiteralNode_new(wsky_Token *token) {
  if (!wsky_Token_isLiteral(token))
    return NULL;
  wsky_LiteralNode *node = malloc(sizeof(wsky_LiteralNode));
  if (!node)
    return NULL;

}
