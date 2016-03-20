#include "tests.h"

#include <string.h>
#include "memory.h"
#include "objects/exception.h"


static void base(void) {
  wsky_Exception *e = wsky_Exception_new("yolo", NULL);
  yolo_assert_str_eq(e->message, "yolo");
}


void exceptionTestSuite(void) {
  base();
}
