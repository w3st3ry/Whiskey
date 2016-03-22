#include "test.h"
#include "whiskey.h"

void positionTestSuite(void) {
  yolo_assert(wsky_Position_isUnknown(&wsky_Position_UNKNOWN));
  wsky_ProgramFile *file = wsky_ProgramFile_getUnknown();
  wsky_StringReader r = wsky_StringReader_create(file, "Hello");
  wsky_Position pos = r.position;
  yolo_assert(wsky_Position_equals(&pos, &pos));
  wsky_StringReader_free(&r);
}
