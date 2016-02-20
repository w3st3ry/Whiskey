#include "tests.h"

#include "whiskey.h"
#include "eval.h"
#include "objects/exception.h"

typedef wsky_ReturnValue ReturnValue;

static void runWhiskeyTests(void) {
  ReturnValue rv = wsky_evalFile("test/tests.wsky");
  if (rv.exception) {
    wsky_Exception_print(rv.exception);
  }
}

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

  runWhiskeyTests();

  wsky_stop();
  yolo_end();

  return (0);
}
