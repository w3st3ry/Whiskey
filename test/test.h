#ifndef TEST_H
# define TEST_H

# include "yolo.h"


void assertEvalEqImpl(const char *expected,
                      const char *source,
                      const char *testName,
                      const char *position);

void assertExceptionImpl(const char *exceptionClass,
                         const char *expectedMessage,
                         const char *source,
                         const char *testName,
                         const char *position);

# define assertEvalEq(expectedAstString, source)        \
  assertEvalEqImpl((expectedAstString), (source),       \
                   __func__, YOLO__POSITION_STRING)

# define assertException(exceptionClass, expectedMessage, source)       \
  assertExceptionImpl((exceptionClass), (expectedMessage), (source),    \
                      __func__, YOLO__POSITION_STRING)


char *getLocalFilePath(const char *fileName);

void dictTestSuite(void);
void programFileTestSuite(void);
void exceptionTestSuite(void);
void positionTestSuite(void);
void stringReaderTestSuite(void);
void lexerTestSuite(void);
void parserTestSuite(void);
void evalTestSuite(void);

#endif /* TEST_H */
