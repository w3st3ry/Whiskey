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
  *listPointer = (*listPointer)->next;
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
  *listPointer = (*listPointer)->next;
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

static wsky_ParserResult parseMul(wsky_TokenList **listPointer) {
  return parseTerm(listPointer);
}

static wsky_ParserResult parseAdd(wsky_TokenList **listPointer) {
  wsky_ParserResult lr = parseMul(listPointer);
  if (!lr.success) {
    return lr;
  }
  wsky_ASTNode *left = lr.node;

  while (*listPointer) {
    wsky_Token *opToken = &(*listPointer)->token;
    if (opToken->type != wsky_TokenType_OPERATOR) {
      break;
    }

    wsky_Operator op = opToken->v.operator;
    printf("%d\n", op);
    if (op != wsky_Operator_PLUS && op != wsky_Operator_MINUS) {
      break;
    }

    wsky_TokenList *previous = *listPointer;
    *listPointer = (*listPointer)->next;
    wsky_ParserResult rr = parseMul(listPointer);
    if (!rr.success) {
      *listPointer = previous;
    }
    left = (wsky_ASTNode *) wsky_OperatorNode_new(opToken,
						  left, op, rr.node);
  }

  return NODE_RESULT(left);
}

static wsky_ParserResult parseExpr(wsky_TokenList **listPointer) {
  return parseAdd(listPointer);
}



wsky_ParserResult wsky_parse(wsky_TokenList *tokens) {
  return parseExpr(&tokens);
}
