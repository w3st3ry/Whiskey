#include "parser.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "gc.h"


typedef wsky_Operator Operator;
typedef wsky_Position Position;
typedef wsky_Token Token;
typedef wsky_TokenList TokenList;
typedef wsky_SyntaxError SyntaxError;
typedef wsky_ASTNode Node;
typedef wsky_ASTNodeList NodeList;
typedef wsky_ParserResult ParserResult;

#define OP(name) (wsky_Operator_ ## name)

static inline bool isOpToken(Token *token) {
  return token->type == wsky_TokenType_OPERATOR;
}



static inline ParserResult createResultFromError(SyntaxError e) {
  ParserResult r = {
    .success = 0,
    .node = NULL,
    .syntaxError = e,
  };
  return r;
}

static inline ParserResult createNodeResult(Node *n) {
  ParserResult r = {
    .success = 1,
    .node = n,
  };
  return r;
}

static inline ParserResult createError(const char *msg,
                                             const Position pos) {
  SyntaxError e = wsky_SyntaxError_create(msg, pos);
  return createResultFromError(e);
}

/**
 * Prefer ERROR_RESULT. Use this one only if there is no position.
 */
static inline ParserResult createEofError(const char *msg) {
  /*
   * We have no position, so let's create an invalid one.
   * It will be replaced by a valid one later.
   */
  Position position = {
    .file = NULL,
    .index = -1,
    .column = -1,
    .line = -1,
  };
  return createError(msg, position);
}

static inline ParserResult createUnexpectedEofError() {
  return createEofError("Unexpected end of file");
}

static inline ParserResult createUnexpectedTokenError(const Token *t) {
  char *message = wsky_MALLOC(strlen(t->string) + 20);
  sprintf(message, "Unexpected '%s'", t->string);
  ParserResult r = createError(message, t->begin);
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
  return createNodeResult(node);
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
  return createNodeResult(node);
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
  return createNodeResult(node);
}


