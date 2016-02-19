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
  assertEvalEq("0.0", "import math; math.toDegrees(0)");
  assertEvalEq("0.0", "import math; math.toDegrees(0.0)");
  assertEvalEq("0.0", "import math; math.toRadians(0)");
  assertEvalEq("0.0", "import math; math.toRadians(0.0)");
  assertEvalEq("0.0", "import math; math.toRadians(math.toDegrees(0))");
  assertEvalEq("123.0", "import math; math.toRadians(math.toDegrees(123))");
  assertEvalEq("123.0", "import math; math.toDegrees(math.toRadians(123))");
  assertEvalEq("123.0", "import math; math.toDegrees(math.toRadians(123.0))");

  wsky_GC_unmarkAll();
  wsky_GC_visitBuiltins();
  wsky_GC_collect();
}
