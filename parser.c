#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include "lexer.h"


#define NODE_RESULT(node) ParserResult_createFromNode(node)

#define ERROR_RESULT(message, pos) createErrorResult(message, pos)

/*
 * Prefer ERROR_RESULT. Use this one only if there is no position.
 */
#define EOF_ERROR_RESULT(message) \
  createEofErrorResult(message)

#define UNEXPECTED_EOF_ERROR_RESULT() \
  EOF_ERROR_RESULT("Unexpected end of file")

#define UNEXPECTED_TOKEN_ERROR_RESULT(token) \
  createUnexpTokenErrorResult(token)



static wsky_ParserResult ParserResult_createFromError(wsky_SyntaxError e) {
  wsky_ParserResult r = {
    .success = 0,
    .node = NULL,
    .syntaxError = e,
  };
  return r;
}

static wsky_ParserResult ParserResult_createFromNode(wsky_ASTNode *n) {
  wsky_ParserResult r = {
    .success = 1,
    .node = n,
  };
  return r;
}

static wsky_ParserResult createErrorResult(const char *msg,
					   const wsky_Position pos) {
  wsky_SyntaxError e = wsky_SyntaxError_create(msg, pos);
  return  ParserResult_createFromError(e);
}

static wsky_ParserResult createEofErrorResult(const char *msg) {
  /*
   * We have no position, so let's create an invalid one.
   * It will be replaced by a valid one later.
   */
  wsky_Position position = {
    .file = NULL,
    .index = -1,
    .column = -1,
    .line = -1,
  };
  return createErrorResult(msg, position);
}

static wsky_ParserResult createUnexpTokenErrorResult(const wsky_Token *t) {
  char *message = malloc(strlen(t->string) + 20);
  sprintf(message, "Unexpected '%s'", t->string);
  wsky_ParserResult r = ERROR_RESULT(message, t->begin);
  free(message);
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

  return UNEXPECTED_TOKEN_ERROR_RESULT(&(*listPointer)->token);
}

static wsky_ParserResult parseUnary(wsky_TokenList **listPointer) {
  if (!*listPointer) {
    return UNEXPECTED_EOF_ERROR_RESULT();
  }

  wsky_Token *opToken = &(*listPointer)->token;
  if (opToken->type != wsky_TokenType_OPERATOR) {
    return parseTerm(listPointer);
  }

  wsky_Operator op = opToken->v.operator;
  if (op != wsky_Operator_MINUS && op != wsky_Operator_PLUS) {
    return parseTerm(listPointer);
  }

  *listPointer = (*listPointer)->next;
  wsky_ParserResult rr = parseUnary(listPointer);
  if (!rr.success) {
    return rr;
  }
  wsky_ASTNode *node;
  node = (wsky_ASTNode *) wsky_OperatorNode_newUnary(opToken, op, rr.node);
  return NODE_RESULT(node);
}

static wsky_ParserResult parseFactor(wsky_TokenList **listPointer) {
  return parseUnary(listPointer);
}

static wsky_ParserResult parseMul(wsky_TokenList **listPointer) {
  wsky_ParserResult lr = parseFactor(listPointer);
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
    if (op != wsky_Operator_STAR && op != wsky_Operator_SLASH) {
      break;
    }

    *listPointer = (*listPointer)->next;
    wsky_ParserResult rr = parseFactor(listPointer);
    if (!rr.success) {
      wsky_ASTNode_delete(left);
      return rr;
    }
    left = (wsky_ASTNode *) wsky_OperatorNode_new(opToken,
						  left, op, rr.node);
  }

  return NODE_RESULT(left);
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
    if (op != wsky_Operator_PLUS && op != wsky_Operator_MINUS) {
      break;
    }

    *listPointer = (*listPointer)->next;
    wsky_ParserResult rr = parseMul(listPointer);
    if (!rr.success) {
      wsky_ASTNode_delete(left);
      return rr;
    }
    left = (wsky_ASTNode *) wsky_OperatorNode_new(opToken,
						  left, op, rr.node);
  }

  return NODE_RESULT(left);
}

static wsky_ParserResult parseExpr(wsky_TokenList **listPointer) {
  if (!*listPointer) {
    return UNEXPECTED_EOF_ERROR_RESULT();
  }

  return parseAdd(listPointer);
}



wsky_ParserResult wsky_parse(wsky_TokenList *tokens) {
  wsky_TokenList *begin = tokens;

  wsky_ParserResult r = parseExpr(&tokens);
  if (!r.success) {
    wsky_SyntaxError *e = &r.syntaxError;
    if (e->position.index == -1 && begin) {
      /* That's an "unexpected EOF error" */
      wsky_Token *lastToken = &wsky_TokenList_getLast(begin)->token;
      e->position = lastToken->end;
    }
  }
  return r;
}

wsky_ParserResult wsky_parseString(const char *string,
				   wsky_TokenList **listPointer) {
  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    return ParserResult_createFromError(lr.syntaxError);
  }

  wsky_ParserResult pr = wsky_parse(lr.tokens);
  if (!pr.success) {
    wsky_TokenList_delete(lr.tokens);
    return pr;
  }
  *listPointer = lr.tokens;
  return pr;
}
