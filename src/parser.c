#include "parser.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "gc.h"



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
#define EOF_ERROR_RESULT(message)               \
  createEofErrorResult(message)

#define UNEXPECTED_EOF_ERROR_RESULT()           \
  EOF_ERROR_RESULT("Unexpected end of file")

#define UNEXPECTED_TOKEN_ERROR_RESULT(token)    \
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
  char *message = wsky_MALLOC(strlen(t->string) + 20);
  sprintf(message, "Unexpected '%s'", t->string);
  ParserResult r = ERROR_RESULT(message, t->begin);
  wsky_FREE(message);
  return r;
}

static const ParserResult ParserResult_NULL = {
  .success = 1,
  .node = NULL,
};



static ParserResult parseExpr(TokenList **listPointer);



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
  assert(node);
  *listPointer = (*listPointer)->next;
  return NODE_RESULT(node);
}

/* Returns an HTML node or NULL */
static ParserResult parseHtml(TokenList **listPointer) {
  if (!*listPointer) {
    return ParserResult_NULL;
  }

  Token *token = &(*listPointer)->token;
  if (token->type != wsky_TokenType_HTML) {
    return ParserResult_NULL;
  }
  Node *node = (Node *) wsky_HtmlNode_new(token);
  assert(node);
  *listPointer = (*listPointer)->next;
  return NODE_RESULT(node);
}

/* Returns an identifier or NULL */
static wsky_IdentifierNode *parseIdentifierNode(TokenList **listPointer) {
  if (!*listPointer) {
    return NULL;
  }

  Token *token = &(*listPointer)->token;
  if (token->type != wsky_TokenType_IDENTIFIER) {
    return NULL;
  }
  wsky_IdentifierNode *node = wsky_IdentifierNode_new(token);
  assert(node);
  *listPointer = (*listPointer)->next;
  return node;
}

/* Returns an identifier or NULL */
static ParserResult parseIdentifier(TokenList **listPointer) {
  Node *node = (Node *) parseIdentifierNode(listPointer);
  if (!node) {
    return ParserResult_NULL;
  }
  return NODE_RESULT(node);
}

/* Returns a Token or NULL */
static Token *tryToReadOperator(TokenList **listPointer,
                                wsky_Operator expectedOp) {
  if (!*listPointer) {
    return NULL;
  }
  Token *token = &(*listPointer)->token;
  if (token->type != wsky_TokenType_OPERATOR) {
    return NULL;
  }
  wsky_Operator op = token->v.operator;
  if (op != expectedOp) {
    return NULL;
  }
  *listPointer = (*listPointer)->next;
  return token;
}

/* Returns a Token or NULL */
static Token *tryToReadKeyword(TokenList **listPointer,
                               wsky_Keyword expectedKeyword) {
  if (!*listPointer) {
    return NULL;
  }
  Token *token = &(*listPointer)->token;
  if (token->type != wsky_TokenType_KEYWORD) {
    return NULL;
  }
  wsky_Keyword keyword = token->v.keyword;
  if (keyword != expectedKeyword) {
    return NULL;
  }
  *listPointer = (*listPointer)->next;
  return token;
}

static ParserResult parseSequenceImpl(TokenList **listPointer,
                                      wsky_Operator separatorOperator,
                                      Token *beginToken,
                                      wsky_Operator endOperator,
                                      const char *expectedSeparatorErr,
                                      const char *expectedEndErr) {
  NodeList *nodes = NULL;
  bool separated = true;

  while (*listPointer) {
    Token *right = tryToReadOperator(listPointer, endOperator);
    if (right) {
      Node *node = (Node *) wsky_SequenceNode_new(&beginToken->begin,
                                                  nodes);
      return NODE_RESULT(node);
    }

    if (!separated) {
      Node *lastNode = wsky_ASTNodeList_getLastNode(nodes);
      wsky_Position p = lastNode->position;
      wsky_ASTNodeList_delete(nodes);
      return ERROR_RESULT(expectedSeparatorErr, p);
    }

    ParserResult r = parseExpr(listPointer);
    if (!r.success) {
      wsky_ASTNodeList_delete(nodes);
      return r;
    }
    wsky_ASTNodeList_addNode(&nodes, r.node);
    separated = tryToReadOperator(listPointer, separatorOperator);
  }
  wsky_ASTNodeList_delete(nodes);
  return ERROR_RESULT(expectedEndErr, beginToken->begin);
}

