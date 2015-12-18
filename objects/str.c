#include "str.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef wsky_Value Value;
typedef wsky_String String;
typedef wsky_ReturnValue ReturnValue;


static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 wsky_Value *params);
static void destroy(wsky_Object *object);



#define M(name, paramCount)                             \
  {#name, paramCount, (void *) &wsky_String_ ## name}

static wsky_MethodDef methods[] = {
  M(getLength, 0),
  M(startsWith, 1),
  M(indexOf, 1),
  M(contains, 1),
  {0, 0, 0},
};

#undef M

wsky_Class wsky_String_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "String",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(String),
  .methodDefs = methods,
};



String *wsky_String_new(const char *cString) {
  ReturnValue r = wsky_Object_new(&wsky_String_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  String *string = (String *) r.v.v.objectValue;
  string->string = strdup(cString);
  return string;
}

static wsky_Exception *construct(wsky_Object *object,
                                 unsigned paramCount,
                                 Value *params) {
  (void) paramCount;
  (void) params;

  String *this = (String *) object;
  this->string = NULL;
  return NULL;
}

static void destroy(wsky_Object *object) {
  String *this = (String *) object;
  if (this->string)
    free(this->string);
}



bool wsky_isString(const Value value) {
  if (value.type != wsky_Type_OBJECT)
    return false;
  if (wsky_Value_isNull(value))
    return false;
  return value.v.objectValue->class == &wsky_String_CLASS;
}



ReturnValue wsky_String_getLength(String *this) {
  wsky_RETURN_INT((int32_t) strlen(this->string));
}

ReturnValue wsky_String_equals(String *this,
                               Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *other = wsky_Value_toString(otherV);
  wsky_RETURN_BOOL(strcmp(this->string, other->string));
}

static bool startsWith(const char *a, const char *prefix) {
  while (*a == *prefix) {
    if (!*a)
      return true;
    a++;
    prefix++;
  }
  return !prefix;
}

static int64_t indexOf(const char *a, const char *target) {
  int index = 0;
  while (*a) {
    if (startsWith(a, target)) {
      return index;
    }
    index++;
    a++;
  }
  return -1;
}

ReturnValue wsky_String_startsWith(String *this,
                                   Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *prefix = wsky_Value_toString(otherV);
  wsky_RETURN_BOOL(startsWith(this->string, prefix->string));
}

ReturnValue wsky_String_indexOf(String *this,
                                Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *other = wsky_Value_toString(otherV);
  wsky_RETURN_INT(indexOf(this->string, other->string));
}

ReturnValue wsky_String_contains(String *this,
                                 Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *other = wsky_Value_toString(otherV);
  wsky_RETURN_BOOL(indexOf(this->string, other->string) != -1);
}

void wsky_String_print(const String *this) {
  printf("%s", this->string);
}



static void escapeChar(char *dest, char source) {
  /* TODO: Improve a lot, it’s crappy */

  switch (source) {
  case '\n':
    strcat(dest, "\\n");
    return;
  case '\r':
    strcat(dest, "\\r");
    return;
  case '\t':
    strcat(dest, "\\t");
    return;
  case '\0':
    strcat(dest, "\\0");
    return;
  case '\'':
    strcat(dest, "\\'");
    return;
  case '\"':
    strcat(dest, "\\\"");
    return;
  }

  size_t length = strlen(dest);
  dest[length] = source;
  dest[length + 1] = '\0';
}

char *wsky_String_escapeCString(const char *source) {
  size_t max_length = strlen(source) * 2 + 2;
  char *s = malloc(max_length + 1);
  strcpy(s, "'");
  while (*source) {
    escapeChar(s, *source);
    source++;
  }
  strcat(s, "'");
  return (s);
}
