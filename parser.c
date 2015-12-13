#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include "lexer.h"



typedef wsky_Token Token;
typedef wsky_TokenList TokenList;

typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;

typedef wsky_ParserResult ParserResult;

#define NODE_RESULT(node) ParserResult_createFromNode(node)

#define ERROR_RESULT(message, pos) createErrorResult(message, pos)

/*
 * Prefer ERROR_RESULT. Use this one only if there is no position.
 */
#define EOF_ERROR_RESULT(message)		\
  createEofErrorResult(message)

#define UNEXPECTED_EOF_ERROR_RESULT()		\
  EOF_ERROR_RESULT("Unexpected end of file")

#define UNEXPECTED_TOKEN_ERROR_RESULT(token)	\
  createUnexpectedTokenErrorResult(token)



static ParserResult ParserResult_createFromError(wsky_SyntaxError e) {
  ParserResult r = {
    .success = 0,
    .node = NULL,
    .syntaxError = e,
  };
  return r;
}

static ParserResult ParserResult_createFromNode(Node *n) {
  ParserResult r = {
    .success = 1,
    .node = n,
  };
  return r;
}

static ParserResult createErrorResult(const char *msg,
				      const wsky_Position pos) {
  wsky_SyntaxError e = wsky_SyntaxError_create(msg, pos);
  return  ParserResult_createFromError(e);
}

static ParserResult createEofErrorResult(const char *msg) {
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

static ParserResult createUnexpectedTokenErrorResult(const Token *t) {
  char *message = malloc(strlen(t->string) + 20);
  sprintf(message, "Unexpected '%s'", t->string);
  ParserResult r = ERROR_RESULT(message, t->begin);
  free(message);
  return r;
}

static const ParserResult ParserResult_NULL = {
  .success = 1,
  .node = NULL,
};



/* Returns a literal (string, int or float) or NULL */
static ParserResult parseLiteral(TokenList **listPointer) {
  if (!*listPointer) {
    return ParserResult_NULL;
  }

  Token *token = &(*listPointer)->token;
  if (!wsky_Token_isLiteral(token)) {
    return ParserResult_NULL;
  }
  Node *node = (Node *) wsky_LiteralNode_new(token);
  if (!node) {
    abort();
  }
  *listPointer = (*listPointer)->next;
  return NODE_RESULT(node);
}

/* Returns an identifier or NULL */
static ParserResult parseIdentifier(TokenList **listPointer) {
  if (!*listPointer) {
    return ParserResult_NULL;
  }

  Token *token = &(*listPointer)->token;
  if (token->type != wsky_TokenType_IDENTIFIER) {
    return ParserResult_NULL;
  }
  Node *node = (Node *) wsky_IdentifierNode_new(token);
  if (!node) {
    abort();
  }
  *listPointer = (*listPointer)->next;
  return NODE_RESULT(node);
}

static ParserResult parseTerm(TokenList **listPointer) {
  ParserResult result;

  result = parseIdentifier(listPointer);
  if (result.node)
    return result;
  result = parseLiteral(listPointer);
  if (result.node)
    return result;

  return UNEXPECTED_TOKEN_ERROR_RESULT(&(*listPointer)->token);
}

static ParserResult parseUnary(TokenList **listPointer) {
  if (!*listPointer) {
    return UNEXPECTED_EOF_ERROR_RESULT();
  }

  Token *opToken = &(*listPointer)->token;
  if (opToken->type != wsky_TokenType_OPERATOR) {
    return parseTerm(listPointer);
  }

  wsky_Operator op = opToken->v.operator;
  if (op != wsky_Operator_MINUS && op != wsky_Operator_PLUS) {
    return parseTerm(listPointer);
  }

  *listPointer = (*listPointer)->next;
  ParserResult rr = parseUnary(listPointer);
  if (!rr.success) {
    return rr;
  }
  Node *node;
  node = (Node *) wsky_OperatorNode_newUnary(opToken, op, rr.node);
  return NODE_RESULT(node);
}

static ParserResult parseFactor(TokenList **listPointer) {
  return parseUnary(listPointer);
}

static ParserResult parseMul(TokenList **listPointer) {
  ParserResult lr = parseFactor(listPointer);
  if (!lr.success) {
    return lr;
  }
  Node *left = lr.node;

  while (*listPointer) {
    Token *opToken = &(*listPointer)->token;
    if (opToken->type != wsky_TokenType_OPERATOR) {
      break;
    }

    wsky_Operator op = opToken->v.operator;
    if (op != wsky_Operator_STAR && op != wsky_Operator_SLASH) {
      break;
    }

    *listPointer = (*listPointer)->next;
    ParserResult rr = parseFactor(listPointer);
    if (!rr.success) {
      wsky_ASTNode_delete(left);
      return rr;
    }
    left = (Node *) wsky_OperatorNode_new(opToken,
					  left, op, rr.node);
  }

  return NODE_RESULT(left);
}

static ParserResult parseAdd(TokenList **listPointer) {
  ParserResult lr = parseMul(listPointer);
  if (!lr.success) {
    return lr;
  }
  Node *left = lr.node;

  while (*listPointer) {
    Token *opToken = &(*listPointer)->token;
    if (opToken->type != wsky_TokenType_OPERATOR) {
      break;
    }

    wsky_Operator op = opToken->v.operator;
    if (op != wsky_Operator_PLUS && op != wsky_Operator_MINUS) {
      break;
    }

    *listPointer = (*listPointer)->next;
    ParserResult rr = parseMul(listPointer);
    if (!rr.success) {
      wsky_ASTNode_delete(left);
      return rr;
    }
    left = (Node *) wsky_OperatorNode_new(opToken,
					  left, op, rr.node);
  }

  return NODE_RESULT(left);
}

static ParserResult parseExpr(TokenList **listPointer) {
  if (!*listPointer) {
    return UNEXPECTED_EOF_ERROR_RESULT();
  }

  return parseAdd(listPointer);
}



ParserResult wsky_parse(TokenList *tokens) {
  TokenList *begin = tokens;

  ParserResult r = parseExpr(&tokens);
  if (!r.success) {
    wsky_SyntaxError *e = &r.syntaxError;
    if (e->position.index == -1 && begin) {
      /* That's an "unexpected EOF error" */
      Token *lastToken = &wsky_TokenList_getLast(begin)->token;
      e->position = lastToken->end;
    }
  }
  return r;
}

ParserResult wsky_parseString(const char *string,
			      TokenList **listPointer) {
  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    return ParserResult_createFromError(lr.syntaxError);
  }

  ParserResult pr = wsky_parse(lr.tokens);
  if (!pr.success) {
    wsky_TokenList_delete(lr.tokens);
    return pr;
  }
  *listPointer = lr.tokens;
  return pr;
}
