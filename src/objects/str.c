#include "objects/str.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"
#include "objects/value_error.h"
#include "objects/not_implemented_error.h"


typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_String String;
typedef wsky_ReturnValue ReturnValue;


static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             Value *params);
static ReturnValue destroy(Object *object);


static ReturnValue toString(String *self);
static ReturnValue getLength(String *self);

static ReturnValue operatorPlus(String *object, Value *value);

static ReturnValue operatorPlus(String *object, Value *value);
static ReturnValue operatorRPlus(String *self, Value *value);
static ReturnValue operatorStar(String *self, Value *value);


#define M(name, paramCount)                     \
  {#name, paramCount, wsky_MethodFlags_PUBLIC,  \
      (void *) &wsky_String_ ## name}

#define GET(name)                                       \
  {#name, 0, wsky_MethodFlags_GET, (void *) &name}

#define OP(op, name)                            \
  {"operator " #op, 0, wsky_MethodFlags_PUBLIC, \
      (void *) &operator ## name}


static wsky_MethodDef methods[] = {
  GET(getLength),
  GET(toString),

  M(startsWith, 1),
  M(indexOf, 1),
  M(contains, 1),

  OP(+, Plus),
  OP(r+, RPlus),
  OP(*, Star),
  {0, 0, 0, 0},
};

#undef M
#undef GET
#undef OP


const wsky_ClassDef wsky_String_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "String",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(String),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};



String *wsky_String_new(const char *cString) {
  ReturnValue r = wsky_Object_new(wsky_String_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  String *string = (String *) r.v.v.objectValue;
  string->string = wsky_STRDUP(cString);
  return string;
}

static wsky_ReturnValue construct(Object *object,
                                  unsigned paramCount,
                                  Value *params) {
  (void) paramCount;
  (void) params;

  String *self = (String *) object;
  self->string = NULL;
  wsky_RETURN_NULL;
}

static wsky_ReturnValue destroy(Object *object) {
  String *self = (String *) object;
  if (self->string)
    wsky_FREE(self->string);
  wsky_RETURN_NULL;
}



bool wsky_isString(const Value value) {
  if (value.type != wsky_Type_OBJECT)
    return false;
  if (wsky_isNull(value))
    return false;
  return value.v.objectValue->class == wsky_String_CLASS;
}



static ReturnValue getLength(String *self) {
  wsky_RETURN_INT((wsky_int) strlen(self->string));
}

ReturnValue wsky_String_equals(String *self,
                               Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *other = wsky_toString(otherV);
  wsky_RETURN_BOOL(strcmp(self->string, other->string));
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

static wsky_int indexOf(const char *a, const char *target) {
  wsky_int index = 0;
  while (*a) {
    if (startsWith(a, target)) {
      return index;
    }
    index++;
    a++;
  }
  return -1;
}

ReturnValue wsky_String_startsWith(String *self,
                                   Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *prefix = wsky_toString(otherV);
  wsky_RETURN_BOOL(startsWith(self->string, prefix->string));
}

ReturnValue wsky_String_indexOf(String *self,
                                Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *other = wsky_toString(otherV);
  wsky_RETURN_INT(indexOf(self->string, other->string));
}

ReturnValue wsky_String_contains(String *self,
                                 Value otherV) {
  if (!wsky_isString(otherV))
    wsky_RETURN_FALSE;
  String *other = wsky_toString(otherV);
  wsky_RETURN_BOOL(indexOf(self->string, other->string) != -1);
}

void wsky_String_print(const String *self) {
  printf("%s", self->string);
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
  char *s = wsky_MALLOC(max_length + 1);
  strcpy(s, "'");
  while (*source) {
    escapeChar(s, *source);
    source++;
  }
  strcat(s, "'");
  return (s);
}


static String *concat(const char *left, size_t leftLength,
                      const char *right, size_t rightLength) {

  ReturnValue r = wsky_Object_new(wsky_String_CLASS, 0, NULL);
  String *string = (String *) r.v.v.objectValue;
  size_t newLength = leftLength + rightLength;
  string->string = wsky_MALLOC(newLength + 1);
  if (!string->string) {
    return NULL;
  }
  memcpy(string->string, left, leftLength);
  memcpy(string->string + leftLength, right, rightLength);
  string->string[newLength] = '\0';
  return string;
}

static String *multiply(const char *source, size_t sourceLength,
                        unsigned count) {

  ReturnValue r = wsky_Object_new(wsky_String_CLASS, 0, NULL);
  String *string = (String *) r.v.v.objectValue;
  size_t newLength = sourceLength * count;
  string->string = wsky_MALLOC(newLength + 1);
  if (!string->string) {
    return NULL;
  }
  unsigned i;
  for (i = 0; i < count; i++) {
    memcpy(string->string + sourceLength * i, source, sourceLength);
  }
  string->string[newLength] = '\0';
  return string;
}

static ReturnValue toString(String *self) {
  wsky_RETURN_OBJECT((Object *) self);
}



#define RETURN_NOT_IMPL                                         \
  wsky_RETURN_EXCEPTION(wsky_NotImplementedError_new(""))



static ReturnValue operatorPlus(String *self, Value *value) {
  char *right = wsky_toCString(*value);

  String *new = concat(self->string, strlen(self->string),
                       right, strlen(right));
  wsky_FREE(right);
  wsky_RETURN_OBJECT((Object *)new);
}


static ReturnValue operatorRPlus(String *self, Value *value) {
  char *right = wsky_toCString(*value);

  String *new = concat(right, strlen(right),
                       self->string, strlen(self->string));
  wsky_FREE(right);
  wsky_RETURN_OBJECT((Object *)new);
}


static ReturnValue operatorStar(String *self, Value *value) {
  if (value->type != wsky_Type_INT) {
    RETURN_NOT_IMPL;
  }
  wsky_int count = value->v.intValue;
  if (count < 0) {
    wsky_ValueError *e = wsky_ValueError_new("The factor cannot be negative");
    wsky_RETURN_EXCEPTION(e);
  }

  String *new = multiply(self->string, strlen(self->string),
                         (unsigned) count);
  wsky_RETURN_OBJECT((Object *)new);
}



#undef RETURN_NOT_IMPL