/* Returns a Token or NULL */
static Token *tryToReadOperator(TokenList **listPointer,
                                Operator expectedOp) {
  if (!*listPointer) {
    return NULL;
  }
  Token *token = &(*listPointer)->token;
  if (!isOpToken(token)) {
    return NULL;
  }
  Operator op = token->v.operator;
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
                                      Operator separatorOperator,
                                      Token *beginToken,
                                      Operator endOperator,
                                      const char *expectedSeparatorErr,
                                      const char *expectedEndErr) {
  NodeList *nodes = NULL;
  bool separated = true;

  while (*listPointer) {
    Token *right = tryToReadOperator(listPointer, endOperator);
    if (right) {
      Node *node = (Node *) wsky_SequenceNode_new(&beginToken->begin,
                                                  nodes);
      return createNodeResult(node);
    }

    if (!separated) {
      Node *lastNode = wsky_ASTNodeList_getLastNode(nodes);
      Position p = lastNode->position;
      wsky_ASTNodeList_delete(nodes);
      return createError(expectedSeparatorErr, p);
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
  return createError(expectedEndErr, beginToken->begin);
}


static ParserResult parseSequence(TokenList **listPointer) {
  Token *left = tryToReadOperator(listPointer, OP(LEFT_PAREN));
  if (!left) {
    return ParserResult_NULL;
  }

  return parseSequenceImpl(listPointer,
                           OP(SEMICOLON),
                           left,
                           OP(RIGHT_PAREN),
                           "Expected ';' or ')'", "Expected ')'");
}


static ParserResult parseFunction(TokenList **listPointer) {
  Token *left = tryToReadOperator(listPointer, OP(LEFT_BRACE));
  if (!left) {
    return ParserResult_NULL;
  }

  TokenList *begin = *listPointer;
  ParserResult paramPr;
  paramPr = parseSequenceImpl(listPointer,
                              OP(COMMA),
                              left,
                              OP(COLON),
                              "Expected ',' or ':'", "Expected ':'");
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
                         OP(SEMICOLON),
                         left,
                         OP(RIGHT_BRACE),
                         "Expected ';' or '}'", "Expected '}'");
  if (!pr.success)
    return pr;
  wsky_SequenceNode *sequence = (wsky_SequenceNode *) pr.node;
  wsky_FunctionNode *func = wsky_FunctionNode_new(left,
                                                  params,
                                                  sequence->children);
  wsky_FREE(sequence);
  return createNodeResult((Node *) func);
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

  return createUnexpectedTokenError(&(*listPointer)->token);
}


/* Returns a heap allocated string or NULL */
static char *parseMemberName(TokenList **listPointer) {
  Token *token = tryToReadKeyword(listPointer, wsky_Keyword_CLASS);
  if (token) {
    return wsky_strdup("class");
  }

  wsky_IdentifierNode *identifier = parseIdentifierNode(listPointer);
  if (!identifier) {
    return NULL;
  }
  char *name = wsky_strdup(identifier->name);
  wsky_ASTNode_delete((Node *) identifier);
  return name;
}


static ParserResult parseMemberAccess(TokenList **listPointer,
                                      Node *left,
                                      Token *dotToken) {
  if (!*listPointer) {
    return createUnexpectedEofError();
  }
  char *name = parseMemberName(listPointer);
  if (!name) {
    return createError("Expected member name after `.`", dotToken->end);
  }
  wsky_MemberAccessNode *node;
  node = wsky_MemberAccessNode_new(dotToken, left, name);
  wsky_FREE(name);
  return createNodeResult((Node *) node);
}


static ParserResult parseCall(TokenList **listPointer,
                              Node *left,
                              Token *leftParen) {

  ParserResult pr;
  pr = parseSequenceImpl(listPointer,
                         OP(COMMA),
                         leftParen,
                         OP(RIGHT_PAREN),
                         "Expected ',' or ')'", "Expected ')'");
  if (!pr.success)
    return pr;
  wsky_SequenceNode *sequence = (wsky_SequenceNode *) pr.node;
  wsky_CallNode *callNode = wsky_CallNode_new(leftParen,
                                              left, sequence->children);
  wsky_FREE(sequence);
  return createNodeResult((Node *) callNode);
}


static ParserResult parseCallDotIndex(TokenList **listPointer) {
  ParserResult leftResult = parseTerm(listPointer);
  if (!leftResult.success) {
    return leftResult;
  }
  Node *left = leftResult.node;

  while (*listPointer) {
    Token *token = &(*listPointer)->token;
    if (!isOpToken(token)) {
      break;
    }

    if (token->v.operator == OP(LEFT_PAREN)) {
      *listPointer = (*listPointer)->next;
      ParserResult pr = parseCall(listPointer, left, token);
      if (!pr.success) {
        wsky_ASTNode_delete(left);
        return pr;
      }
      left = pr.node;

    } else if (token->v.operator == OP(DOT)) {
      *listPointer = (*listPointer)->next;
      ParserResult pr = parseMemberAccess(listPointer, left, token);
      if (!pr.success) {
        wsky_ASTNode_delete(left);
        return pr;
      }
      left = pr.node;

    } else {
      break;
    }
  }

  return createNodeResult(left);
}


static bool isUnaryOperator(Operator op) {
  return (op == OP(MINUS) || op == OP(PLUS) || op == OP(NOT));
}

static ParserResult parseUnary(TokenList **listPointer) {
  if (!*listPointer) {
    return createUnexpectedEofError();
  }

  Token *token = &(*listPointer)->token;
  if (!isOpToken(token)) {
    return parseCallDotIndex(listPointer);
  }

  Operator op = token->v.operator;
  if (!isUnaryOperator(op)) {
    return parseCallDotIndex(listPointer);
  }

  *listPointer = (*listPointer)->next;
  ParserResult rr = parseUnary(listPointer);
  if (!rr.success) {
    return rr;
  }
  Node *node;
  node = (Node *) wsky_OperatorNode_newUnary(token, op, rr.node);
  return createNodeResult(node);
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
    if (!isOpToken(opToken)) {
      break;
    }

    Operator op = opToken->v.operator;
    if (op != OP(STAR) && op != OP(SLASH)) {
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

  return createNodeResult(left);
}


static ParserResult parseAdd(TokenList **listPointer) {
  ParserResult lr = parseMul(listPointer);
  if (!lr.success) {
    return lr;
  }
  Node *left = lr.node;

  while (*listPointer) {
    Token *opToken = &(*listPointer)->token;
    if (!isOpToken(opToken)) {
      break;
    }

    Operator op = opToken->v.operator;
    if (op != OP(PLUS) && op != OP(MINUS)) {
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

  return createNodeResult(left);
}


static bool isComparisonOp(Operator op) {
  return (op == OP(LT) || op == OP(LT_EQ) ||
          op == OP(GT) || op == OP(GT_EQ));
}

static ParserResult parseComparison(TokenList **listPointer) {
  ParserResult lr = parseAdd(listPointer);
  if (!lr.success) {
    return lr;
  }
  Node *left = lr.node;

  while (*listPointer) {
    Token *opToken = &(*listPointer)->token;
    if (!isOpToken(opToken)) {
      break;
    }

    Operator op = opToken->v.operator;
    if (!isComparisonOp(op)) {
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

  return createNodeResult(left);
}


static ParserResult parseEquals(TokenList **listPointer) {
  ParserResult lr = parseComparison(listPointer);
  if (!lr.success) {
    return lr;
  }
  Node *left = lr.node;

  while (*listPointer) {
    Token *opToken = &(*listPointer)->token;
    if (!isOpToken(opToken)) {
      break;
    }

    wsky_Operator op = opToken->v.operator;
    if (op != OP(EQUALS) && op != OP(NOT_EQUALS)) {
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

  return createNodeResult(left);
}


static ParserResult parseBoolOp(TokenList **listPointer) {
  ParserResult lr = parseEquals(listPointer);
  if (!lr.success) {
    return lr;
  }
  Node *left = lr.node;

  while (*listPointer) {
    Token *opToken = &(*listPointer)->token;
    if (!isOpToken(opToken)) {
      break;
    }

    wsky_Operator op = opToken->v.operator;
    if (op != OP(AND) && op != OP(OR)) {
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

  return createNodeResult(left);
}


static ParserResult parseClass(TokenList **listPointer) {
  Token *classToken = tryToReadKeyword(listPointer, wsky_Keyword_CLASS);
  if (!classToken)
    return ParserResult_NULL;
  if (!*listPointer) {
    return createError("Expected class name", classToken->end);
  }
  wsky_IdentifierNode *identifier = parseIdentifierNode(listPointer);
  if (!identifier) {
    return createError("Expected class name", classToken->end);
  }
  const char *name = identifier->name;

  // TODO
  Token *leftParen = tryToReadOperator(listPointer, OP(LEFT_PAREN));
  Token *rightParen = tryToReadOperator(listPointer, OP(RIGHT_PAREN));

  Node *classNode = (Node *)wsky_ClassNode_new(classToken, name);
  wsky_ASTNode_delete((Node *)identifier);
  return createNodeResult(classNode);
}


static ParserResult parseVar(TokenList **listPointer) {
  Token *varToken = tryToReadKeyword(listPointer, wsky_Keyword_VAR);
  if (!varToken)
    return ParserResult_NULL;
  if (!*listPointer) {
    return createError("Expected variable name", varToken->end);
  }
  wsky_IdentifierNode *identifier = parseIdentifierNode(listPointer);
  if (!identifier) {
    return createError("Expected variable name", varToken->end);
  }
  const char *name = identifier->name;

  wsky_VarNode *node;
  Token *assignToken = tryToReadOperator(listPointer, OP(ASSIGN));
  if (assignToken) {
    if (!*listPointer) {
      wsky_ASTNode_delete((Node *) identifier);
      return createUnexpectedEofError();
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
  return createNodeResult((Node *) node);
}

static ParserResult parseAssignement(TokenList **listPointer) {
  ParserResult pr;
  TokenList *begin = *listPointer;

  Node *leftNode = (Node *) parseIdentifierNode(listPointer);
  if (!leftNode) {
    return ParserResult_NULL;
  }

  Token *eqToken = tryToReadOperator(listPointer, OP(ASSIGN));
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
  return createNodeResult((Node *) node);
}

static ParserResult parseCoumpoundExpr(TokenList **listPointer) {
  ParserResult pr;

  pr = parseClass(listPointer);
  if (!pr.success || pr.node)
    return pr;

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
    return createUnexpectedEofError();
  }

  return parseCoumpoundExpr(listPointer);
}

/*
  static ParserResult parseStatement(TokenList **listPointer) {
  if (!*listPointer) {
  return UNEXPECTED_EOF_ERROR_RESULT();
  }

  Position begin = (*listPointer)->token.end;
  ParserResult result = parseExpr(listPointer);
  if (!result.success) {
  return result;
  }
  if (!tryToReadOperator(listPointer, OP(SEMICOLON)) {
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

static Position createZeroPosition() {
  Position position = {
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
    Token *semi = tryToReadOperator(listPointer, OP(SEMICOLON));
    if (!semi) {
      wsky_ASTNodeList_delete(nodes);
      return createUnexpectedTokenError(&(*listPointer)->token);
    }
  }

  Position begin = token ? token->begin : createZeroPosition();
  wsky_SequenceNode *seqNode = wsky_SequenceNode_new(&begin, nodes);
  seqNode->program = true;
  return createNodeResult((Node *) seqNode);
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
    return createResultFromError(lr.syntaxError);
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
    return createResultFromError(lr.syntaxError);
  }

  ParserResult pr = wsky_parseTemplate(lr.tokens);
  wsky_TokenList_delete(lr.tokens);
  if (!pr.success) {
    return pr;
  }
  return pr;
}
