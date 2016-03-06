#include "return_value.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"
#include "objects/str.h"
#include "string_utils.h"

typedef wsky_String String;
typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;

inline static char *boolToCString(bool v) {
  return wsky_strdup(v ? "true" : "false");
}

inline static char *intToCString(wsky_int v) {
  char *s = wsky_asprintf("%ld", (long) v);
  return s;
}

inline static char *floatToCString(wsky_float v) {
  char *s = wsky_asprintf("%.10g", (double) v);
  if (!strchr(s, '.') && !strchr(s, 'e')) {
    strcat(s, ".0");
  }
  return s;
}

/* Returns a malloc'd null-terminated string */
static char *primitiveToCString(const Value value) {
  switch (value.type) {
  case wsky_Type_BOOL:
    return boolToCString(value.v.objectValue);
  case wsky_Type_INT:
    return intToCString(value.v.intValue);
  case wsky_Type_FLOAT:
    return floatToCString(value.v.floatValue);
  case wsky_Type_OBJECT:
    abort();
  }
  abort();
}

static String *primitiveToString(const Value value) {
  char *cs = primitiveToCString(value);
  String *s = wsky_String_new(cs);
  wsky_free(cs);
  return s;
}

ReturnValue wsky_toString(const Value value) {
  if (value.type == wsky_Type_OBJECT) {
    return (wsky_Object_toString(value.v.objectValue));
  }
  wsky_RETURN_OBJECT((Object *) primitiveToString(value));
}
