#include "tests.h"

#include <string.h>
#include "gc.h"
#include "objects/program_file.h"
#include "path.h"

typedef wsky_ProgramFile ProgramFile;
typedef wsky_ReturnValue ReturnValue;

static void baseTests(void) {
  char *filePath = getLocalFilePath("eval.c");
  ReturnValue rv = wsky_ProgramFile_new(filePath);
  wsky_free(filePath);
  yolo_assert_null(rv.exception);
  ProgramFile *pf = (ProgramFile *)rv.v.v.objectValue;
  yolo_assert_not_null(pf);
  yolo_assert_ulong_neq(0, strlen(pf->content));
  yolo_assert_str_eq("eval.c", pf->name);
  if (!pf)
    return;

  wsky_GC_autoCollect();
}

void programFileTestSuite(void) {
  baseTests();
}
