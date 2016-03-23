#include "test.h"

#define mAssert(expectedString, source)                         \
  assertEvalEqImpl((expectedString), "import math;" source,     \
                   __func__, YOLO__POSITION_STRING)


static void toRadiansToDegrees(void) {
  mAssert("0.0", "math.toDegrees(0)");
  mAssert("0.0", "math.toDegrees(0.0)");
  mAssert("0.0", "math.toRadians(0)");
  mAssert("0.0", "math.toRadians(0.0)");
  mAssert("0.0", "math.toRadians(math.toDegrees(0))");
  mAssert("123.0", "math.toRadians(math.toDegrees(123))");
  mAssert("123.0", "math.toDegrees(math.toRadians(123))");
  mAssert("123.0", "math.toDegrees(math.toRadians(123.0))");
}

static void max(void) {
  mAssert("1", "math.max(1)");
  mAssert("2", "math.max(1, 2)");
  mAssert("2", "math.max(2, 1)");
  mAssert("4", "math.max(4, 3, 2, 1)");
  mAssert("4", "math.max(1, 2, 3, 4)");
  mAssert("4.0", "math.max(1, 2, 3, 4.0, 3.2)");
  mAssert("4", "math.max(1.0, 2.0, 3.0, 4, 3.5)");
}

static void min(void) {
  mAssert("1", "math.min(1)");
  mAssert("-7", "math.min(1, -7)");
  mAssert("-7", "math.min(-7, 1)");
  mAssert("-7", "math.min(-7, 1.0)");
}

static void trigonometry(void) {
  mAssert("1.0", "math.cos(0)");
  mAssert("-1.0", "math.cos(math.PI)");
  mAssert("true", "var a = math.cos(math.PI / 2); a > -0.001 and a < 0.001");
  mAssert("true", "var a = math.cos(-math.PI / 2); a > -0.001 and a < 0.001");

  mAssert("0.0", "math.sin(0)");
  mAssert("true", "var a = math.sin(math.PI); a > -0.001 and a < 0.001");
  mAssert("1.0", "math.sin(math.PI / 2)");
  mAssert("-1.0", "math.sin(-math.PI / 2)");

  mAssert("0.0", "math.tan(0)");
  mAssert("true", "var a = math.tan(math.PI); a > -0.001 and a < 0.001");
  mAssert("true", "math.tan(math.PI / 2) > 99999999");
  mAssert("true", "math.tan(-math.PI / 2) < -99999999");
}

static void sign(void) {
  mAssert("1.0", "math.sign(math.PI)");
  mAssert("1.0", "math.sign(1)");
  mAssert("1.0", "math.sign(0.00001)");
  mAssert("1.0", "math.sign(1 - 1)");
  mAssert("-1.0", "math.sign(-1)");
  mAssert("-1.0", "math.sign(-math.PI)");
  mAssert("-1.0", "math.sign(math.PI - 2 * math.PI)");
}

static void test_abs(void) {
  mAssert("3.14", "math.abs(3.14)");
  mAssert("3.14", "math.abs(-3.14)");
  mAssert("123", "math.abs(-123)");
  mAssert("123", "math.abs(123)");
  mAssert("0", "math.abs(0)");
  mAssert("0.0", "math.abs(0.0)");
}

void mathTestSuite(void) {
  assertEvalEq("<Module math>", "import math");

  mAssert("true", "math.PI > 3.141 and math.PI < 3.142");
  mAssert("true", "math.E > 2.718 and math.E < 2.719");

  toRadiansToDegrees();
  max();
  min();
  trigonometry();
  sign();
  test_abs();
}
