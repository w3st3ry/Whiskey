#ifndef LEXER_H_
# define LEXER_H_

# include "string_reader.h"
# include "syntax_error.h"

typedef struct wsky_LexerResult_s wsky_LexerResult;

struct wsky_LexerResult_s {

  /** Undefined on success */
  wsky_SyntaxError syntaxError;

  /** `NULL` on error */
  wsky_TokenList *tokens;
};



wsky_LexerResult wsky_LexerResult_createFromError(wsky_SyntaxError e);
wsky_LexerResult wsky_LexerResult_createFromTokens(wsky_TokenList *tokens);

wsky_LexerResult *wsky_lexFromReader(wsky_StringReader *reader);
wsky_LexerResult *wsky_lexFromString(const char *string);

#endif /* !LEXER_H_ */