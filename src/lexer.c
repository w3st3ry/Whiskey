#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "whiskey_private.h"


static inline bool hasMore(const StringReader *reader) {
  return wsky_StringReader_hasMore(reader);
}

static inline char next(StringReader *reader) {
  return wsky_StringReader_next(reader);
}

static inline Token createToken(StringReader *reader, Position begin,
                                TokenType type) {
  return wsky_StringReader_createToken(reader, begin, type);
}



wsky_LexerResult wsky_LexerResult_createFromError(SyntaxError e) {
  wsky_LexerResult r = {
    .syntaxError = e,
    .tokens = NULL,
  };
  return r;
}

wsky_LexerResult wsky_LexerResult_createFromTokens(TokenList *tokens) {
  wsky_LexerResult r = {
    .tokens = tokens,
  };
  return r;
}



typedef enum {
  ResultType_TOKEN,
  ResultType_NULL,
  ResultType_ERROR,
} ResultType;


/* TODO: Make an union */
typedef struct Result_s {

  ResultType type;

  /** Only defined on success */
  SyntaxError syntaxError;

  /** Only defined on error */
  Token token;
} Result;

static inline Result createResultFromError(SyntaxError e) {
  Result r = {
    .type = ResultType_ERROR,
    .syntaxError = e,
  };
  return r;
}

static inline Result createErrorResult(const char *message,
                                       Position position) {
  SyntaxError e = wsky_SyntaxError_create(message, position);
  return createResultFromError(e);
}

static inline Result createResultFromToken(Token token) {
  Result r = {
    .type = ResultType_TOKEN,
    .token = token,
  };
  return r;
}

static inline Result createTokenResult(StringReader *reader,
                                       Position begin,
                                       TokenType type) {
  Token token = wsky_StringReader_createToken(reader, begin, type);
  return createResultFromToken(token);
}

static inline Result createStringTokenResult(StringReader *reader,
                                             Position position,
                                             const char *value) {
  Token t = createToken(reader, position, wsky_TokenType_STRING);
  t.v.stringValue = wsky_strdup(value);
  return createResultFromToken(t);
}

static inline Result createIntTokenResult(StringReader *reader,
                                          Position position,
                                          wsky_int value) {
  Token t = createToken(reader, position, wsky_TokenType_INT);
  t.v.intValue = value;
  return createResultFromToken(t);
}

static inline Result createFloatTokenResult(StringReader *reader,
                                            Position position,
                                            wsky_float value) {
  Token t = createToken(reader, position, wsky_TokenType_FLOAT);
  t.v.floatValue = value;
  return createResultFromToken(t);
}

static inline Result createOpTokenResult(StringReader *reader,
                                         Position position,
                                         wsky_Operator op) {
  Token t = createToken(reader, position, wsky_TokenType_OPERATOR);
  t.v.operator = op;
  return createResultFromToken(t);
}


static const Result Result_NULL = {
  .type = ResultType_NULL,
};



static Result lexMultiLineComment(StringReader *reader,
                                  Position begin) {

  while (hasMore(reader)) {
    char c = next(reader);
    if (c == '*') {
      c = next(reader);
      if (c == '/')
        return createTokenResult(reader, begin, wsky_TokenType_COMMENT);
    }
  }
  return createErrorResult("Expected */", begin);
}

static Result lexSingleLineComment(StringReader *reader,
                                   Position begin) {

  while (hasMore(reader)) {
    Position previous = reader->position;
    char c = next(reader);
    if (c == '\n') {
      reader->position = previous;
      break;
    }
  }
  return createTokenResult(reader, begin, wsky_TokenType_COMMENT);
}

