#include "tests.h"

#include <stdlib.h>
#include "../parser.h"
#include "../lexer.h"



# define assertAstEq(expectedAstString, source)			\
  assertAstEqualsImpl((expectedAstString), (source),		\
		      __func__, YOLO__POSITION_STRING)


# define assertSyntaxError(expectedMsg, source)				\
  assertSyntaxErrorImpl((expectedMsg), (source),			\
			__func__, YOLO__POSITION_STRING)


static void assertAstEqualsImpl(const char *expectedAstString,
				const char *source,
				const char *testName,
				const char *position) {

  wsky_TokenList *tokens = NULL;
  wsky_ParserResult pr = wsky_parseString(source, &tokens);

  if (!pr.success) {
    yolo_fail_impl(testName, position);
    wsky_SyntaxError_print(&pr.syntaxError, stdout);
    wsky_SyntaxError_free(&pr.syntaxError);
    printf("\n");
    return;
  }
  char *astString = wsky_ASTNode_toString(pr.node);
  yolo_assert_str_eq_impl(expectedAstString, astString, testName, position);
  free(astString);
  wsky_ASTNode_delete(pr.node);
  wsky_TokenList_delete(tokens);
}

static void assertSyntaxErrorImpl(const char *expectedMessage,
				  const char *source,
				  const char *testName,
				  const char *position) {

  wsky_TokenList *tokens = NULL;
  wsky_ParserResult pr = wsky_parseString(source, &tokens);

  if (!pr.success) {
    yolo_assert_str_eq_impl(expectedMessage, pr.syntaxError.message,
			    testName, position);
    wsky_SyntaxError_free(&pr.syntaxError);
  } else {
    yolo_fail_impl(testName, position);
    wsky_ASTNode_delete(pr.node);
    wsky_TokenList_delete(tokens);
  }
}



static void literals(void) {
  assertSyntaxError("Unexpected end of file", "");

  assertAstEq("hello", "hello");
  assertAstEq("'hello'", "'hello'");
  assertAstEq("'\\\"'", "'\"'");
  assertAstEq("255", "0xff");
  assertAstEq("8.45", "0008.4500");
}

static void unary(void) {
  assertAstEq("(-1)", "   -   1  ");
  assertAstEq("(-(+(+1)))", "-++1");
  assertSyntaxError("Unexpected end of file", "-");
  assertSyntaxError("Unexpected '*'", "*");
}

static void binary(void) {
  assertSyntaxError("Unexpected end of file", "3 *");
  assertSyntaxError("Unexpected '/'", "3 * /");

  assertAstEq("(((-(-6)) + (-(+5))) - (-4))",
	      "--6+-+5--4");
  assertAstEq("(((-3) * (-4)) + ((-lol) * poney))",
	      "-3 * -4 + - lol * poney");
}

void parserTestSuite(void) {
  literals();
  unary();
  binary();
}
