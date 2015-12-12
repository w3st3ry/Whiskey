#include "parser.h"



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



static wsky_ParserResult parseLiteral(wsky_TokenList **list_pointer) {
  if (!*list_pointer)
    return ParserResult_NULL;
  wsky_Token *token = &(*list_pointer)->token;
  wsky_ASTNode *node = (wsky_ASTNode *) wsky_LiteralNode_new(token);
  if (!node) {
    return ParserResult_NULL;
  }
  return NODE_RESULT(node);
}



wsky_ParserResult wsky_parse(wsky_TokenList *tokens) {
  return parseLiteral(&tokens);
}
