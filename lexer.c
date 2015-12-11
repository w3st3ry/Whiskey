#include "lexer.h"

wsky_LexerResult wsky_LexerResult_createFromError(wsky_SyntaxError e) {
  wsky_LexerResult r = {
    .syntaxError = e,
    .tokens = NULL,
  };
  return r;
}

wsky_LexerResult wsky_LexerResult_createFromTokens(wsky_TokenList *tokens) {
  wsky_LexerResult r = {
    .tokens = tokens,
  };
  return r;
}