static ParserResult parseSequence(TokenList **listPointer) {
  Token *left = tryToReadOperator(listPointer, wsky_Operator_LEFT_PAREN);
  if (!left) {
    return ParserResult_NULL;
  }

  return parseSequenceImpl(listPointer,
                           wsky_Operator_SEMICOLON,
                           left,
                           wsky_Operator_RIGHT_PAREN,
                           "Expected ';' or ')'",
                           "Expected ')'");
}

static ParserResult parseFunction(TokenList **listPointer) {
  Token *left = tryToReadOperator(listPointer, wsky_Operator_LEFT_BRACE);
  if (!left) {
    return ParserResult_NULL;
  }

  TokenList *begin = *listPointer;
  ParserResult paramPr;
  paramPr = parseSequenceImpl(listPointer,
                              wsky_Operator_COMMA,
                              left,
                              wsky_Operator_COLON,
                              "Expected ',' or ':'",
                              "Expected ':'");
  NodeList *params;
  if (paramPr.success) {
    wsky_SequenceNode *sequence = (wsky_SequenceNode *) paramPr.node;
    params = sequence->children;
    wsky_FREE(sequence);
  } else {
    params = NULL;
    *listPointer = begin;
    wsky_SyntaxError_free(&paramPr.syntaxError);
  }

  ParserResult pr;
  pr = parseSequenceImpl(listPointer,
                         wsky_Operator_SEMICOLON,
                         left,
                         wsky_Operator_RIGHT_BRACE,
                         "Expected ';' or '}'",
                         "Expected '}'");
  if (!pr.success)
    return pr;
  wsky_SequenceNode *sequence = (wsky_SequenceNode *) pr.node;
  wsky_FunctionNode *func = wsky_FunctionNode_new(left,
                                                  params,
                                                  sequence->children);
  wsky_FREE(sequence);
  return NODE_RESULT((Node *) func);
}

// TODO: Rename
static ParserResult parseTerm(TokenList **listPointer) {
  ParserResult result;

  result = parseIdentifier(listPointer);
  if (result.node)
    return result;

  result = parseLiteral(listPointer);
  if (result.node)
    return result;

  result = parseHtml(listPointer);
  if (result.node)
    return result;

  /*
    result = parseTemplatePrint(listPointer);
    if (result.node)
    return result;
  */

  result = parseSequence(listPointer);
  if (!result.success || result.node)
    return result;

  result = parseFunction(listPointer);
  if (!result.success || result.node)
    return result;

  return UNEXPECTED_TOKEN_ERROR_RESULT(&(*listPointer)->token);
}



static ParserResult parseMemberAccess(TokenList **listPointer,
                                      Node *left,
                                      Token *dotToken) {
  if (!*listPointer) {
    return UNEXPECTED_EOF_ERROR_RESULT();
  }
  wsky_IdentifierNode *identifier = parseIdentifierNode(listPointer);
  if (!identifier) {
    return ERROR_RESULT("Expected member name", dotToken->end);
  }
  wsky_MemberAccessNode *node;
  node = wsky_MemberAccessNode_new(dotToken, left, identifier->name);
  wsky_ASTNode_delete((Node *) identifier);
  return NODE_RESULT((Node *) node);
}


static ParserResult parseCall(TokenList **listPointer,
                              Node *left,
                              Token *leftParen) {

  ParserResult pr;
  pr = parseSequenceImpl(listPointer,
                         wsky_Operator_COMMA,
                         leftParen,
                         wsky_Operator_RIGHT_PAREN,
                         "Expected ',' or ')'",
                         "Expected ')'");
  if (!pr.success)
    return pr;
  wsky_SequenceNode *sequence = (wsky_SequenceNode *) pr.node;
  wsky_CallNode *callNode = wsky_CallNode_new(leftParen,
                                              left, sequence->children);
  wsky_FREE(sequence);
  return NODE_RESULT((Node *) callNode);
}


static ParserResult parseCallDotIndex(TokenList **listPointer) {
  ParserResult leftResult = parseTerm(listPointer);
  if (!leftResult.success) {
    return leftResult;
  }
  Node *left = leftResult.node;

  while (*listPointer) {
    Token *token = &(*listPointer)->token;
    if (token->type != wsky_TokenType_OPERATOR) {
      break;
    }

    if (token->v.operator == wsky_Operator_LEFT_PAREN) {
      *listPointer = (*listPointer)->next;
      ParserResult pr = parseCall(listPointer, left, token);
      if (!pr.success) {
        return pr;
      }
      left = pr.node;

    } else if (token->v.operator == wsky_Operator_DOT) {
      *listPointer = (*listPointer)->next;
      ParserResult pr = parseMemberAccess(listPointer, left, token);
      if (!pr.success) {
        return pr;
      }
      left = pr.node;

    } else {
      break;
    }
  }

  return NODE_RESULT(left);
}


