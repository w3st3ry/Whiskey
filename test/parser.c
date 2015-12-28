#include "tests.h"

#include <stdlib.h>
#include "parser.h"
#include "lexer.h"
#include "gc.h"



# define assertAstEq(expectedAstString, source)                 \
  assertAstEqualsImpl((expectedAstString), (source),            \
                      __func__, YOLO__POSITION_STRING, false)

# define assertTpltAstEq(expectedAstString, source)             \
  assertAstEqualsImpl((expectedAstString), (source),            \
                      __func__, YOLO__POSITION_STRING, true)

# define assertSyntaxError(expectedMsg, source)                 \
  assertSyntaxErrorImpl((expectedMsg), (source),                \
                        __func__, YOLO__POSITION_STRING)


static void assertAstEqualsImpl(const char *expectedAstString,
                                const char *source,
                                const char *testName,
                                const char *position,
                                bool template) {

  wsky_ParserResult pr = template ?
    wsky_parseTemplateString(source) : wsky_parseString(source);

  if (!pr.success) {
    yolo_fail_impl(testName, position);
    wsky_SyntaxError_print(&pr.syntaxError, stdout);
    wsky_SyntaxError_free(&pr.syntaxError);
    printf("\n");
    return;
  }
  char *astString = wsky_ASTNode_toString(pr.node);
  yolo_assert_str_eq_impl(expectedAstString, astString, testName, position);
  wsky_FREE(astString);
  wsky_ASTNode_delete(pr.node);
}

static void assertSyntaxErrorImpl(const char *expectedMessage,
                                  const char *source,
                                  const char *testName,
                                  const char *position) {

  wsky_ParserResult pr = wsky_parseString(source);

  if (!pr.success) {
    yolo_assert_str_eq_impl(expectedMessage, pr.syntaxError.message,
                            testName, position);
    wsky_SyntaxError_free(&pr.syntaxError);
  } else {
    yolo_fail_impl(testName, position);
    wsky_ASTNode_print(pr.node, stderr);
    printf("\n");
    wsky_ASTNode_delete(pr.node);
  }
}



static void literals(void) {
  assertSyntaxError("Unexpected end of file", "");

  assertAstEq("hello", "hello");
  assertAstEq("'hello'", "'hello'");
  assertAstEq("'\\\"'", "'\"'");
  assertAstEq("255", "0xff");
  assertAstEq("8.45", "0008.4500");
  assertAstEq("8.45432", "0008.4543200f");
  assertSyntaxError("Invalid float number", "2882.34.2");
  assertSyntaxError("Invalid float number", "45678903456789086432345678"
                    "900975434567288234567896423456789523456789234567890234567890123456789.34");
  assertSyntaxError("Invalid float number", "0.");
  assertAstEq("8.45.", "0008.4500f");
}

static void unary(void) {
  assertAstEq("(- 1)", "   -   1  ");
  assertAstEq("(- (+ (+ 1)))", "-++1");

  assertAstEq("(not true)", "not true");

  assertAstEq("true.getClass()", "true.getClass()");

  assertSyntaxError("Unexpected end of file", "-");
  assertSyntaxError("Unexpected '*'", "*");
}

static void binary(void) {
  assertSyntaxError("Unexpected end of file", "3 *");
  assertSyntaxError("Unexpected '/'", "3 * /");

  assertAstEq("(((- (- 6)) + (- (+ 5))) - (- 4))",
              "--6+-+5--4");
  assertAstEq("(((- 3) * (- 4)) + ((- lol) * poney))",
              "-3 * -4 + - lol * poney");
}

static void equals(void) {
  assertAstEq("(6 == 3)",
              "6 == 3");
  assertAstEq("(6 != 3)",
              "6 != 3");
}

static void comparison(void) {
  assertAstEq("(6 < 3)",
              "6 < 3");
  assertAstEq("(6 <= 3)",
              "6 <= 3");
  assertAstEq("(6 > 3)",
              "6 > 3");
  assertAstEq("(6 >= 3)",
              "6 >= 3");
}

static void sequence(void) {
  assertAstEq("()", "()");
  assertAstEq("(((())))", "(((())))");
  assertSyntaxError("Expected ')'", "(");
  assertSyntaxError("Expected ')'", "( yolo");
  assertSyntaxError("Expected ')'", "( yolo;");
  assertSyntaxError("Unexpected ';'", "(;)");
  assertSyntaxError("Unexpected ')'", ")");
  assertAstEq("(yolo)", "(yolo)");
  assertAstEq("(yolo)", "(yolo;)");
  assertAstEq("(l; ISEG; c; est; pourri)", "(l;ISEG; c;est; pourri)");
  assertAstEq("(l; ESME; c; est; pourri)", "(l;ESME; c;est; pourri;)");
  assertSyntaxError("Expected ';' or ')'", "(manque;une virgule)");
  assertSyntaxError("Unexpected ';'", "(a; b;; c)");

  assertAstEq("(6 * ((7 + 1)))", "6 * (7 + 1)");
  assertAstEq("(((6 - 7)) / 1)", "(6 - 7) / 1");
}

static void function(void) {
  assertAstEq("{}", "{}");
  assertAstEq("{}", "{ : }");
  assertAstEq("{vodka}", "{    vodka  }");
  assertAstEq("{vodka}", "{vodka;}");
  assertAstEq("{vodka; shooter}", "{vodka; shooter}");
  assertAstEq("{vodka; shooter}", "{vodka; shooter;}");
  assertAstEq("{ rhum : }", "{ rhum : }");
  assertAstEq("{ rhum : }", "{ rhum, : }");
  assertAstEq("{ rhum, vodka : }", "{ rhum, vodka : }");
  assertAstEq("{ rhum, vodka : ricard}", "{ rhum, vodka : ricard}");
  assertAstEq("{ rhum, vodka : ricard}", "{ rhum, vodka : ricard;}");
  assertAstEq("{ rhum, vodka : a; b}", "{ rhum, vodka : a; b;}");

  assertSyntaxError("Expected '}'", "{");
  assertSyntaxError("Expected '}'", "{ :");
}

static void call(void) {
  assertAstEq("foo()", "foo()");
  assertAstEq("foo()()", "foo()()");
  assertAstEq("foo(34)", "foo(34)");
  assertAstEq("foo(34)", "foo(34,)");
  assertAstEq("foo(34, 6, bar)", "foo(34, 6, bar)");
}

static void template(void) {
  assertTpltAstEq("HTML( <html> )", " <html> ");
  assertTpltAstEq("((6 * 5); HTML( yolo ))",
                  "<% (6 * 5; %> yolo <% ) %>");
}

static void var(void) {
  assertSyntaxError("Expected variable name", "var");
  assertSyntaxError("Expected variable name", "var 'lol'");
  assertSyntaxError("Unexpected end of file", "var a =");
  assertAstEq("var a", "var a");
  assertAstEq("var a = 56", "var a = 56");

  assertSyntaxError("Unexpected end of file", "a =");
  //assertSyntaxError("not assignable", "123 = 456");
}

void parserTestSuite(void) {
  literals();
  unary();
  binary();
  equals();
  comparison();
  sequence();
  function();
  call();
  template();
  var();
}
