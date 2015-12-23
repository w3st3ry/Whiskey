#include "tests.h"

#include <string.h>
#include "class.h"
#include "wsky_gc.h"
#include "str.h"

static const char *helloCString = "Hello World!";
static wsky_String *helloString;
static wsky_Object *helloStringObj;
static wsky_String *hString;
static wsky_Object *hStringObj;
static wsky_String *emptyString;
static wsky_Object *emptyStringObj;



static void setup(void) {
  helloString = wsky_String_new(helloCString);
  helloStringObj = (wsky_Object *) helloString;
  hString = wsky_String_new("h");
  hStringObj = (wsky_Object *) hString;
  emptyString = wsky_String_new("");
  emptyStringObj = (wsky_Object *) emptyString;
}

static void teardown(void) {
  wsky_GC_unmarkAll();
  wsky_GC_collect();
}



static void base(void) {
  yolo_assert_int_eq(0, strcmp(helloString->string, helloCString));
  yolo_assert_int_eq(0, strcmp(emptyString->string, ""));
}

static void getLength(void) {
  wsky_ReturnValue r;
  r = wsky_Object_callMethod0(helloStringObj, "getLength");
  yolo_assert_long_eq((long)strlen(helloCString), r.v.v.intValue);
  r = wsky_Object_callMethod0(emptyStringObj, "getLength");
  yolo_assert_long_eq(0, r.v.v.intValue);
}

static void startsWith(void) {
  wsky_ReturnValue r;
  r = wsky_Object_callMethod1(helloStringObj,
                              "startsWith",
                              wsky_Value_fromObject(emptyStringObj));
  yolo_assert_int_eq(0, r.v.v.boolValue);
}


void strTestSuite(void) {
  setup();

  base();
  getLength();
  startsWith();

  teardown();
}