static bool isUnaryOperator(wsky_Operator op) {
  return (op == wsky_Operator_MINUS ||
          op == wsky_Operator_PLUS ||
          op == wsky_Operator_NOT);
}

static ParserResult parseUnary(TokenList **listPointer) {
  if (!*listPointer) {
    return UNEXPECTED_EOF_ERROR_RESULT();
  }

  Token *opToken = &(*listPointer)->token;
  if (opToken->type != wsky_TokenType_OPERATOR) {
    return parseCallDotIndex(listPointer);
  }

  wsky_Operator op = opToken->v.operator;
  if (!isUnaryOperator(op)) {
    return parseCallDotIndex(listPointer);
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
    left = (Node *) wsky_OperatorNode_new(opToken, left, op, rr.node);
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
    left = (Node *) wsky_OperatorNode_new(opToken, left, op, rr.node);
  }

  return NODE_RESULT(left);
}

static ParserResult parseComparison(TokenList **listPointer) {
  ParserResult lr = parseAdd(listPointer);
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
    if (op != wsky_Operator_LT && op != wsky_Operator_LT_EQ &&
        op != wsky_Operator_GT && op != wsky_Operator_GT_EQ) {
      break;
    }

    *listPointer = (*listPointer)->next;
    ParserResult rr = parseAdd(listPointer);
    if (!rr.success) {
      wsky_ASTNode_delete(left);
      return rr;
    }
    left = (Node *) wsky_OperatorNode_new(opToken, left, op, rr.node);
  }

  return NODE_RESULT(left);
}

static ParserResult parseEquals(TokenList **listPointer) {
  ParserResult lr = parseComparison(listPointer);
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
    if (op != wsky_Operator_EQUALS && op != wsky_Operator_NOT_EQUALS) {
      break;
    }

    *listPointer = (*listPointer)->next;
    ParserResult rr = parseComparison(listPointer);
    if (!rr.success) {
      wsky_ASTNode_delete(left);
      return rr;
    }
    left = (Node *) wsky_OperatorNode_new(opToken, left, op, rr.node);
  }

  return NODE_RESULT(left);
}


static ParserResult parseBoolOp(TokenList **listPointer) {
  ParserResult lr = parseEquals(listPointer);
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
    if (op != wsky_Operator_AND && op != wsky_Operator_OR) {
      break;
    }

    *listPointer = (*listPointer)->next;
    ParserResult rr = parseEquals(listPointer);
    if (!rr.success) {
      wsky_ASTNode_delete(left);
      return rr;
    }
    left = (Node *) wsky_OperatorNode_new(opToken, left, op, rr.node);
  }

  return NODE_RESULT(left);
}


static ParserResult parseVar(TokenList **listPointer) {
  Token *varToken = tryToReadKeyword(listPointer, wsky_Keyword_VAR);
  if (!varToken)
    return ParserResult_NULL;
  if (!*listPointer) {
    return ERROR_RESULT("Expected variable name", varToken->end);
  }
  wsky_IdentifierNode *identifier = parseIdentifierNode(listPointer);
  if (!identifier) {
    return ERROR_RESULT("Expected variable name", varToken->end);
  }
  const char *name = identifier->name;

  wsky_VarNode *node;
  Token *assignToken = tryToReadOperator(listPointer, wsky_Operator_ASSIGN);
  if (assignToken) {
    if (!*listPointer) {
      wsky_ASTNode_delete((Node *) identifier);
      return UNEXPECTED_EOF_ERROR_RESULT();
    }
    ParserResult pr = parseExpr(listPointer);
    if (!pr.success) {
      wsky_ASTNode_delete((Node *) identifier);
      return pr;
    }
    node = wsky_VarNode_new(varToken, name, pr.node);
  } else {
    node = wsky_VarNode_new(varToken, name, NULL);
  }
  wsky_ASTNode_delete((Node *) identifier);
  return NODE_RESULT((Node *) node);
}

