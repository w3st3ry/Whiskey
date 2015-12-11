#include "tests.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../program_file.h"



static void baseTests(void) {
  wsky_ProgramFile *pf = wsky_ProgramFile_new("str.c");
  yolo_assert_not_null(pf);
  yolo_assert_ulong_neq(0, strlen(pf->content));
  yolo_assert_str_eq("str.c", pf->name);
  yolo_assert_str_eq("str.c", pf->path);
  if (!pf)
    return;
  wsky_DECREF(pf);
}

void programFileTestSuite(void) {
  baseTests();
}
