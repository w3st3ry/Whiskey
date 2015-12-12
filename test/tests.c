#include "tests.h"
#include "../class.h"

int main() {
  yolo_begin();
  wsky_init();

  strTestSuite();
  exceptionTestSuite();
  programFileTestSuite();
  stringReaderTestSuite();
  lexerTestSuite();

  wsky_free();
  yolo_end();

  return (0);
}
