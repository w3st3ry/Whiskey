#include "value.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "objects/boolean.h"
#include "objects/exception.h"
#include "objects/float.h"
#include "objects/integer.h"
#include "objects/null.h"
#include "objects/str.h"
#include "objects/class.h"
#include "gc.h"
#include "return_value.h"


typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_Object Object;


const Value wsky_Value_NULL = {
  .type = wsky_Type_OBJECT,
  .v = {
    .objectValue = NULL
  }
};
const Value wsky_Value_ZERO = {
  .type = wsky_Type_INT,
  .v = {
    .intValue = 0
  }
};
const Value wsky_Value_TRUE = {
  .type = wsky_Type_BOOL,
  .v = {
    .boolValue = true
  }
};
const Value wsky_Value_FALSE = {
  .type = wsky_Type_BOOL,
  .v = {
    .boolValue = false
  }
};



Value *wsky_Value_new(Value v) {
  Value *pointer = wsky_malloc(sizeof(Value));
  if (!pointer)
    return NULL;
  *pointer = v;
  return pointer;
}



wsky_Class *wsky_getClass(const Value value) {
  switch (value.type) {
  case wsky_Type_INT:
    return wsky_Integer_CLASS;

  case wsky_Type_BOOL:
    return wsky_Boolean_CLASS;

  case wsky_Type_FLOAT:
    return wsky_Float_CLASS;

  case wsky_Type_OBJECT:
    if (!value.v.objectValue)
      return wsky_Null_CLASS;
    return value.v.objectValue->class;
  }
  abort();
}

const char *wsky_getClassName(const Value value) {
  const wsky_Class *class = wsky_getClass(value);
  return class->name;
}



Value wsky_vaBuildValue(const char *format, va_list parameters) {
  while (*format) {
    switch (*format) {
    case 'i':
      return wsky_Value_fromInt((wsky_int) va_arg(parameters, long));
    case 'f':
      return wsky_Value_fromFloat((wsky_float) va_arg(parameters, double));
    case 's': {
      wsky_String *s = wsky_String_new(va_arg(parameters, char *));
      return wsky_Value_fromObject((wsky_Object *) s);
    }
    default:
      fprintf(stderr, "wsky_Value_vaBuild(): Invalid format\n");
      abort();
    }
  }
  return wsky_Value_NULL;
}

Value wsky_buildValue(const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  Value v = wsky_vaBuildValue(format, parameters);
  va_end(parameters);
  return v;
}



static int wsky_vaParseObject(wsky_Object *o,
                              const char format,
                              va_list params) {
  switch (format) {
  case 's': {
    assert(o);
    if (o->class != wsky_String_CLASS)
      return 1;
    char *dest = va_arg(params, char*);
    wsky_String *src = (wsky_String *)o;
    strcpy(dest, src->string);
    break;
  }

  case 'S': {
    if (o && o->class != wsky_String_CLASS)
      return 1;
    char **dest = va_arg(params, char**);
    wsky_String *src = (wsky_String *)o;
    if (src)
      *dest = wsky_strdup(src->string);
    else
      *dest = NULL;
    break;
  }

  default:
    return 1;
  }

  return 0;
}

static int wsky_vaParseValue(Value value, const char format, va_list params) {
  switch (format) {
  case 'i':
    if (value.type != wsky_Type_INT)
      return 1;
    *va_arg(params, wsky_int *) = value.v.intValue;
    break;

  case 'f':
    if (value.type != wsky_Type_FLOAT)
      return 1;
    *va_arg(params, double *) = (wsky_float) value.v.floatValue;
    break;

  default:
    if (value.type != wsky_Type_OBJECT)
      return 1;
    return wsky_vaParseObject(value.v.objectValue, format, params);
  }

  return 0;
}

int wsky_vaParseValues(const Value *values, const char *format,
                       va_list parameters) {
  while (*format) {
    if (wsky_vaParseValue(*values, *format, parameters))
      return 1;
    format++;
    values++;
  }
  return 0;
}

int wsky_parseValues(const Value *values, const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  int r = wsky_vaParseValues(values, format, parameters);
  va_end(parameters);
  return r;
}
