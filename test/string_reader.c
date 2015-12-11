#include "tests.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../string_reader.h"


static void empty(void) {
  degat_StringReader *reader;

  reader = degat_StringReader_newStr("");
  yolo_assert(!degat_StringReader_hasMore(reader));
  yolo_assert_int_eq(1, reader->position.line);
  yolo_assert_int_eq(0, reader->position.column);
  degat_StringReader_delete(reader);
}

static void newLine(void) {
  degat_StringReader *reader;

  reader = degat_StringReader_newStr("a\n");
  yolo_assert_int_eq(1, reader->position.line);
  yolo_assert_int_eq(0, reader->position.column);
  yolo_assert(degat_StringReader_hasMore(reader));

  yolo_assert_char_eq('a', degat_StringReader_next(reader));
  yolo_assert(degat_StringReader_hasMore(reader));
  yolo_assert_int_eq(1, reader->position.line);
  yolo_assert_int_eq(1, reader->position.column);

  yolo_assert_char_eq('\n', degat_StringReader_next(reader));
  yolo_assert(!degat_StringReader_hasMore(reader));
  yolo_assert_int_eq(2, reader->position.line);
  yolo_assert_int_eq(0, reader->position.column);

  degat_StringReader_delete(reader);
}

static void token(void) {
  degat_StringReader *reader;

  reader = degat_StringReader_newStr("a bc d");

  degat_StringReader_next(reader);
  yolo_assert_char_eq(' ', degat_StringReader_next(reader));
  degat_Position begin = reader->position;
  yolo_assert_char_eq('b', degat_StringReader_next(reader));
  yolo_assert_char_eq('c', degat_StringReader_next(reader));
  degat_Token token = degat_StringReader_createToken(reader, begin);
  yolo_assert_str_eq("bc", token.string);
  degat_Token_free(&token);
  yolo_assert_char_eq(' ', degat_StringReader_next(reader));

  degat_StringReader_delete(reader);
}

void stringReaderTestSuite(void) {
  empty();
  newLine();
  token();
}
