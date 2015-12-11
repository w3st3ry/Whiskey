#include "tests.h"

#include <string.h>
#include "../class.h"
#include "../gc.h"
#include "../str.h"

static const char *helloCString = "Hello World!";
static degat_String *helloString;
static degat_Object *helloStringObj;
static degat_String *hString;
static degat_Object *hStringObj;
static degat_String *emptyString;
static degat_Object *emptyStringObj;



static void setup(void) {
  helloString = degat_String_new(helloCString);
  helloStringObj = (degat_Object *) helloString;
  hString = degat_String_new("h");
  hStringObj = (degat_Object *) hString;
  emptyString = degat_String_new("");
  emptyStringObj = (degat_Object *) emptyString;
}

static void teardown(void) {
  degat_DECREF(helloString);
  degat_DECREF(hString);
  degat_DECREF(emptyString);
}



static void base(void) {
  yolo_assert_int_eq(0, strcmp(helloString->string, helloCString));
  yolo_assert_int_eq(0, strcmp(emptyString->string, ""));
}

static void getLength(void) {
  degat_ReturnValue r;
  r = degat_Object_callMethod0(helloStringObj, "getLength");
  yolo_assert_long_eq((long)strlen(helloCString), r.v.v.intValue);
  r = degat_Object_callMethod0(emptyStringObj, "getLength");
  yolo_assert_long_eq(0, r.v.v.intValue);
}

static void startsWith(void) {
  degat_ReturnValue r;
  r = degat_Object_callMethod1(helloStringObj,
			       "startsWith",
			       degat_Value_fromObject(emptyStringObj));
  yolo_assert_int_eq(0, r.v.v.boolValue);
}


void strTestSuite(void) {
  setup();

  base();
  getLength();
  startsWith();

  teardown();
}
