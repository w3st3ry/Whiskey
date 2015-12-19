#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wsky_gc.h"



// Some shortcuts here...
typedef wsky_Token Token;
typedef wsky_TokenList TokenList;
typedef wsky_StringReader StringReader;
typedef wsky_Position Position;

#define HAS_MORE(reader) wsky_StringReader_hasMore(reader)
#define NEXT(reader) wsky_StringReader_next(reader)

#define ERROR_RESULT(message, pos) TokenResult_createError(message, pos)

#define CREATE_TOKEN(reader, begin, type)               \
  wsky_StringReader_createToken(reader, begin, type)

#define STRING_TOKEN_RESULT(reader, begin, value)       \
  TokenResult_createStringToken(reader,                 \
                                begin,                  \
                                value);

#define INT_TOKEN_RESULT(reader, begin, value)          \
  TokenResult_createIntToken(reader, begin, value);

#define FLOAT_TOKEN_RESULT(reader, begin, value)        \
  TokenResult_createFloatToken(reader, begin, value);

#define TOKEN_RESULT(reader, begin, type)                               \
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

/* TODO: Make an union */
struct TokenResult_s {

  TokenResultType type;

  /** Only defined on success */
  wsky_SyntaxError syntaxError;

  /** Only defined on error */
  Token token;
};

static TokenResult TokenResult_createFromError(wsky_SyntaxError e) {
  TokenResult r = {
    .type = TokenResultType_ERROR,
    .syntaxError = e,
  };
  return  r;
}

static TokenResult TokenResult_createError(const char *message,
                                           Position position) {
  wsky_SyntaxError e = wsky_SyntaxError_create(message, position);
  return  TokenResult_createFromError(e);
}

static TokenResult TokenResult_createFromToken(Token token) {
  TokenResult r = {
    .type = TokenResultType_TOKEN,
    .token = token,
  };
  return  r;
}

static TokenResult TokenResult_createStringToken(StringReader *reader,
                                                 Position position,
                                                 const char *value) {
  Token t = CREATE_TOKEN(reader, position, wsky_TokenType_STRING);
  t.v.stringValue = wsky_STRDUP(value);
  return TokenResult_createFromToken(t);
}

static TokenResult TokenResult_createIntToken(StringReader *reader,
                                              Position position,
                                              int64_t value) {
  Token t = CREATE_TOKEN(reader, position, wsky_TokenType_INT);
  t.v.intValue = value;
  return TokenResult_createFromToken(t);
}

static TokenResult TokenResult_createFloatToken(StringReader *reader,
                                                Position position,
                                                double value) {
  Token t = CREATE_TOKEN(reader, position, wsky_TokenType_FLOAT);
  t.v.floatValue = value;
  return TokenResult_createFromToken(t);
}

static const TokenResult TokenResult_NULL = {
  .type = TokenResultType_NULL,
};



