#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>



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

#define INT_TOKEN_RESULT(reader, begin, value)		\
  TokenResult_createIntToken(reader, begin, value);

#define TOKEN_RESULT(reader, begin, type)				\
  TokenResult_createFromToken(CREATE_TOKEN(reader, begin, type));



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

static TokenResult TokenResult_createIntToken(wsky_StringReader *reader,
					      wsky_Position position,
					      int64_t value) {
  wsky_Token t = CREATE_TOKEN(reader, position, wsky_TokenType_INT);
  t.v.intValue = value;
  return TokenResult_createFromToken(t);
}

static TokenResult TokenResult_createFloatToken(wsky_StringReader *reader,
						wsky_Position position,
						double value) {
  wsky_Token t = CREATE_TOKEN(reader, position, wsky_TokenType_FLOAT);
  t.v.floatValue = value;
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



static TokenResult lexMultiLineComment(wsky_StringReader *reader,
				       wsky_Position begin) {

  while (HAS_MORE(reader)) {
    char c = NEXT(reader);
    if (c == '*') {
      c = NEXT(reader);
      if (c == '/')
	return TOKEN_RESULT(reader, begin, wsky_TokenType_COMMENT);
    }
  }
  return ERROR_RESULT("Expected */", begin);
}

static TokenResult lexSingleLineComment(wsky_StringReader *reader,
					wsky_Position begin) {

  while (HAS_MORE(reader)) {
    wsky_Position previous = reader->position;
    char c = NEXT(reader);
    if (c == '\n') {
      reader->position = previous;
      return TOKEN_RESULT(reader, begin, wsky_TokenType_COMMENT);
    }
  }
  return ERROR_RESULT("Expected */", begin);
}

static TokenResult lexComment(wsky_StringReader *reader) {
  wsky_Position begin = reader->position;
  char c = NEXT(reader);
  if (c != '/') {
    reader->position = begin;
    return TokenResult_NULL;
  }

  c = NEXT(reader);
  if (c == '/') {
    return lexSingleLineComment(reader, begin);
  } else if (c == '*') {
    return lexMultiLineComment(reader, begin);
  }
  reader->position = begin;
  return TokenResult_NULL;
}



/**
 * @param endChar " or '
 */
static TokenResult lexStringEnd(wsky_StringReader *reader,
				wsky_Position begin,
				char endChar) {

  int maxLength = getStringMaxLength(reader->string + begin.index + 1,
				     endChar);
  char *value = malloc((unsigned)maxLength + 1);
  int valueLength = 0;

  while (HAS_MORE(reader)) {
    char c = NEXT(reader);

    if (c == '\\') {
      /* TODO:  */
    } else if (c == endChar) {
      value[valueLength] = '\0';
      TokenResult result = STRING_TOKEN_RESULT(reader, begin, value);
      free(value);
      return result;
    }

    value[valueLength++] = c;
  }

  free(value);
  return ERROR_RESULT("Expected end of string", begin);
}

static TokenResult lexString(wsky_StringReader *reader) {
  wsky_Position begin = reader->position;
  char c = NEXT(reader);
  if (c != '\"' && c != '\'') {
    reader->position = begin;
    return TokenResult_NULL;
  }
  return lexStringEnd(reader, begin, c);
}



static bool isNumberChar(char c) {
  return isdigit(c) || islower(c) || c == '.';
}

static TokenResult parseFloat(const char *string, wsky_Position begin) {
  (void) string;

  return ERROR_RESULT("Floating point numbers are not implemented yet",
		      begin);
}

/**
 * Returns -1 on error
 */
static int digitToInt(char c, const char *baseChars) {
  int base = (int) strlen(baseChars);
  int i;
  for (i = 0; i < base; i++) {
    if (tolower(c) == tolower(baseChars[i])) {
      return i;
    }
  }
  return -1;
}

/**
 * Returns NULL if unkown base.
 */
static const char *getNumberBaseChars(int base) {
  switch (base) {
  case 2:
    return "01";
  case 8:
    return "01234567";
  case 10:
    return "0123456789";
  case 16:
    return "0123456789abcdef";
  default:
    return NULL;
  }
}

/**
 * Returns -1 on error
 */
static int64_t parseUintBase(const char *string, const char *baseChars) {
  int64_t number = 0;
  int base = (int) strlen(baseChars);

  while (*string) {
    int digit = digitToInt(*string, baseChars);
    if (digit == -1)
      return -1;

    /* Overflow check */
    int64_t previousNumber = number;
    number *= base;
    if (number / base != previousNumber)
      return -1;
    previousNumber = number;
    number += digit;
    if (number - digit != previousNumber)
      return -1;

    string++;
  }
  return number;
}

static TokenResult parseNumber(wsky_StringReader *reader,
			       const char *string,
			       wsky_Position begin) {

  size_t length = strlen(string);
  if (strchr(string, '.')) {
    return parseFloat(string, begin);
  }

  int base = 10;
  if (string[0] == '0') {
    switch (string[1]) {
    case 'x':
      base = 16;
      string += 2;
      break;
    case 'b':
      base = 2;
      string += 2;
      break;
    }
  }

  if (base == 10 && string[length - 1] == 'f') {
    return parseFloat(string, begin);
  }

  if (!*string)
    return ERROR_RESULT("Invalid number", begin);

  const char *baseChars = getNumberBaseChars(base);
  if (!baseChars)
    abort();
  int64_t v = parseUintBase(string, baseChars);
  if (v == -1)
    return ERROR_RESULT("Invalid number", begin);
  return INT_TOKEN_RESULT(reader, begin, v);
}

#define MAX_NUMBER_LENGTH 64

static TokenResult lexNumber(wsky_StringReader *reader) {
  wsky_Position begin = reader->position;
  char c = NEXT(reader);
  if (!isdigit(c)) {
    reader->position = begin;
    return TokenResult_NULL;
  }

  char buffer[MAX_NUMBER_LENGTH];
  buffer[0] = c;
  int numberLength = 1;
  while (HAS_MORE(reader)) {
    wsky_Position previous = reader->position;
    c = NEXT(reader);

    if (!isNumberChar(c)) {
      reader->position = previous;
      break;
    }
    if (numberLength >= MAX_NUMBER_LENGTH - 1) {
      return ERROR_RESULT("Too long number", begin);
    }
    buffer[numberLength] = c;
    numberLength++;
  }
  buffer[numberLength] = '\0';
  return parseNumber(reader, buffer, begin);
}



static bool isIdentifierStart(char c) {
  return c == '_' || isalpha(c);
}

static bool isIdentifier(char c) {
  return isIdentifierStart(c) || isdigit(c);
}

static TokenResult lexIdentifier(wsky_StringReader *reader) {
  wsky_Position begin = reader->position;
  char c = NEXT(reader);
  if (!isIdentifierStart(c)) {
    reader->position = begin;
    return TokenResult_NULL;
  }

  while (HAS_MORE(reader)) {
    wsky_Position previous = reader->position;
    c = NEXT(reader);

    if (!isIdentifier(c)) {
      reader->position = previous;
      break;
    }
  }
  return TOKEN_RESULT(reader, begin, wsky_TokenType_IDENTIFIER);
}



typedef TokenResult (*LexerFunction)(wsky_StringReader *reader);



static TokenResult lexToken(wsky_StringReader *reader) {
  LexerFunction functions[] = {
    lexString,
    lexComment,
    lexNumber,
    lexIdentifier,
    NULL,
  };

  LexerFunction function = functions[0];
  int i = 0;
  while (function) {
    wsky_Position begin = reader->position;
    TokenResult result = function(reader);
    if (result.type == TokenResultType_ERROR) {
      return result;
    }
    if (result.type == TokenResultType_TOKEN) {
      return result;
    }
    if (begin.index != reader->position.index) {
      abort();
    }

    i++;
    function = functions[i];
  }
  return ERROR_RESULT("Unexpected token", reader->position);
}

wsky_LexerResult wsky_lexFromReader(wsky_StringReader *reader) {
  wsky_TokenList *tokens = NULL;

  while (HAS_MORE(reader)) {
    wsky_StringReader_skipWhitespaces(reader);
    if (!HAS_MORE(reader))
      break;

    TokenResult result = lexToken(reader);

    if (result.type == TokenResultType_ERROR) {
      wsky_TokenList_delete(tokens);
      wsky_LexerResult lr = {
	.success = false,
	.syntaxError = result.syntaxError,
	.tokens = NULL,
      };
      return lr;
    }

    if (result.type == TokenResultType_NULL) {
      abort();
    }

    wsky_TokenList_add(&tokens, result.token);
  }

  wsky_LexerResult lr = {
    .success = true,
    .tokens = tokens,
  };
  return lr;
}

wsky_LexerResult wsky_lexFromString(const char *string) {
  wsky_StringReader reader = wsky_StringReader_create(NULL, string);
  return wsky_lexFromReader(&reader);
}
