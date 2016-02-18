#include "tests.h"
#include "gc.h"


void mathTestSuite(void) {
  assertEvalEq("<Module math>", "import math");

  assertException("SyntaxError", "Expected module name",
                  "import");

  assertException("ImportError", "No module named 'barfoofoobar'",
                  "import barfoofoobar");

  assertException("NameError", "Use of undeclared identifier 'math'",
                  "math");

  assertEvalEq("true", "import math; math.PI > 3.141 and math.PI < 3.142");

  wsky_GC_unmarkAll();
  wsky_GC_visitBuiltins();
  wsky_GC_collect();
}
