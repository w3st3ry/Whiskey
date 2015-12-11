#include "tests.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../string_reader.h"


static void base(void) {
  degat_StringReader *reader;

  reader = degat_StringReader_newStr("");
  yolo_assert(!degat_StringReader_hasMore(reader));
  degat_StringReader_delete(reader);

  reader = degat_StringReader_newStr("a");
  yolo_assert(degat_StringReader_hasMore(reader));
  yolo_assert_char_eq(degat_StringReader_next(reader), 'a');
  yolo_assert(!degat_StringReader_hasMore(reader));
  degat_StringReader_delete(reader);
}


void stringReaderTestSuite(void) {
  base();
}
