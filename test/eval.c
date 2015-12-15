#include "tests.h"

#include "eval.h"
#include "exception.h"

void evalTestSuite(void) {
  wsky_ReturnValue r;

  r = wsky_evalString("123");
  yolo_assert_null(r.exception);
  yolo_assert(r.v.type == wsky_Type_INT);
  yolo_assert(r.v.v.intValue == 123);

  r = wsky_evalString("123.0");
  yolo_assert_null(r.exception);
  yolo_assert(r.v.type == wsky_Type_FLOAT);
  yolo_assert(r.v.v.floatValue == 123.0);
}
