#ifndef TESTS_H_
# define TESTS_H_

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
void strTestSuite(void);
void programFileTestSuite(void);
void exceptionTestSuite(void);
void stringReaderTestSuite(void);
void lexerTestSuite(void);
void parserTestSuite(void);
void evalTestSuite(void);
void mathTestSuite(void);


#endif /* !TESTS_H_ */
