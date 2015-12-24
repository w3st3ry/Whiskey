#include "tests.h"

#include <string.h>
#include "class.h"
#include "gc.h"
#include "program_file.h"



static void baseTests(void) {
  wsky_ProgramFile *pf = wsky_ProgramFile_new("test/str.c");
  yolo_assert_not_null(pf);
  yolo_assert_ulong_neq(0, strlen(pf->content));
  yolo_assert_str_eq("str.c", pf->name);
  yolo_assert_str_eq("test/str.c", pf->path);
  if (!pf)
    return;

  wsky_GC_unmarkAll();
  wsky_GC_collect();
}

void programFileTestSuite(void) {
  baseTests();
}
