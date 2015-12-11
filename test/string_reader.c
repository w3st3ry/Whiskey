#include "tests.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../string_reader.h"


static void empty(void) {
  wsky_StringReader *reader;

  reader = wsky_StringReader_newStr("");
  yolo_assert(!wsky_StringReader_hasMore(reader));
  yolo_assert_int_eq(1, reader->position.line);
  yolo_assert_int_eq(0, reader->position.column);
  wsky_StringReader_delete(reader);
}

static void newLine(void) {
  wsky_StringReader *reader;

  reader = wsky_StringReader_newStr("a\n");
  yolo_assert_int_eq(1, reader->position.line);
  yolo_assert_int_eq(0, reader->position.column);
  yolo_assert(wsky_StringReader_hasMore(reader));

  yolo_assert_char_eq('a', wsky_StringReader_next(reader));
  yolo_assert(wsky_StringReader_hasMore(reader));
  yolo_assert_int_eq(1, reader->position.line);
  yolo_assert_int_eq(1, reader->position.column);

  yolo_assert_char_eq('\n', wsky_StringReader_next(reader));
  yolo_assert(!wsky_StringReader_hasMore(reader));
  yolo_assert_int_eq(2, reader->position.line);
  yolo_assert_int_eq(0, reader->position.column);

  wsky_StringReader_delete(reader);
}

static void token(void) {
  wsky_StringReader *reader;

  reader = wsky_StringReader_newStr("a bc d");

  wsky_StringReader_next(reader);
  yolo_assert_char_eq(' ', wsky_StringReader_next(reader));
  wsky_Position begin = reader->position;
  yolo_assert_char_eq('b', wsky_StringReader_next(reader));
  yolo_assert_char_eq('c', wsky_StringReader_next(reader));
  wsky_Token token = wsky_StringReader_createToken(reader, begin);
  yolo_assert_str_eq("bc", token.string);
  wsky_Token_free(&token);
  yolo_assert_char_eq(' ', wsky_StringReader_next(reader));

  wsky_StringReader_delete(reader);
}

void stringReaderTestSuite(void) {
  empty();
  newLine();
  token();
}
