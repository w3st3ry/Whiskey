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

static void trigonometry(void) {
  assertEvalEq("1.0", "import math; math.cos(0)");
  assertEvalEq("-1.0", "import math; math.cos(math.PI)");
  assertEvalEq("true", "import math; var a = math.cos(math.PI / 2); a > -0.001 and a < 0.001");
  assertEvalEq("true", "import math; var a = math.cos(-math.PI / 2); a > -0.001 and a < 0.001");

  assertEvalEq("0.0", "import math; math.sin(0)");
  assertEvalEq("true", "import math; var a = math.sin(math.PI); a > -0.001 and a < 0.001");
  assertEvalEq("1.0", "import math; math.sin(math.PI / 2)");
  assertEvalEq("-1.0", "import math; math.sin(-math.PI / 2)");

  assertEvalEq("0.0", "import math; math.tan(0)");
  assertEvalEq("true", "import math; var a = math.tan(math.PI); a > -0.001 and a < 0.001");
  assertEvalEq("true", "import math; math.tan(math.PI / 2) > 99999999");
  assertEvalEq("true", "import math; math.tan(-math.PI / 2) < -99999999");
}

static void sign(void) {
  assertEvalEq("1.0", "import math; math.sign(math.PI)");
  assertEvalEq("1.0", "import math; math.sign(1)");
  assertEvalEq("1.0", "import math; math.sign(0.00001)");
  assertEvalEq("1.0", "import math; math.sign(1 - 1)");
  assertEvalEq("-1.0", "import math; math.sign(-1)");
  assertEvalEq("-1.0", "import math; math.sign(-math.PI)");
  assertEvalEq("-1.0", "import math; math.sign(math.PI - 2 * math.PI)");
}

void mathTestSuite(void) {
  assertEvalEq("<Module math>", "import math");

  assertEvalEq("true", "import math; math.PI > 3.141 and math.PI < 3.142");
  assertEvalEq("true", "import math; math.E > 2.718 and math.E < 2.719");

  toRadiansToDegrees();
  max();
  min();
  trigonometry();
  sign();

  wsky_GC_autoCollect();
}
