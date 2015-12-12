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
  yolo_assert(token.type == wsky_TokenType_STRING);
  yolo_assert_str_eq("hello", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);


  r = wsky_lexFromString("   \"hello\"  ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);

  token = r.tokens->token;
  yolo_assert_str_eq("\"hello\"", token.string);
  yolo_assert(token.type == wsky_TokenType_STRING);
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
  yolo_assert(token.type == wsky_TokenType_STRING);
  yolo_assert_str_eq("ab", token.v.stringValue);

  token = r.tokens->next->token;
  yolo_assert_str_eq("\'c\'", token.string);
  yolo_assert(token.type == wsky_TokenType_STRING);
  yolo_assert_str_eq("c", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);
}

static void integersTest(void) {
  wsky_LexerResult r;
  wsky_Token token;

  r = wsky_lexFromString(" 0 ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("0", token.string);
  yolo_assert(token.type == wsky_TokenType_INT);
  yolo_assert_long_eq(0, (long)token.v.intValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" 9 ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("9", token.string);
  yolo_assert(token.type == wsky_TokenType_INT);
  yolo_assert_long_eq(9, (long)token.v.intValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" 00123 ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("00123", token.string);
  yolo_assert(token.type == wsky_TokenType_INT);
  yolo_assert_long_eq(123, (long)token.v.intValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" 0x123f ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("0x123f", token.string);
  yolo_assert(token.type == wsky_TokenType_INT);
  yolo_assert_long_eq(0x123f, (long)token.v.intValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" 0x123fa601 ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("0x123fa601", token.string);
  yolo_assert(token.type == wsky_TokenType_INT);
  yolo_assert_long_eq(0x123fa601, (long)token.v.intValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" 0b1 ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("0b1", token.string);
  yolo_assert(token.type == wsky_TokenType_INT);
  yolo_assert_long_eq(1, (long)token.v.intValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" 0b10010001111111010011000000001 ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("0b10010001111111010011000000001", token.string);
  yolo_assert(token.type == wsky_TokenType_INT);
  yolo_assert_long_eq(0x123fa601, (long)token.v.intValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString("  0b123 ");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Invalid number", r.syntaxError.message);
  yolo_assert_int_eq(2, r.syntaxError.position.index);
  wsky_SyntaxError_free(&r.syntaxError);

  /*
  r = wsky_lexFromString("  56.8.89 ");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Invalid number", r.syntaxError.message);
  yolo_assert_int_eq(2, r.syntaxError.position.index);
  wsky_SyntaxError_free(&r.syntaxError);
  */
}

void lexerTestSuite(void) {
  basicTest();
  stringsTest();
  integersTest();
}
