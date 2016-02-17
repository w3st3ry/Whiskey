#include "tests.h"
#include "gc.h"


void mathTestSuite(void) {
  assertEvalEq("3.14159", "math.PI");

  wsky_GC_unmarkAll();
  wsky_GC_visitBuiltins();
  wsky_GC_collect();
}
