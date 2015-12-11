#include "lexer.h"

#include <stdlib.h>
#include <string.h>



// Some shortcuts here...
#define HAS_MORE(reader) wsky_StringReader_hasMore(reader)
#define NEXT(reader) wsky_StringReader_next(reader)

#define ERROR_RESULT(message, pos) TokenResult_createError(message, pos)

#define CREATE_TOKEN(reader, begin, type)		\
  wsky_StringReader_createToken(reader, begin, type)

#define STRING_TOKEN_RESULT(reader, begin, value)	\
  TokenResult_createStringToken(reader,			\
				begin,			\
				value);



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



typedef enum {
  TokenResultType_TOKEN,
  TokenResultType_NULL,
  TokenResultType_ERROR,
} TokenResultType;

typedef struct TokenResult_s TokenResult;

struct TokenResult_s {

  TokenResultType type;

  /** Only defined on success */
  wsky_SyntaxError syntaxError;

  /** Only defined on error */
  wsky_Token token;
};

static TokenResult TokenResult_createFromError(wsky_SyntaxError e) {
  TokenResult r = {
    .type = TokenResultType_ERROR,
    .syntaxError = e,
  };
  return  r;
}

static TokenResult TokenResult_createError(const char *message,
					   wsky_Position position) {
  wsky_SyntaxError e = wsky_SyntaxError_create(message, position);
  return  TokenResult_createFromError(e);
}

static TokenResult TokenResult_createFromToken(wsky_Token token) {
  TokenResult r = {
    .type = TokenResultType_TOKEN,
    .token = token,
  };
  return  r;
}

static TokenResult TokenResult_createStringToken(wsky_StringReader *reader,
						 wsky_Position position,
						 const char *value) {
  wsky_Token t = CREATE_TOKEN(reader, position, wsky_TokenType_STRING);
  t.v.stringValue = strdup(value);
  return TokenResult_createFromToken(t);
}

static const TokenResult TokenResult_NULL = {
  .type = TokenResultType_NULL,
};



/**
 * @param endChar " or '
 */
static int getStringMaxLength(const char *string, char endChar) {
  const char *begin = string;
  while (*string && *string != endChar) {
    if (*string == '\\') {
      string++;
      if (!*string)
	break;
    }
    string++;
  }
  return (int)(string - begin);
}

/**
 * @param endChar " or '
 */
static TokenResult lexStringEnd(wsky_StringReader *reader,
				wsky_Position begin,
				char endChar) {
  int maxLength = getStringMaxLength(reader->string + begin.index, endChar);
  char *value = malloc((unsigned)maxLength + 1);
  while (HAS_MORE(reader)) {
    char c = NEXT(reader);
    if (c == '\\') {
    } else if (c == '\"') {
      TokenResult result = STRING_TOKEN_RESULT(reader, begin, value);
      free(value);
      return result;
    }
  }
  return ERROR_RESULT("Expected end of string", begin);
}

static TokenResult lexString(wsky_StringReader *reader) {
  wsky_Position begin = reader->position;
  char c = NEXT(reader);
  if (c != '\"' || c != '\'') {
    reader->position = begin;
    return TokenResult_NULL;
  }
  return lexStringEnd(reader, begin, c);
}

static bool isDigit(char c) {
  return (c >= '0' || c <= '9');
}

/*
static TokenResult lexNumber(wsky_StringReader *reader) {
  wsky_Position begin = reader->position;
  char c = NEXT(reader);
  if (!isDigit(c)) {
    reader->position = begin;
    return TokenResult_NULL;
  }
  while () {

  }
  }
*/

wsky_LexerResult wsky_lexFromReader(wsky_StringReader *reader) {
  /* TODO: ... */
  (void) reader;
  abort();
  /*
  wsky_LexerResult result;
  return result;
  */
}

wsky_LexerResult wsky_lexFromString(const char *string) {
  wsky_StringReader reader = wsky_StringReader_create(NULL, string);
  return wsky_lexFromReader(&reader);
}
