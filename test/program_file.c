#include "tests.h"

#include <string.h>
#include "gc.h"
#include "objects/program_file.h"

typedef wsky_ProgramFile ProgramFile;
typedef wsky_ReturnValue ReturnValue;

static void baseTests(void) {
  ReturnValue rv = wsky_ProgramFile_new("test/str.c");
  yolo_assert_null(rv.exception);
  ProgramFile *pf = (ProgramFile *)rv.v.v.objectValue;
  yolo_assert_not_null(pf);
  yolo_assert_ulong_neq(0, strlen(pf->content));
  yolo_assert_str_eq("str.c", pf->name);
  yolo_assert_str_eq("test/str.c", pf->path);
  if (!pf)
    return;

  wsky_GC_unmarkAll();
  wsky_GC_visitBuiltins();
  wsky_GC_collect();
}

void programFileTestSuite(void) {
  baseTests();
}
