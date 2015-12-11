#include "tests.h"
#include "../class.h"

int main() {
  yolo_begin();
  degat_init();

  strTestSuite();
  exceptionTestSuite();
  programFileTestSuite();
  stringReaderTestSuite();

  degat_free();
  yolo_end();
}
