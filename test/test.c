#include "test.h"

#include "whiskey.h"

typedef wsky_ReturnValue ReturnValue;

char *getLocalFilePath(const char *fileName) {
  char *dirPath = wsky_path_getProgramDirectoryPath();
  yolo_assert_not_null(dirPath);
  char *file = wsky_path_concat(dirPath, fileName);
  yolo_assert_not_null(file);
  wsky_free(dirPath);
  return file;
}

static void runWhiskeyTests(void) {
  char *filePath = getLocalFilePath("test.wsky");
  ReturnValue rv = wsky_evalFile(filePath);
  wsky_free(filePath);
  if (rv.exception) {
    wsky_Exception_print(rv.exception);
  }
}

int main() {
  yolo_begin();
  wsky_start();

  dictTestSuite();
  exceptionTestSuite();
  programFileTestSuite();
  positionTestSuite();
  stringReaderTestSuite();
  lexerTestSuite();
  parserTestSuite();
  evalTestSuite();
  mathTestSuite();

  runWhiskeyTests();

  wsky_stop();
  yolo_end();

  return 0;
}
