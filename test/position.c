#include "test.h"
#include "whiskey.h"

void positionTestSuite(void) {
  wsky_StringReader r = wsky_StringReader_createFromString("Hello");
  wsky_Position pos = r.position;
  yolo_assert(wsky_Position_equals(&pos, &pos));
  wsky_StringReader_free(&r);
}
