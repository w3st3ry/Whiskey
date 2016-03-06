#include "tests.h"
#include "gc.h"


static void toRadiansToDegrees(void) {
  assertEvalEq("0.0", "import math; math.toDegrees(0)");
  assertEvalEq("0.0", "import math; math.toDegrees(0.0)");
  assertEvalEq("0.0", "import math; math.toRadians(0)");
  assertEvalEq("0.0", "import math; math.toRadians(0.0)");
  assertEvalEq("0.0", "import math; math.toRadians(math.toDegrees(0))");
  assertEvalEq("123.0", "import math; math.toRadians(math.toDegrees(123))");
  assertEvalEq("123.0", "import math; math.toDegrees(math.toRadians(123))");
  assertEvalEq("123.0", "import math; math.toDegrees(math.toRadians(123.0))");
}

static void max(void) {
  assertEvalEq("1", "import math; math.max(1)");
  assertEvalEq("2", "import math; math.max(1, 2)");
  assertEvalEq("2", "import math; math.max(2, 1)");
  assertEvalEq("4", "import math; math.max(4, 3, 2, 1)");
  assertEvalEq("4", "import math; math.max(1, 2, 3, 4)");
  assertEvalEq("4.0", "import math; math.max(1, 2, 3, 4.0, 3.2)");
  assertEvalEq("4", "import math; math.max(1.0, 2.0, 3.0, 4, 3.5)");
}

static void min(void) {
  assertEvalEq("1", "import math; math.min(1)");
  assertEvalEq("-7", "import math; math.min(1, -7)");
  assertEvalEq("-7", "import math; math.min(-7, 1)");
  assertEvalEq("-7", "import math; math.min(-7, 1.0)");
}

void mathTestSuite(void) {
  assertEvalEq("<Module math>", "import math");

  assertEvalEq("true", "import math; math.PI > 3.141 and math.PI < 3.142");

  toRadiansToDegrees();
  max();
  min();

  wsky_GC_autoCollect();
}
