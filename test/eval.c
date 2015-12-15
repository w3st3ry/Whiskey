#include "tests.h"

#include <stdlib.h>
#include "eval.h"
#include "exception.h"
#include "gc.h"



# define assertEvalEq(expectedAstString, source)		\
  assertEvalEqImpl((expectedAstString), (source),		\
		   __func__, YOLO__POSITION_STRING)

static void assertEvalEqImpl(const char *expected,
			 const char *source,
			 const char *testName,
			 const char *position) {

  wsky_ReturnValue r = wsky_evalString(source);
  yolo_assert_ptr_eq_impl(NULL, r.exception, testName, position);
  if (r.exception) {
    printf("%s\n", r.exception->message);
    wsky_DECREF(r.exception);
    return;
  }
  char *string = wsky_Value_toCString(r.v);
  yolo_assert_str_eq_impl(expected, string, testName, position);
  free(string);
  wsky_Value_DECREF(r.v);
}

static void literals(void) {
  assertEvalEq("123","123");

  assertEvalEq("123.0","123.0");
  assertEvalEq("123.456","123.456");
  assertEvalEq("1e+23","100000000000000000000000.0");
  /* assertEvalEq("1e+23","100000000000000000000000f"); */

  assertEvalEq("lol","'lol'");
}

static void binaryOps(void) {
  assertEvalEq("2", "1 + 1");
  assertEvalEq("20", "4 * 5");
  assertEvalEq("-1", "4 - 5");
  assertEvalEq("113", "567 / 5");

  assertEvalEq("2.0", "1 + 1.0");
  assertEvalEq("20.0", "4 * 5.0");
  assertEvalEq("-1.0", "4 - 5.0");
  assertEvalEq("113.4", "567 / 5.0");

  assertEvalEq("2.0", "1.0 + 1");
  assertEvalEq("20.0", "4.0 * 5");
  assertEvalEq("-1.0", "4.0 - 5");
  assertEvalEq("113.4", "567.0 / 5");

  assertEvalEq("2.0", "1.0 + 1.0");
  assertEvalEq("20.0", "4.0 * 5.0");
  assertEvalEq("-1.0", "4.0 - 5.0");
  assertEvalEq("113.4", "567.0 / 5.0");
}

static void binaryCmpOps(void) {
  assertEvalEq("false", "567 == 56");
  assertEvalEq("true", "567 == 567");
  assertEvalEq("false", "567 != 567");
  assertEvalEq("true", "567 != 566");
  assertEvalEq("true", "567 != 568");

  assertEvalEq("false", "566 > 566");
  assertEvalEq("true", "567 > 566");
  assertEvalEq("false", "566 > 567");

  assertEvalEq("false", "566.0 > 566");
  assertEvalEq("true", "567.0 > 566");
  assertEvalEq("false", "566.0 > 567");

  assertEvalEq("false", "566 > 566.0");
  assertEvalEq("true", "567 > 566.0");
  assertEvalEq("false", "566 > 567.0");

  assertEvalEq("false", "566.0 > 566.0");
  assertEvalEq("true", "567.0 > 566.0");
  assertEvalEq("false", "566.0 > 567.0");

  assertEvalEq("true", "566 >= 566");
  assertEvalEq("true", "567 >= 566");
  assertEvalEq("false", "566 >= 567");

  assertEvalEq("false", "566 < 566");
  assertEvalEq("false", "567 < 566");
  assertEvalEq("true", "566 < 567");

  assertEvalEq("true", "566 <= 566");
  assertEvalEq("false", "567 <= 566");
  assertEvalEq("true", "566 <= 567");
}

static void sequence(void) {
  assertEvalEq("12", "(12)");
  assertEvalEq("12", "(12;)");
  assertEvalEq("12", "(678;12;)");
}

void evalTestSuite(void) {
  literals();
  binaryOps();
  binaryCmpOps();
  sequence();
}
