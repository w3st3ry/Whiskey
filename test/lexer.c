#include "tests.h"

#include "../lexer.h"

static void basicTest(void) {
  wsky_LexerResult r = wsky_lexFromString("");
  yolo_assert(r.success);
  yolo_assert_null(r.tokens);

  r = wsky_lexFromString("#");
  yolo_assert(!r.success);
  yolo_assert_null(r.tokens);
  yolo_assert_str_eq("Unexpected token", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);
}

static void stringsTest(void) {
  wsky_LexerResult r;

  r = wsky_lexFromString("'hello'");

  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  wsky_Token token = r.tokens->token;
  yolo_assert_str_eq("\'hello\'", token.string);
  yolo_assert(token.type = wsky_TokenType_STRING);
  yolo_assert_str_eq("hello", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);
}

void lexerTestSuite(void) {
  basicTest();
  stringsTest();
}