static TokenResult lexMultiLineComment(StringReader *reader,
                                       Position begin) {

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

static TokenResult lexSingleLineComment(StringReader *reader,
                                        Position begin) {

  while (HAS_MORE(reader)) {
    Position previous = reader->position;
    char c = NEXT(reader);
    if (c == '\n') {
      reader->position = previous;
      break;
    }
  }
  return TOKEN_RESULT(reader, begin, wsky_TokenType_COMMENT);
}

static TokenResult lexComment(StringReader *reader) {
  Position begin = reader->position;
  char c = NEXT(reader);
  if (c != '/' || !HAS_MORE(reader)) {
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
static int getStringMaxLength(const char *string, char endChar) {
  const char *begin = string;
  while (*string && *string != endChar) {
    if (*string == '\\') {
      string++;
      if (!*string)
        break;
      if (*string == 'x') {
        string++;
        if (!*string)
          break;
        string++;
        if (!*string)
          break;
      }
    }
    string++;
  }
  return (int)(string - begin);
}

/* Return true on error */
static bool lexStringEscape(StringReader *reader,
                            char *value,
                            int *valueLengthPointer) {
  int valueLength = *valueLengthPointer;
  switch (NEXT(reader)) {
  case 'a':
    value[valueLength++] = '\a'; break;
  case 'b':
    value[valueLength++] = '\b'; break;
  case 'f':
    value[valueLength++] = '\f'; break;
  case 'n':
    value[valueLength++] = '\n'; break;
  case 'r':
    value[valueLength++] = '\r'; break;
  case 't':
    value[valueLength++] = '\t'; break;
  case 'v':
    value[valueLength++] = '\v'; break;
  case 'x': {
    char numberString[3];
    if (!HAS_MORE(reader))
      return true;
    numberString[0] = NEXT(reader);
    if (!HAS_MORE(reader))
      return true;
    numberString[1] = NEXT(reader);
    numberString[2] = '\0';
    char c;
    if (sscanf(numberString, "%hhx", &c) != 1) {
      return true;
    }
    value[valueLength++] = c;
    break;
  }
  case '\\':
    value[valueLength++] = '\\'; break;
  case '\'':
    value[valueLength++] = '\''; break;
  case '\"':
    value[valueLength++] = '\"'; break;
  case '?':
    value[valueLength++] = '?'; break;
  default:
    return true;
  }
  *valueLengthPointer = valueLength;
  return false;
}

/**
 * @param endChar " or '
 */
static TokenResult lexStringEnd(StringReader *reader,
                                Position begin,
                                char endChar) {

  int maxLength = getStringMaxLength(reader->string + begin.index + 1,
                                     endChar);
  char *value = wsky_MALLOC((unsigned)maxLength + 1);
  int valueLength = 0;

  while (HAS_MORE(reader)) {
    char c = NEXT(reader);
    if (c == '\\') {
      if (!HAS_MORE(reader)) {
        wsky_FREE(value);
        return ERROR_RESULT("Expected escape sequence and end of string",
                            begin);
      }
      bool r = lexStringEscape(reader, value, &valueLength);
      if (r) {
        wsky_FREE(value);
        return ERROR_RESULT("Invalid escape sequence", begin);
      }
    } else if (c == endChar) {
      value[valueLength] = '\0';
      TokenResult result = STRING_TOKEN_RESULT(reader, begin, value);
      wsky_FREE(value);
      return result;
    } else {
      value[valueLength++] = c;
    }
  }

  wsky_FREE(value);
  return ERROR_RESULT("Expected end of string", begin);
}

static TokenResult lexString(StringReader *reader) {
  Position begin = reader->position;
  char c = NEXT(reader);
  if (c != '\"' && c != '\'') {
    reader->position = begin;
    return TokenResult_NULL;
  }
  return lexStringEnd(reader, begin, c);
}



static TokenResult parseFloat(StringReader *reader,
                              const char *string,
                              Position begin) {
  char *end;
  double value = strtod(string, &end);

  if ((value == 0.0 && string == end) || end != strlen(string) + string) {
    return ERROR_RESULT("Invalid float number", begin);
  }
  return FLOAT_TOKEN_RESULT(reader, begin, value);
}

static bool isNumberChar(char c) {
  return isdigit(c) || islower(c) || c == '.';
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
 * Returns NULL if unknown base.
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

static TokenResult parseNumber(StringReader *reader,
                               const char *string,
                               Position begin) {

  size_t length = strlen(string);
  if (strchr(string, '.')) {
    return parseFloat(reader, string, begin);
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
    return parseFloat(reader, string, begin);
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

static TokenResult lexNumber(StringReader *reader) {
  Position begin = reader->position;
  char c = NEXT(reader);
  if (!isdigit(c)) {
    reader->position = begin;
    return TokenResult_NULL;
  }

  char buffer[MAX_NUMBER_LENGTH];
  buffer[0] = c;
  int numberLength = 1;
  while (HAS_MORE(reader)) {
    Position previous = reader->position;
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

static TokenResult lexIdentifier(StringReader *reader) {
  Position begin = reader->position;
  char c = NEXT(reader);
  if (!isIdentifierStart(c)) {
    reader->position = begin;
    return TokenResult_NULL;
  }

  while (HAS_MORE(reader)) {
    Position previous = reader->position;
    c = NEXT(reader);

    if (!isIdentifier(c)) {
      reader->position = previous;
      break;
    }
  }

  int length = reader->position.index - begin.index;
  char *string = wsky_MALLOC((size_t) length + 1);
  strncpy(string, reader->string + begin.index, (size_t) length);
  string[length] = '\0';
  wsky_Keyword keyword;
  if (wsky_Keyword_parse(string, &keyword)) {
    wsky_FREE(string);
    return TOKEN_RESULT(reader, begin, wsky_TokenType_IDENTIFIER);
  } else {
    wsky_FREE(string);
    Token token = CREATE_TOKEN(reader, begin, wsky_TokenType_KEYWORD);
    token.v.keyword = keyword;
    return TokenResult_createFromToken(token);
  }
}



static wsky_Operator lexOperatorEq(char a, StringReader *reader) {
  Position previous = reader->position;

  bool equals = false;

  if (HAS_MORE(reader)) {
    char b = NEXT(reader);
    if (b != '=')
      reader->position = previous;
    else
      equals = true;
  }

  switch (a) {
  case '=': return equals ? wsky_Operator_EQUALS : wsky_Operator_ASSIGN;
  case '-': return equals ? wsky_Operator_MINUS_EQ : wsky_Operator_MINUS;
  case '+': return equals ? wsky_Operator_PLUS_EQ : wsky_Operator_PLUS;
  case '*': return equals ? wsky_Operator_STAR_EQ : wsky_Operator_STAR;
  case '/': return equals ? wsky_Operator_SLASH_EQ : wsky_Operator_SLASH;
  case '<': return equals ? wsky_Operator_LT_EQ : wsky_Operator_LT;
  case '>': return equals ? wsky_Operator_GT_EQ : wsky_Operator_GT;
  }
  abort();
}

/**
 * Returns `wsky_Operator_ASSIGN` if not recognized
 */
static wsky_Operator charToOperator(char a) {
  switch (a) {
  case '.': return wsky_Operator_DOT;
  case ',': return wsky_Operator_COMMA;

  case ':': return wsky_Operator_COLON;
  case ';': return wsky_Operator_SEMICOLON;

  case '(': return wsky_Operator_LEFT_PAREN;
  case ')': return wsky_Operator_RIGHT_PAREN;

  case '[': return wsky_Operator_LEFT_BRACKET;
  case ']': return wsky_Operator_RIGHT_BRACKET;

  case '{': return wsky_Operator_LEFT_BRACE;
  case '}': return wsky_Operator_RIGHT_BRACE;
  }
  return wsky_Operator_ASSIGN;
}

static TokenResult lexOperator(StringReader *reader) {

  Position begin = reader->position;
  char c = NEXT(reader);

  switch (c) {
  case '!':
    if (HAS_MORE(reader) && NEXT(reader) == '=') {
      Token token = CREATE_TOKEN(reader, begin, wsky_TokenType_OPERATOR);
      token.v.operator = wsky_Operator_NOT_EQUALS;
      return TokenResult_createFromToken(token);
    }
    reader->position = begin;
    return TokenResult_NULL;

  case '=':
  case '-': case '+':
  case '*': case '/':
  case '<': case '>': {
    wsky_Operator op = lexOperatorEq(c, reader);
    Token token = CREATE_TOKEN(reader, begin, wsky_TokenType_OPERATOR);
    token.v.operator = op;
    return TokenResult_createFromToken(token);
  }
  }
  wsky_Operator op = charToOperator(c);
  if (op == wsky_Operator_ASSIGN) {
    reader->position = begin;
    return TokenResult_NULL;
  }
  Token token = CREATE_TOKEN(reader, begin, wsky_TokenType_OPERATOR);
  token.v.operator = op;
  return TokenResult_createFromToken(token);
}



typedef TokenResult (*LexerFunction)(StringReader *reader);



/**
 * @param functions A null-terminated array of function pointers
 */
static TokenResult lexToken(StringReader *reader,
                            const LexerFunction *functions) {

  LexerFunction function = functions[0];
  int i = 0;
  while (function) {
    Position begin = reader->position;
    TokenResult result = function(reader);
    if (result.type == TokenResultType_ERROR) {
      return result;
    }
    if (result.type == TokenResultType_TOKEN) {
      return result;
    }
    if (begin.index != reader->position.index) {
      fprintf(stderr, "lexToken(): Function index: %d\n", i);
      abort();
    }

    i++;
    function = functions[i];
  }
  return TokenResult_NULL;
}

wsky_LexerResult wsky_lexFromReader(StringReader *reader,
                                    bool autoStop) {

  const LexerFunction functions[] = {
    lexString,
    lexComment,
    lexNumber,
    lexIdentifier,
    lexOperator,
    NULL,
  };

  TokenList *tokens = NULL;

  while (HAS_MORE(reader)) {
    wsky_StringReader_skipWhitespaces(reader);
    if (!HAS_MORE(reader))
      break;

    TokenResult result = lexToken(reader, functions);


    if (result.type == TokenResultType_NULL) {
      if (autoStop)
        break;
      else
        result = ERROR_RESULT("Unexpected token", reader->position);
    }

    if (result.type == TokenResultType_ERROR) {
      wsky_TokenList_delete(tokens);
      wsky_LexerResult lr = {
        .success = false,
        .syntaxError = result.syntaxError,
        .tokens = NULL,
      };
      return lr;
    }

    wsky_TokenList_addToken(&tokens, &result.token);
  }

  wsky_LexerResult lr = {
    .success = true,
    .tokens = tokens,
  };
  return lr;
}

wsky_LexerResult wsky_lexFromString(const char *string) {
  StringReader reader = wsky_StringReader_create(NULL, string);
  return wsky_lexFromReader(&reader, false);
}



#define TEMPLATE_PRINT_BEGIN    "<%="
#define TEMPLATE_PRINT_END      "%>"
#define TEMPLATE_STMTS_BEGIN    "<%"
#define TEMPLATE_STMTS_END      "%>"

static TokenResult lexWhiskeyInTemplate(StringReader *reader,
                                        const char *beginTag,
                                        const char *endTag,
                                        wsky_TokenType tokenType) {
  Position begin = reader->position;

  if (!wsky_StringReader_readString(reader, beginTag)) {
    return TokenResult_NULL;
  }

  wsky_LexerResult lr = wsky_lexFromReader(reader, true);
  if (!lr.success) {
    return TokenResult_createFromError(lr.syntaxError);
  }

  if (!wsky_StringReader_readString(reader, endTag)) {
    return ERROR_RESULT("Expected Whiskey closing tag", begin);
  }

  Token token = CREATE_TOKEN(reader, begin, tokenType);
  token.v.children = lr.tokens;
  return TokenResult_createFromToken(token);
}

static TokenResult lexWhiskeyPrint(StringReader *reader) {
  return lexWhiskeyInTemplate(reader,
                              TEMPLATE_PRINT_BEGIN,
                              TEMPLATE_PRINT_END,
                              wsky_TokenType_WSKY_PRINT);
}

/*
 * Lex Whiskey statements in a Whiskey template
 */
static TokenResult lexWhiskeyStatements(StringReader *reader) {
  return lexWhiskeyInTemplate(reader,
                              TEMPLATE_STMTS_BEGIN,
                              TEMPLATE_STMTS_END,
                              wsky_TokenType_WSKY_STMTS);
}

/*
 * Lex a Whiskey template
 */
static TokenResult lexHtml(StringReader *reader) {
  Position begin = reader->position;

  while (HAS_MORE(reader)) {
    Position previous = reader->position;
    if (wsky_StringReader_readString(reader, TEMPLATE_STMTS_BEGIN)) {
      reader->position = previous;
      break;
    }
    if (wsky_StringReader_readString(reader, TEMPLATE_PRINT_BEGIN)) {
      reader->position = previous;
      break;
    }
    NEXT(reader);
  }
  if (begin.index == reader->position.index)
    return TokenResult_NULL;
  return TOKEN_RESULT(reader, begin, wsky_TokenType_HTML);
}



static void addTokenToTemplate(Token *token, TokenList **tokens) {
  if (token->type == wsky_TokenType_WSKY_STMTS) {
    wsky_TokenList_add(tokens, token->v.children);

    /*
     *'Manual' free. It's not very nice, but wet can’t free
     * the children.
     */
    wsky_FREE(token->string);

  } else if (token->type == wsky_TokenType_WSKY_PRINT ||
             token->type == wsky_TokenType_HTML) {

    wsky_TokenList_addToken(tokens, token);

  } else {
    abort();
  }
}

wsky_LexerResult wsky_lexTemplateFromReader(StringReader *reader) {
  LexerFunction functions[] = {
    lexWhiskeyPrint,
    lexWhiskeyStatements,
    lexHtml,
    NULL,
  };

  TokenList *tokens = NULL;

  while (HAS_MORE(reader)) {
    TokenResult result = lexToken(reader, functions);

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

    addTokenToTemplate(&result.token, &tokens);
  }

  wsky_LexerResult lr = {
    .success = true,
    .tokens = tokens,
  };
  return lr;
}

wsky_LexerResult wsky_lexTemplateFromString(const char *string) {
  StringReader reader = wsky_StringReader_create(NULL, string);
  return wsky_lexTemplateFromReader(&reader);
}
