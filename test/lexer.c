#include "tests.h"

#include "../lexer.h"

static void basicTests(void) {
  wsky_LexerResult r = wsky_lexFromString("");
  yolo_assert_int_eq(true, r.success);
  yolo_assert_null(r.tokens);
}


void lexerTestSuite(void) {
  basicTests();
}
