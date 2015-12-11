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


void stringReaderTestSuite(void) {
  empty();
  newLine();
}
