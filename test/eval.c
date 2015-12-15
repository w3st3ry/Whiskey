#include "tests.h"

#include <stdlib.h>
#include "eval.h"
#include "exception.h"



# define assertEvalEq(expectedAstString, source)		\
  assertEvalEqImpl((expectedAstString), (source),		\
		   __func__, YOLO__POSITION_STRING)

static void assertEvalEqImpl(const char *expected,
			 const char *source,
			 const char *testName,
			 const char *position) {

  wsky_ReturnValue r = wsky_evalString(source);
  yolo_assert_ptr_eq_impl(NULL, r.exception, testName, position);
  if (r.exception)
    return;
  char *string = wsky_Value_toCString(r.v);
  yolo_assert_str_eq_impl(expected, string, testName, position);
  free(string);
}



void evalTestSuite(void) {
  assertEvalEq("123","123");

  assertEvalEq("123.0","123.0");
  assertEvalEq("123.456","123.456");
  assertEvalEq("1e+23","100000000000000000000000.0");
  /* assertEvalEq("1e+23","100000000000000000000000f"); */
}
