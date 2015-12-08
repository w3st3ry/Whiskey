#include "minunit.h"

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

void testSetup() {
  degat_init();
  helloString = degat_String_new(helloCString);
  helloStringObj = (degat_Object *) helloString;
  hString = degat_String_new("h");
  hStringObj = (degat_Object *) hString;
  emptyString = degat_String_new("");
  emptyStringObj = (degat_Object *) emptyString;
}

void testTeardown() {
  degat_Object_DECREF(helloString);
  degat_Object_DECREF(hString);
  degat_Object_DECREF(emptyString);
  degat_free();
}



MU_TEST(base) {
  mu_assert_int_eq(0, strcmp(helloString->string, helloCString));
  mu_assert_int_eq(0, strcmp(emptyString->string, ""));
}

MU_TEST(getLength) {
  degat_ReturnValue r;
  r = degat_Object_callMethod0(helloStringObj, "getLength");
  mu_assert_int_eq(strlen(helloCString), r.v.v.intValue);
  r = degat_Object_callMethod0(emptyStringObj, "getLength");
  mu_assert_int_eq(0, r.v.v.intValue);
}

MU_TEST(startsWith) {
  degat_ReturnValue r;
  r = degat_Object_callMethod1(helloStringObj,
			       "startsWith",
			       degat_Value_fromObject(emptyStringObj));
  mu_assert_int_eq(0, r.v.v.boolValue);
}



int main() {
  MU_SUITE_CONFIGURE(&testSetup, &testTeardown);
  MU_RUN_TEST(base);
  MU_RUN_TEST(getLength);
  MU_RUN_TEST(startsWith);
}
