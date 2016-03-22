#include "test.h"

#include <string.h>
#include "whiskey.h"


static wsky_ProgramFile *file;

#define CREATE(string) wsky_StringReader_create(file, string)
#define HAS_MORE(reader) wsky_StringReader_hasMore(reader)
#define NEXT(reader) wsky_StringReader_next(reader)
#define ASSERT_HAS_MORE(reader) yolo_assert(HAS_MORE(reader))
#define ASSERT_EOF(reader) yolo_assert(!HAS_MORE(reader))

#define ASSERT_LINE_EQ(lineNumber, reader)                      \
  yolo_assert_int_eq(lineNumber, (reader)->position.line)

#define ASSERT_COLUMN_EQ(columnNumber, reader)                  \
  yolo_assert_int_eq(columnNumber, (reader)->position.column)


static void empty(void) {
  wsky_StringReader reader;

  reader = CREATE("");
  ASSERT_EOF(&reader);
  ASSERT_LINE_EQ(1, &reader);
  ASSERT_COLUMN_EQ(0, &reader);
  wsky_StringReader_free(&reader);
}

static void newLine(void) {
  wsky_StringReader reader;

  reader = CREATE("a\n");
  yolo_assert_int_eq(1, reader.position.line);
  yolo_assert_int_eq(0, reader.position.column);
  ASSERT_HAS_MORE(&reader);

  yolo_assert_char_eq('a', NEXT(&reader));
  ASSERT_HAS_MORE(&reader);
  yolo_assert_int_eq(1, reader.position.line);
  yolo_assert_int_eq(1, reader.position.column);

  yolo_assert_char_eq('\n', NEXT(&reader));
  ASSERT_EOF(&reader);
  yolo_assert_int_eq(2, reader.position.line);
  yolo_assert_int_eq(0, reader.position.column);

  wsky_StringReader_free(&reader);
}

static void skipping(void) {
  wsky_StringReader reader;
  reader = CREATE("a\n\r\t \t   b");

  yolo_assert_char_eq('a', NEXT(&reader));
  ASSERT_HAS_MORE(&reader);

  wsky_StringReader_skipWhitespaces(&reader);

  yolo_assert_char_eq('b', NEXT(&reader));
  ASSERT_EOF(&reader);

  wsky_StringReader_free(&reader);
}

static void skipping2(void) {
  wsky_StringReader reader;
  reader = CREATE("a\n\r  \t  ");

  yolo_assert_char_eq('a', NEXT(&reader));
  ASSERT_HAS_MORE(&reader);

  wsky_StringReader_skipWhitespaces(&reader);

  ASSERT_EOF(&reader);

  wsky_StringReader_free(&reader);
}

static void readString(void) {
  wsky_StringReader reader;
  reader = CREATE("abcdef");

  yolo_assert(wsky_StringReader_readString(&reader, "abcde"));
  yolo_assert_char_eq('f', NEXT(&reader));

  wsky_StringReader_free(&reader);
}

static void token(void) {
  wsky_StringReader reader;
  reader = CREATE("a bc d");

  NEXT(&reader);
  yolo_assert_char_eq(' ', NEXT(&reader));
  wsky_Position begin = reader.position;
  yolo_assert_char_eq('b', NEXT(&reader));
  yolo_assert_char_eq('c', NEXT(&reader));
  wsky_Token t;
  t = wsky_StringReader_createToken(&reader, begin, wsky_TokenType_COMMENT);
  yolo_assert_str_eq("bc", t.string);
  wsky_Token_free(&t);
  yolo_assert_char_eq(' ', NEXT(&reader));

  wsky_StringReader_free(&reader);
}

void stringReaderTestSuite(void) {
  file = wsky_ProgramFile_getUnknown();

  empty();
  newLine();
  skipping();
  skipping2();
  readString();
  token();
}
