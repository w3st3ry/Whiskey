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
  wsky_Token token;

  r = wsky_lexFromString("'hello'");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);

  token = r.tokens->token;
  yolo_assert_str_eq("\'hello\'", token.string);
  yolo_assert(token.type = wsky_TokenType_STRING);
  yolo_assert_str_eq("hello", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);


  r = wsky_lexFromString("   \"hello\"  ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);

  token = r.tokens->token;
  yolo_assert_str_eq("\"hello\"", token.string);
  yolo_assert(token.type = wsky_TokenType_STRING);
  yolo_assert_str_eq("hello", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);


  r = wsky_lexFromString("  \"hello\' ");
  yolo_assert(!r.success);
  yolo_assert_null(r.tokens);
  yolo_assert_str_eq("Expected end of string", r.syntaxError.message);
  yolo_assert_int_eq(2, r.syntaxError.position.index);
  wsky_SyntaxError_free(&r.syntaxError);


  r = wsky_lexFromString(" \"ab\" \'c\' ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_not_null(r.tokens->next);
  yolo_assert_null(r.tokens->next->next);

  token = r.tokens->token;
  yolo_assert_str_eq("\"ab\"", token.string);
  yolo_assert(token.type = wsky_TokenType_STRING);
  yolo_assert_str_eq("ab", token.v.stringValue);

  token = r.tokens->next->token;
  yolo_assert_str_eq("\'c\'", token.string);
  yolo_assert(token.type = wsky_TokenType_STRING);
  yolo_assert_str_eq("c", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);
}

void lexerTestSuite(void) {
  basicTest();
  stringsTest();
}
