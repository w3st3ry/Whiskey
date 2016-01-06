#include "tests.h"

#include <string.h>
#include "gc.h"
#include "objects/exception.h"


static void base(void) {
  wsky_Exception *e = wsky_Exception_new("yolo", NULL);
  yolo_assert_str_eq(e->message, "yolo");

  wsky_GC_unmarkAll();
  wsky_GC_visitBuiltins();
  wsky_GC_collect();
}


void exceptionTestSuite(void) {
  base();
}
