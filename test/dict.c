#include "test.h"

#include <stdlib.h>
#include "dict.h"

static void delete(void) {
  wsky_Dict *dict = wsky_Dict_new();
  wsky_Dict_delete(dict);
}

static void a(void) {
  wsky_Dict *dict = wsky_Dict_new();
  wsky_Dict_set(dict, "in", "the");
  wsky_Dict_set(dict, "town", "where");
  wsky_Dict_set(dict, "i", "was");
  wsky_Dict_set(dict, "born", "lived");
  yolo_assert_str_eq("where", wsky_Dict_remove(dict, "town"));
  wsky_Dict_delete(dict);
}

void dictTestSuite(void) {
  delete();
  a();
}
