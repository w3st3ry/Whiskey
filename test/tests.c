#include "tests.h"
#include "class.h"
#include <gc.h>

int main() {
  GC_INIT();

  yolo_begin();
  wsky_init();

  dictTestSuite();
  strTestSuite();
  exceptionTestSuite();
  programFileTestSuite();
  stringReaderTestSuite();
  lexerTestSuite();
  parserTestSuite();
  evalTestSuite();

  wsky_free();
  yolo_end();

  return (0);
}
