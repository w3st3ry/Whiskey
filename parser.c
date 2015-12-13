#include "parser.h"

#include <stdlib.h>


#define NODE_RESULT(node) wsky_ParserResult_createFromNode(node)

wsky_ParserResult wsky_ParserResult_createFromNode(wsky_ASTNode *n) {
  wsky_ParserResult r = {
    .success = 1,
    .node = n,
  };
  return r;
}

static const wsky_ParserResult ParserResult_NULL = {
  .success = 1,
  .node = NULL,
};



/* Returns a literal (string, int or float) or NULL */
static wsky_ParserResult parseLiteral(wsky_TokenList **listPointer) {
  if (!*listPointer) {
    return ParserResult_NULL;
  }

  wsky_Token *token = &(*listPointer)->token;
  if (!wsky_Token_isLiteral(token)) {
    return ParserResult_NULL;
  }
  wsky_ASTNode *node = (wsky_ASTNode *) wsky_LiteralNode_new(token);
  if (!node) {
    abort();
  }
  return NODE_RESULT(node);
}

/* Returns an identifier or NULL */
static wsky_ParserResult parseIdentifier(wsky_TokenList **listPointer) {
  if (!*listPointer) {
    return ParserResult_NULL;
  }

  wsky_Token *token = &(*listPointer)->token;
  if (token->type != wsky_TokenType_IDENTIFIER) {
    return ParserResult_NULL;
  }
  wsky_ASTNode *node = (wsky_ASTNode *) wsky_IdentifierNode_new(token);
  if (!node) {
    abort();
  }
  return NODE_RESULT(node);
}

static wsky_ParserResult parseTerm(wsky_TokenList **listPointer) {
  wsky_ParserResult result;

  result = parseIdentifier(listPointer);
  if (result.node)
    return result;
  result = parseLiteral(listPointer);
  if (result.node)
    return result;

  return ParserResult_NULL;
}



wsky_ParserResult wsky_parse(wsky_TokenList *tokens) {
  return parseTerm(&tokens);
}
