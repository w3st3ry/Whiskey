#include "tests.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../exception.h"

static void base(void) {
  degat_Exception *e = degat_Exception_new("yolo", NULL);
  yolo_assert_str_eq(e->message, "yolo");
  degat_Object_DECREF(e);
}

void exceptionTestSuite(void) {
  base();
}