static Result lexComment(StringReader *reader) {
  Position begin = reader->position;
  char c = next(reader);
  if (c != '/' || !hasMore(reader)) {
    reader->position = begin;
    return Result_NULL;
  }

  c = next(reader);
  if (c == '/') {
    return lexSingleLineComment(reader, begin);
  } else if (c == '*') {
    return lexMultiLineComment(reader, begin);
  }
  reader->position = begin;
  return Result_NULL;
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
  switch (next(reader)) {
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
    if (!hasMore(reader))
      return true;
    numberString[0] = next(reader);
    if (!hasMore(reader))
      return true;
    numberString[1] = next(reader);
    numberString[2] = '\0';
    unsigned char c;
    if (sscanf(numberString, "%hhx", &c) != 1) {
      return true;
    }
    value[valueLength++] = (char)c;
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
static Result lexStringEnd(StringReader *reader,
                           Position begin,
                           char endChar) {

  int maxLength = getStringMaxLength(reader->string + begin.index + 1,
                                     endChar);
  char *value = wsky_safeMalloc((unsigned)maxLength + 1);
  int valueLength = 0;

  while (hasMore(reader)) {
    char c = next(reader);
    if (c == '\\') {
      if (!hasMore(reader)) {
        wsky_free(value);
        return createErrorResult("Expected escape sequence and end of string",
                                 begin);
      }
      bool r = lexStringEscape(reader, value, &valueLength);
      if (r) {
        wsky_free(value);
        return createErrorResult("Invalid escape sequence", begin);
      }
    } else if (c == endChar) {
      value[valueLength] = '\0';
      Result result = createStringTokenResult(reader, begin, value);
      wsky_free(value);
      return result;
    } else {
      value[valueLength++] = c;
    }
  }

  wsky_free(value);
  return createErrorResult("Expected end of string", begin);
}

static Result lexString(StringReader *reader) {
  Position begin = reader->position;
  char c = next(reader);
  if (c != '\"' && c != '\'') {
    reader->position = begin;
    return Result_NULL;
  }
  return lexStringEnd(reader, begin, c);
}



static Result parseFloat(StringReader *reader,
                         const char *string,
                         Position begin) {
  char *end;
  wsky_float value = (wsky_float) strtod(string, &end);

  if (value == 0.0 && string == end) {
    return createErrorResult("Invalid float number", begin);
  }
  const char *expectedEnd = strlen(string) + string;
  if (expectedEnd[-1] == 'f') { /* if the string ends with a 'f' */
    expectedEnd--;
  }
  if (end != expectedEnd) {
    return createErrorResult("Invalid float number", begin);
  }
  return createFloatTokenResult(reader, begin, value);
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
static wsky_int parseUintBase(const char *string, const char *baseChars) {
  wsky_int number = 0;
  int base = (int) strlen(baseChars);

  while (*string) {
    int digit = digitToInt(*string, baseChars);
    if (digit == -1)
      return -1;

    /* Overflow check */
    wsky_int previousNumber = number;
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

static Result parseNumber(StringReader *reader,
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
      length -= 2;
      break;
    case 'b':
      base = 2;
      string += 2;
      length -= 2;
      break;
    }
  }

  if (base == 10 && string[length - 1] == 'f') {
    return parseFloat(reader, string, begin);
  }

  if (!*string)
    return createErrorResult("Invalid number", begin);

  const char *baseChars = getNumberBaseChars(base);
  assert(baseChars);
  wsky_int v = parseUintBase(string, baseChars);
  if (v == -1)
    return createErrorResult("Invalid number", begin);
  return createIntTokenResult(reader, begin, v);
}

static bool isNextCharADigit(StringReader *reader) {
  if (!hasMore(reader)) {
    return false;
  }
  Position begin = reader->position;
  char c = next(reader);
  reader->position = begin;
  return isdigit(c);
}

#define MAX_NUMBER_LENGTH 64

static Result lexNumber(StringReader *reader) {
  Position begin = reader->position;
  char c = next(reader);
  if (!isdigit(c)) {
    reader->position = begin;
    return Result_NULL;
  }

  char buffer[MAX_NUMBER_LENGTH];
  buffer[0] = c;
  int numberLength = 1;
  bool dot = false;
  while (hasMore(reader)) {
    Position previous = reader->position;
    c = next(reader);

    if (!isNumberChar(c)) {
      reader->position = previous;
      break;
    }
    if (c == '.') {
      if (dot || !isNextCharADigit(reader)) {
        reader->position = previous;
        break;
      }
      dot = true;
    }
    if (numberLength >= MAX_NUMBER_LENGTH - 1) {
      return createErrorResult("Too long number", begin);
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

static Result lexIdentifier(StringReader *reader) {
  Position begin = reader->position;
  char c = next(reader);
  if (!isIdentifierStart(c)) {
    reader->position = begin;
    return Result_NULL;
  }

  while (hasMore(reader)) {
    Position previous = reader->position;
    c = next(reader);

    if (!isIdentifier(c)) {
      reader->position = previous;
      break;
    }
  }

  int length = reader->position.index - begin.index;
  char *string = wsky_safeMalloc((size_t) length + 1);
  strncpy(string, reader->string + begin.index, (size_t) length);
  string[length] = '\0';
  wsky_Keyword keyword;
  if (wsky_Keyword_parse(string, &keyword)) {
    wsky_free(string);
    return createTokenResult(reader, begin, wsky_TokenType_IDENTIFIER);
  }
  wsky_free(string);

  switch (keyword) {
# define CASE(name)                                                     \
    case wsky_Keyword_##name:                                           \
      return createOpTokenResult(reader, begin, wsky_Operator_##name)

    CASE(AND); CASE(OR); CASE(NOT);

# undef CASE
  default:
    break;
  }

  Token token = createToken(reader, begin, wsky_TokenType_KEYWORD);
  token.v.keyword = keyword;
  return createResultFromToken(token);
}



static wsky_Operator lexOperatorEq(char a, StringReader *reader) {
  Position previous = reader->position;

  bool equals = false;

  if (hasMore(reader)) {
    char b = next(reader);
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
  case '@': return wsky_Operator_AT;

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

static Result lexOperator(StringReader *reader) {

  Position begin = reader->position;
  char c = next(reader);

  switch (c) {
  case '!':
    if (hasMore(reader) && next(reader) == '=') {
      return createOpTokenResult(reader, begin, wsky_Operator_NOT_EQUALS);
    }
    reader->position = begin;
    return Result_NULL;

  case '=':
  case '-': case '+':
  case '*': case '/':
  case '<': case '>': {
    wsky_Operator op = lexOperatorEq(c, reader);
    return createOpTokenResult(reader, begin, op);
  }
  }
  wsky_Operator op = charToOperator(c);
  if (op == wsky_Operator_ASSIGN) {
    reader->position = begin;
    return Result_NULL;
  }
  return createOpTokenResult(reader, begin, op);
}



typedef Result (*LexerFunction)(StringReader *reader);



/**
 * @param functions A null-terminated array of function pointers
 */
static Result lexToken(StringReader *reader,
                       const LexerFunction *functions) {

  LexerFunction function = functions[0];
  int i = 0;
  while (function) {
    Position begin = reader->position;
    Result result = function(reader);
    if (result.type == ResultType_ERROR) {
      return result;
    }
    if (result.type == ResultType_TOKEN) {
      return result;
    }

    assert(begin.index == reader->position.index);

    i++;
    function = functions[i];
  }
  return Result_NULL;
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

  while (hasMore(reader)) {
    wsky_StringReader_skipWhitespaces(reader);
    if (!hasMore(reader))
      break;

    Result result = lexToken(reader, functions);

    if (result.type == ResultType_NULL) {
      if (autoStop)
        break;
      else
        result = createErrorResult("Unexpected character", reader->position);
    }

    if (result.type == ResultType_ERROR) {
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

static wsky_LexerResult lexFromFileAndString(wsky_ProgramFile *file,
                                             const char *string) {
  StringReader reader = wsky_StringReader_create(file, string);
  return wsky_lexFromReader(&reader, false);
}

wsky_LexerResult wsky_lexFromString(const char *string) {
  wsky_ProgramFile *file = wsky_ProgramFile_getUnknown();
  return lexFromFileAndString(file, string);
}

wsky_LexerResult wsky_lexFromFile(wsky_ProgramFile *file) {
  return lexFromFileAndString(file, file->content);
}



#define TEMPLATE_PRINT_BEGIN    "<%="
#define TEMPLATE_PRINT_END      "%>"
#define TEMPLATE_STMTS_BEGIN    "<%"
#define TEMPLATE_STMTS_END      "%>"

static Result lexWhiskeyInTemplate(StringReader *reader,
                                   const char *beginTag,
                                   const char *endTag,
                                   wsky_TokenType tokenType) {
  Position begin = reader->position;

  if (!wsky_StringReader_readString(reader, beginTag)) {
    return Result_NULL;
  }

  wsky_LexerResult lr = wsky_lexFromReader(reader, true);
  if (!lr.success) {
    return createResultFromError(lr.syntaxError);
  }

  if (!wsky_StringReader_readString(reader, endTag)) {
    return createErrorResult("Expected Whiskey closing tag", begin);
  }

  Token token = createToken(reader, begin, tokenType);
  token.v.children = lr.tokens;
  return createResultFromToken(token);
}

static Result lexWhiskeyPrint(StringReader *reader) {
  return lexWhiskeyInTemplate(reader,
                              TEMPLATE_PRINT_BEGIN,
                              TEMPLATE_PRINT_END,
                              wsky_TokenType_WSKY_PRINT);
}

/*
 * Lex Whiskey statements in a Whiskey template
 */
static Result lexWhiskeyStatements(StringReader *reader) {
  return lexWhiskeyInTemplate(reader,
                              TEMPLATE_STMTS_BEGIN,
                              TEMPLATE_STMTS_END,
                              wsky_TokenType_WSKY_STMTS);
}

/*
 * Lex a Whiskey template
 */
static Result lexHtml(StringReader *reader) {
  Position begin = reader->position;

  while (hasMore(reader)) {
    Position previous = reader->position;
    if (wsky_StringReader_readString(reader, TEMPLATE_STMTS_BEGIN)) {
      reader->position = previous;
      break;
    }
    if (wsky_StringReader_readString(reader, TEMPLATE_PRINT_BEGIN)) {
      reader->position = previous;
      break;
    }
    next(reader);
  }
  if (begin.index == reader->position.index)
    return Result_NULL;
  return createTokenResult(reader, begin, wsky_TokenType_HTML);
}



static void addTokenToTemplate(Token *token, TokenList **tokens) {
  if (token->type == wsky_TokenType_WSKY_STMTS) {
    wsky_TokenList_add(tokens, token->v.children);

    /*
     *'Manual' free. It's not very nice, but wet can't free
     * the children.
     */
    wsky_free(token->string);

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

  while (hasMore(reader)) {
    Result result = lexToken(reader, functions);

    if (result.type == ResultType_ERROR) {
      wsky_TokenList_delete(tokens);
      wsky_LexerResult lr = {
        .success = false,
        .syntaxError = result.syntaxError,
        .tokens = NULL,
      };
      return lr;
    }

    assert(result.type != ResultType_NULL);

    addTokenToTemplate(&result.token, &tokens);
  }

  wsky_LexerResult lr = {
    .success = true,
    .tokens = tokens,
  };
  return lr;
}

static wsky_LexerResult lexTemplate(wsky_ProgramFile *file, const char *string) {
  StringReader reader = wsky_StringReader_create(file, string);
  return wsky_lexTemplateFromReader(&reader);
}

wsky_LexerResult wsky_lexTemplateFromString(const char *string) {
  return lexTemplate(wsky_ProgramFile_getUnknown(), string);
}

wsky_LexerResult wsky_lexTemplateFromFile(wsky_ProgramFile *file) {
  return lexTemplate(file, file->content);
}
