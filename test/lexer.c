#include "test.h"

#include <stdlib.h>
#include "whiskey.h"


static void basicTest(void) {
  wsky_LexerResult r = wsky_lexFromString("");
  yolo_assert(r.success);
  yolo_assert_null(r.tokens);

  r = wsky_lexFromString("#");
  yolo_assert(!r.success);
  yolo_assert_null(r.tokens);
  yolo_assert_str_eq("Unexpected character '#'", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);
}


static void string(void) {
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

  r = wsky_lexFromString("'\\'");
  yolo_assert(!r.success);
  yolo_assert_null(r.tokens);
  yolo_assert_str_eq("Expected end of string", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);

  r = wsky_lexFromString("'\\");
  yolo_assert(!r.success);
  yolo_assert_null(r.tokens);
  yolo_assert_str_eq("Expected escape sequence and end of string",
                     r.syntaxError.message);
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


static void stringEscape(void) {
  wsky_LexerResult r;
  wsky_Token token;

  r = wsky_lexFromString("'\\n\\r\\t\\b'");
  yolo_assert(r.success);
  token = r.tokens->token;
  yolo_assert(token.type == wsky_TokenType_STRING);
  yolo_assert_str_eq("\n\r\t\b", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString("'\\xaa\\xAA\\xfF\\x01\\x10'");
  yolo_assert(r.success);
  token = r.tokens->token;
  yolo_assert(token.type == wsky_TokenType_STRING);
  yolo_assert_str_eq("\xaa\xAA\xfF\x01\x10", token.v.stringValue);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString("'\\z'");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Invalid escape sequence", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);

  r = wsky_lexFromString("'\\x");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Invalid escape sequence", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);

  r = wsky_lexFromString("'\\x'");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Invalid escape sequence", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);

  r = wsky_lexFromString("'\\xa");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Invalid escape sequence", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);

  r = wsky_lexFromString("'\\xa'");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Expected end of string", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);
}


static void integer(void) {
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
  if (r.success) {
    yolo_assert_not_null(r.tokens);
    yolo_assert_null(r.tokens->next);
    token = r.tokens->token;
    yolo_assert_str_eq("0x123fa601", token.string);
    yolo_assert(token.type == wsky_TokenType_INT);
    yolo_assert_long_eq(0x123fa601, (long)token.v.intValue);
    wsky_TokenList_delete(r.tokens);
  } else {
    wsky_SyntaxError_free(&r.syntaxError);
  }

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
  if (r.success) {
    yolo_assert_not_null(r.tokens);
    yolo_assert_null(r.tokens->next);
    token = r.tokens->token;
    yolo_assert_str_eq("0b10010001111111010011000000001", token.string);
    yolo_assert(token.type == wsky_TokenType_INT);
    yolo_assert_long_eq(0x123fa601, (long)token.v.intValue);
    wsky_TokenList_delete(r.tokens);
  } else {
    wsky_SyntaxError_free(&r.syntaxError);
  }

  r = wsky_lexFromString("  0b123 ");
  yolo_assert(!r.success);
  yolo_assert_str_eq("Invalid number", r.syntaxError.message);
  yolo_assert_int_eq(2, r.syntaxError.position.index);
  wsky_SyntaxError_free(&r.syntaxError);
}

static void floatTest(void) {
  wsky_LexerResult r;

  r = wsky_lexFromString("12.34.56");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  char *string = wsky_TokenList_toString(r.tokens);
  wsky_TokenList_delete(r.tokens);
  yolo_assert_str_eq("{type: FLOAT; string: 12.34}"
                     "{type: OPERATOR; string: .}"
                     "{type: INT; string: 56}",
                     string);
  wsky_free(string);
}

static void identifiersTest(void) {
  wsky_LexerResult r;
  wsky_Token token;

  r = wsky_lexFromString(" _ ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("_", token.string);
  yolo_assert(token.type == wsky_TokenType_IDENTIFIER);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" y7J__00123_ ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("y7J__00123_", token.string);
  yolo_assert(token.type == wsky_TokenType_IDENTIFIER);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" Z ");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("Z", token.string);
  yolo_assert(token.type == wsky_TokenType_IDENTIFIER);
  wsky_TokenList_delete(r.tokens);
}

static void commentsTest(void) {
  wsky_LexerResult r;
  wsky_Token token;

  r = wsky_lexFromString("/**/");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("/**/", token.string);
  yolo_assert(token.type == wsky_TokenType_COMMENT);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString("/**");
  yolo_assert_str_eq("Expected */", r.syntaxError.message);
  wsky_SyntaxError_free(&r.syntaxError);

  r = wsky_lexFromString(" //yolo yolo\n");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("//yolo yolo", token.string);
  yolo_assert(token.type == wsky_TokenType_COMMENT);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" //yolo yolo");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("//yolo yolo", token.string);
  yolo_assert(token.type == wsky_TokenType_COMMENT);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" // /*yolo*/\n");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("// /*yolo*/", token.string);
  yolo_assert(token.type == wsky_TokenType_COMMENT);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" // 'lol'\n");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("// 'lol'", token.string);
  yolo_assert(token.type == wsky_TokenType_COMMENT);
  wsky_TokenList_delete(r.tokens);

  r = wsky_lexFromString(" '// lol'\n");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  yolo_assert_null(r.tokens->next);
  token = r.tokens->token;
  yolo_assert_str_eq("'// lol'", token.string);
  yolo_assert(token.type == wsky_TokenType_STRING);
  wsky_TokenList_delete(r.tokens);
}