static ParserResult parseAssignement(TokenList **listPointer) {
  ParserResult pr;
  TokenList *begin = *listPointer;

  Node *leftNode = (Node *) parseIdentifierNode(listPointer);
  if (!leftNode) {
    return ParserResult_NULL;
  }

  Token *eqToken = tryToReadOperator(listPointer, wsky_Operator_ASSIGN);
  if (!eqToken) {
    wsky_ASTNode_delete(leftNode);
    *listPointer = begin;
    return ParserResult_NULL;
  }

  pr = parseExpr(listPointer);
  if (!pr.success) {
    wsky_ASTNode_delete(leftNode);
    /*
    wsky_SyntaxError_freen(&pr.syntaxError);
    *listPointer = begin;
    return ParserResult_NULL;
    */
    return pr;
  }
  Node *rightNode = pr.node;

  wsky_AssignmentNode *node;
  node = wsky_AssignmentNode_new(eqToken, leftNode, rightNode);
  return NODE_RESULT((Node *) node);
}

static ParserResult parseCoumpoundExpr(TokenList **listPointer) {
  ParserResult pr;

  pr = parseVar(listPointer);
  if (!pr.success || pr.node)
    return pr;

  pr = parseAssignement(listPointer);
  if (!pr.success || pr.node)
    return pr;

  return parseBoolOp(listPointer);
}

static ParserResult parseExpr(TokenList **listPointer) {
  if (!*listPointer) {
    return UNEXPECTED_EOF_ERROR_RESULT();
  }

  return parseCoumpoundExpr(listPointer);
}

/*
  static ParserResult parseStatement(TokenList **listPointer) {
  if (!*listPointer) {
  return UNEXPECTED_EOF_ERROR_RESULT();
  }

  wsky_Position begin = (*listPointer)->token.end;
  ParserResult result = parseExpr(listPointer);
  if (!result.success) {
  return result;
  }
  if (!tryToReadOperator(listPointer, wsky_Operator_SEMICOLON)) {
  return ERROR_RESULT("Expected ';'", begin);
  }
  return result;
  }
*/



/*
 * 'Unexpected EOF' errors have an invalid position (-1) at the time
 * of their creation, because the line, the index and the column
 * numbers are unknown. This function sets their position to a real
 * value.
 */
static void setEOFErrorPosition(ParserResult *result,
                                TokenList *begin) {
  if (!result->success) {
    wsky_SyntaxError *e = &result->syntaxError;
    if (e->position.index == -1 && begin) {
      /* That's an "unexpected EOF error" */
      Token *lastToken = &wsky_TokenList_getLast(begin)->token;
      e->position = lastToken->end;
    }
  }
}

static wsky_Position createZeroPosition() {
  wsky_Position position = {
    .file = NULL,
    .index = 0,
    .column = 0,
    .line = 0,
  };
  return position;
}

static ParserResult parseProgram(TokenList **listPointer) {
  NodeList *nodes = NULL;

  Token *token = &(*listPointer)->token;
  while (*listPointer) {
    ParserResult pr = parseExpr(listPointer);
    if (!pr.success) {
      wsky_ASTNodeList_delete(nodes);
      return pr;
    }
    wsky_ASTNodeList_addNode(&nodes, pr.node);

    if (!*listPointer)
      break;
    Token *semi = tryToReadOperator(listPointer, wsky_Operator_SEMICOLON);
    if (!semi) {
      wsky_ASTNodeList_delete(nodes);
      return UNEXPECTED_TOKEN_ERROR_RESULT(&(*listPointer)->token);
    }
  }

  wsky_Position begin = token ? token->begin : createZeroPosition();
  wsky_SequenceNode *seqNode = wsky_SequenceNode_new(&begin, nodes);
  seqNode->program = true;
  return NODE_RESULT((Node *) seqNode);
}


ParserResult wsky_parse(TokenList *tokens) {
  TokenList *begin = tokens;

  ParserResult r = parseProgram(&tokens);
  setEOFErrorPosition(&r, begin);
  return r;
}


ParserResult wsky_parseLine(TokenList *tokens) {
  return parseProgram(&tokens);
}


wsky_ParserResult wsky_parseTemplate(wsky_TokenList *tokens) {
  return wsky_parse(tokens);
}

ParserResult wsky_parseString(const char *string) {
  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    return ParserResult_createFromError(lr.syntaxError);
  }

  ParserResult pr = wsky_parse(lr.tokens);
  wsky_TokenList_delete(lr.tokens);
  if (!pr.success) {
    return pr;
  }
  return pr;
}

wsky_ParserResult wsky_parseTemplateString(const char *string) {
  wsky_LexerResult lr = wsky_lexTemplateFromString(string);
  if (!lr.success) {
    return ParserResult_createFromError(lr.syntaxError);
  }

  ParserResult pr = wsky_parseTemplate(lr.tokens);
  wsky_TokenList_delete(lr.tokens);
  if (!pr.success) {
    return pr;
  }
  return pr;
}
