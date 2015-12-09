#include "tests.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../program_file.h"



static void baseTests(void) {
  degat_ProgramFile *pf = degat_ProgramFile_new("str.c");
  yolo_assert_not_null(pf);
  if (!pf)
    return;
  degat_Object_DECREF(pf);
}

void programFileTestSuite(void) {
  baseTests();
}
