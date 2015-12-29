#ifndef LEXER_H_
# define LEXER_H_

# include "string_reader.h"
# include "syntax_error.h"

/**
 * @defgroup lexer lexer
 * The lexer
 * @{
 */

/**
 * The result returned by the lexer
 */
typedef struct wsky_LexerResult_s wsky_LexerResult;
struct wsky_LexerResult_s {

  /** `false` on error */
  bool success;

  /** Undefined on success */
  wsky_SyntaxError syntaxError;

  /** `NULL` on error */
  wsky_TokenList *tokens;
};

wsky_LexerResult wsky_LexerResult_createFromError(wsky_SyntaxError e);
wsky_LexerResult wsky_LexerResult_createFromTokens(wsky_TokenList *tokens);



/**
 * @param reader The reader
 * @param autoStop If `true`, the lexer stops if it can't recognize a token
 * (instead of thowing an error)
 */
wsky_LexerResult wsky_lexFromReader(wsky_StringReader *reader,
                                    bool autoStop);

wsky_LexerResult wsky_lexFromString(const char *string);

wsky_LexerResult wsky_lexTemplateFromReader(wsky_StringReader *reader);
wsky_LexerResult wsky_lexTemplateFromString(const char *string);

/**
 * @}
 */

#endif /* !LEXER_H_ */
