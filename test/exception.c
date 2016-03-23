#include "test.h"

#include <string.h>
#include "whiskey.h"


static void base(void) {
  wsky_Exception *e = wsky_Exception_new("yolo", NULL);
  yolo_assert_str_eq(e->message, "yolo");
}


void exceptionTestSuite(void) {
  base();
}
