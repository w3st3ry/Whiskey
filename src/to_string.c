#include <assert.h>
#include <string.h>
#include "whiskey_private.h"


inline static char *boolToCString(bool v) {
  return wsky_strdup(v ? "true" : "false");
}

inline static char *intToCString(wsky_int v) {
  char *s = wsky_asprintf("%ld", (long) v);
  return s;
}

inline static char *floatToCString(wsky_float v) {
  char *s = wsky_safeMalloc(100);
  snprintf(s, 80, "%.10g", (double) v);
  if (!strchr(s, '.') && !strchr(s, 'e')) {
    strcat(s, ".0");
  }
  return s;
}

/* Returns a malloc'd null-terminated string */
static char *primitiveToCString(const Value value) {
  switch (value.type) {
  case Type_BOOL:
    return boolToCString(value.v.objectValue);
  case Type_INT:
    return intToCString(value.v.intValue);
  case Type_FLOAT:
    return floatToCString(value.v.floatValue);
  case Type_OBJECT:
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
  if (value.type == Type_OBJECT) {
    return wsky_Object_toString(value.v.objectValue);
  }
  wsky_RETURN_OBJECT((Object *) primitiveToString(value));
}
