#include "tests.h"
#include "whiskey.h"

int main() {
  yolo_begin();
  wsky_start();

  dictTestSuite();
  strTestSuite();
  exceptionTestSuite();
  programFileTestSuite();
  stringReaderTestSuite();
  lexerTestSuite();
  parserTestSuite();
  evalTestSuite();
  mathTestSuite();

  wsky_stop();
  yolo_end();

  return (0);
}
