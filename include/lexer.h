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
typedef struct wsky_LexerResult_s {

  /** `false` on error */
  bool success;

  /** Undefined on success */
  wsky_SyntaxError syntaxError;

  /** `NULL` on error */
  wsky_TokenList *tokens;
} wsky_LexerResult;

/**
 * Creates a wsky_LexerResult from a wsky_SyntaxError.
 */
wsky_LexerResult wsky_LexerResult_createFromError(wsky_SyntaxError e);

/**
 * Creates a wsky_LexerResult from a wsky_TokenList.
 */
wsky_LexerResult wsky_LexerResult_createFromTokens(wsky_TokenList *tokens);



/**
 * Tokenizes source code from a wsky_StringReader.
 * @param reader The reader
 * @param autoStop If `true`, the lexer stops if it can't recognize a token
 * (instead of thowing an error)
 */
wsky_LexerResult wsky_lexFromReader(wsky_StringReader *reader,
                                    bool autoStop);

/**
 * Tokenizes source code from a null-terminated string.
 */
wsky_LexerResult wsky_lexFromString(const char *string);

/**
 * Tokenizes a template from a wsky_StringReader.
 */
wsky_LexerResult wsky_lexTemplateFromReader(wsky_StringReader *reader);

/**
 * Tokenizes a template from a null-terminated string.
 */
wsky_LexerResult wsky_lexTemplateFromString(const char *string);

/**
 * @}
 */

#endif /* !LEXER_H_ */
