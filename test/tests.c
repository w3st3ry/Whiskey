#include "tests.h"
#include "../class.h"

int main() {
  yolo_begin();
  degat_init();

  strTestSuite();
  exceptionTestSuite();
  programFileTestSuite();

  degat_free();
  yolo_end();
}