static void operatorsTest(void) {
  wsky_LexerResult r;

  r = wsky_lexFromString(" +=--=*===");
  yolo_assert(r.success);
  yolo_assert_not_null(r.tokens);
  char *string = wsky_TokenList_toString(r.tokens);
  wsky_TokenList_delete(r.tokens);
  yolo_assert_str_eq("{type: OPERATOR; string: +=}"
                     "{type: OPERATOR; string: -}"
                     "{type: OPERATOR; string: -=}"
                     "{type: OPERATOR; string: *=}"
                     "{type: OPERATOR; string: ==}",
                     string);
  wsky_free(string);
}

static void multiTest(void) {
  wsky_LexerResult r;

  r = wsky_lexFromString("123//4\n5;//");
  yolo_assert(r.success);
  char *string = wsky_TokenList_toString(r.tokens);
  wsky_TokenList_delete(r.tokens);
  yolo_assert_str_eq("{type: INT; string: 123}"
                     "{type: COMMENT; string: //4}"
                     "{type: INT; string: 5}"
                     "{type: OPERATOR; string: ;}"
                     "{type: COMMENT; string: //}",
                     string);
  wsky_free(string);
}



static void template0(void) {
  wsky_LexerResult r;

  r = wsky_lexTemplateFromString("  yolo ");
  yolo_assert(r.success);
  char *string = wsky_TokenList_toString(r.tokens);
  wsky_TokenList_delete(r.tokens);
  yolo_assert_str_eq("{type: HTML; string:   yolo }",
                     string);
  wsky_free(string);
}

static void template1(void) {
  wsky_LexerResult r;

  r = wsky_lexTemplateFromString("<% yolo %><%= yolo %>");
  yolo_assert(r.success);
  char *string = wsky_TokenList_toString(r.tokens);
  wsky_TokenList_delete(r.tokens);
  yolo_assert_str_eq("{type: IDENTIFIER; string: yolo}"
                     "{type: WSKY_PRINT; string: <%= yolo %>}",
                     string);
  wsky_free(string);
}

static void template2(void) {
  wsky_LexerResult r;

  char *html = "<html><%= 1 + '2' %></html>";

  r = wsky_lexTemplateFromString(html);
  yolo_assert(r.success);
  char *templateString = wsky_TokenList_toString(r.tokens);

  wsky_Token *whiskeyToken = &r.tokens->next->token;
  char *whiskeyString = wsky_TokenList_toString(whiskeyToken->v.children);
  wsky_TokenList_delete(r.tokens);

  yolo_assert_str_eq("{type: HTML; string: <html>}"
                     "{type: WSKY_PRINT; string: <%= 1 + '2' %>}"
                     "{type: HTML; string: </html>}",
                     templateString);
  wsky_free(templateString);

  yolo_assert_str_eq("{type: INT; string: 1}"
                     "{type: OPERATOR; string: +}"
                     "{type: STRING; string: '2'}",
                     whiskeyString);
  wsky_free(whiskeyString);
}

void lexerTestSuite(void) {
  basicTest();
  string();
  stringEscape();
  integer();
  floatTest();
  identifiersTest();
  commentsTest();
  operatorsTest();
  multiTest();
  template0();
  template1();
  template2();
}
