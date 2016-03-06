#include "objects/str.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"
#include "../return_value_private.h"
#include "objects/value_error.h"
#include "objects/not_implemented_error.h"


typedef wsky_Object Object;
typedef wsky_Value Value;
typedef wsky_String String;


#define CAST_TO_STRING(value) ((String *) (value).v.objectValue)

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params);
static ReturnValue destroy(Object *object);


static ReturnValue toString(String *self);
static ReturnValue getLength(String *self);
static ReturnValue indexOf(String *self, Value *otherV);

static ReturnValue operatorEquals(String *object, Value *value);
static ReturnValue operatorNotEquals(String *object, Value *value);

static ReturnValue operatorPlus(String *object, Value *value);
static ReturnValue operatorRPlus(String *self, Value *value);
static ReturnValue operatorStar(String *self, Value *value);


#define M(name, paramCount)                     \
  {#name, paramCount, wsky_MethodFlags_PUBLIC,  \
      (wsky_Method0)&name}

#define GET(name, function) {                           \
    #name,                                              \
      0,                                                \
      wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC,   \
      (wsky_Method0)&function}

#define OP(op, name)                            \
  {"operator " #op, 1, wsky_MethodFlags_PUBLIC, \
      (wsky_Method0)&operator ## name}


static wsky_MethodDef methods[] = {
  GET(length, getLength),
  GET(toString, toString),

  M(indexOf, 1),

  OP(==, Equals),
  OP(!=, NotEquals),

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
  .final = true,
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(String),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_String_CLASS;



String *wsky_String_new(const char *cString) {
  ReturnValue r = wsky_Object_new(wsky_String_CLASS, 0, NULL);
  if (r.exception)
    return NULL;
  String *string = (String *) r.v.v.objectValue;
  string->string = wsky_strdup(cString);
  return string;
}

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  (void) paramCount;
  (void) params;

  String *self = (String *) object;
  self->string = NULL;
  RETURN_NULL;
}

static ReturnValue destroy(Object *object) {
  String *self = (String *) object;
  if (self->string)
    wsky_free(self->string);
  RETURN_NULL;
}



static ReturnValue getLength(String *self) {
  RETURN_INT((wsky_int) strlen(self->string));
}

ReturnValue wsky_String_equals(String *self,
                               Value otherV) {
  if (!wsky_isString(otherV))
    RETURN_FALSE;
  String *other = CAST_TO_STRING(otherV);
  RETURN_BOOL(strcmp(self->string, other->string));
}

static bool startsWith(const char *a, const char *prefix) {
  while (*a == *prefix) {
    if (!*prefix)
      return true;
    a++;
    prefix++;
    if (!*prefix)
      return true;
  }
  return !prefix;
}

static wsky_int indexOfImpl(const char *a, const char *target) {
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
  if (!wsky_isString(otherV)) {
    RAISE_NEW_EXCEPTION("");
  }
  String *prefix = CAST_TO_STRING(otherV);
  RETURN_BOOL(startsWith(self->string, prefix->string));
}

static ReturnValue indexOf(String *self, Value *otherV) {
  if (!wsky_isString(*otherV)) {
    RAISE_NEW_EXCEPTION("");
  }
  String *other = CAST_TO_STRING(*otherV);
  RETURN_INT(indexOfImpl(self->string, other->string));
}

ReturnValue wsky_String_contains(String *self,
                                 Value otherV) {
  if (!wsky_isString(otherV))
    RAISE_NEW_EXCEPTION("");
  String *other = CAST_TO_STRING(otherV);
  RETURN_BOOL(indexOfImpl(self->string, other->string) != -1);
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
  char *s = wsky_safeMalloc(max_length + 1);
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
  string->string = wsky_malloc(newLength + 1);
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
  string->string = wsky_malloc(newLength + 1);
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

static inline ReturnValue toString(String *self) {
  RETURN_OBJECT((Object *) self);
}



#define RAISE_NOT_IMPL                          \
  RAISE_NEW_NOT_IMPLEMENTED_ERROR("")



static inline char *castToCString(Value v) {
  // TODO: If the given value is not a string, an exception need to be
  // thrown.
  // Rewrite this function.
  assert(wsky_isString(v));
  String *s = (String *) v.v.objectValue;
  return wsky_strdup(s->string);
}

static ReturnValue operatorEquals(String *self, Value *value) {
  if (!wsky_isString(*value))
    RAISE_NOT_IMPL;
  String *other = (String *)value->v.objectValue;
  RETURN_BOOL(strcmp(self->string, other->string) == 0);
}

static ReturnValue operatorNotEquals(String *self, Value *value) {
  if (!wsky_isString(*value))
    RAISE_NOT_IMPL;
  String *other = (String *)value->v.objectValue;
  RETURN_BOOL(strcmp(self->string, other->string) != 0);
}

static ReturnValue operatorPlus(String *self, Value *value) {
  ReturnValue rv = wsky_toString(*value);
  if (rv.exception)
    return rv;
  char *right = castToCString(rv.v);

  String *new = concat(self->string, strlen(self->string),
                       right, strlen(right));
  wsky_free(right);
  RETURN_OBJECT((Object *)new);
}


static ReturnValue operatorRPlus(String *self, Value *value) {
  ReturnValue rv = wsky_toString(*value);
  if (rv.exception)
    return rv;
  char *right = castToCString(rv.v);

  String *new = concat(right, strlen(right),
                       self->string, strlen(self->string));
  wsky_free(right);
  RETURN_OBJECT((Object *)new);
}


static ReturnValue operatorStar(String *self, Value *value) {
  if (value->type != wsky_Type_INT) {
    RAISE_NOT_IMPL;
  }
  wsky_int count = value->v.intValue;
  if (count < 0) {
    wsky_ValueError *e = wsky_ValueError_new("The factor cannot be negative");
    RAISE_EXCEPTION((wsky_Exception *)e);
  }

  String *new = multiply(self->string, strlen(self->string),
                         (unsigned) count);
  RETURN_OBJECT((Object *)new);
}



#undef RETURN_NOT_IMPL
