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
#include "gc.h"
#include "return_value.h"


typedef wsky_Value Value;
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

Value wsky_Value_fromBool(bool n) {
  return n ? wsky_Value_TRUE : wsky_Value_FALSE;
}

Value wsky_Value_fromObject(wsky_Object *object) {
  Value v = {
    .type = wsky_Type_OBJECT,
    .v = {
      .objectValue = object
    }
  };
  return v;
}

Value wsky_Value_fromInt(int64_t n) {
  Value v = {
    .type = wsky_Type_INT,
    .v = {
      .intValue = n
    }
  };
  return v;
}

Value wsky_Value_fromFloat(double n) {
  Value v = {
    .type = wsky_Type_FLOAT,
    .v = {
      .floatValue = n
    }
  };
  return v;
}

bool wsky_Value_isNull(const Value value) {
  return value.type == wsky_Type_OBJECT &&
    !value.v.objectValue;
}



static char *getDefaultString(wsky_Object *object) {
  return wsky_STRDUP(object->class->name);
}

char *wsky_Value_toCString(const Value value) {
  switch (value.type) {
  case wsky_Type_BOOL: {
    return wsky_STRDUP(value.v.boolValue ? "true" : "false");
  }

  case wsky_Type_INT: {
    int64_t v = value.v.intValue;
    char *s = wsky_MALLOC(100);
    snprintf(s, 99, "%ld", (long) v);
    return s;
  }

  case wsky_Type_FLOAT: {
    char *s = wsky_MALLOC(100);
    snprintf(s, 80, "%.10g", value.v.floatValue);
    if (!strchr(s, '.') && !strchr(s, 'e')) {
      strcat(s, ".0");
    }
    return s;
  }

  case wsky_Type_OBJECT: {
    wsky_Object *object = value.v.objectValue;
    const wsky_Class *class = wsky_Value_getClass(value);
    if (class == &wsky_String_CLASS) {
      wsky_String *s = (wsky_String *) object;
      return wsky_STRDUP(s->string);
    }
    wsky_ReturnValue rv = wsky_Object_callMethod0(object, "toString");
    if (rv.exception) {
      return wsky_STRDUP(rv.exception->message);
    }
    if (!wsky_isString(rv.v)) {
      return getDefaultString(object);
    }
    char *cString = wsky_Value_toCString(rv.v);
    return (cString);
  }
  }
  return NULL;
}

wsky_String *wsky_Value_toString(const Value value) {
  char *cString = wsky_Value_toCString(value);
  wsky_String *s = wsky_String_new(cString);
  wsky_FREE(cString);
  return s;
}


const wsky_Class *wsky_Value_getClass(const wsky_Value value) {
  switch (value.type) {
  case wsky_Type_INT:
    return &wsky_Integer_CLASS;

  case wsky_Type_BOOL:
    return &wsky_Boolean_CLASS;

  case wsky_Type_FLOAT:
    return &wsky_Float_CLASS;

  case wsky_Type_OBJECT:
    if (!value.v.objectValue)
      return &wsky_Null_CLASS;
    return value.v.objectValue->class;
  }
}


const char *wsky_Value_getClassName(const wsky_Value value) {
  const wsky_Class *class = wsky_Value_getClass(value);
  return class->name;
}


Value wsky_vaBuildValue(const char *format, va_list parameters) {
  while (*format) {
    switch (*format) {
    case 'i':
      return wsky_Value_fromInt(va_arg(parameters, int64_t));
    case 'f':
      return wsky_Value_fromFloat(va_arg(parameters, double));
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
    if (o->class != &wsky_String_CLASS)
      return 1;
    char *dest = va_arg(params, char*);
    wsky_String *src = (wsky_String *)o;
    strcpy(dest, src->string);
    break;
  }

  case 'S': {
    if (o && o->class != &wsky_String_CLASS)
      return 1;
    char **dest = va_arg(params, char**);
    wsky_String *src = (wsky_String *)o;
    if (src)
      *dest = wsky_STRDUP(src->string);
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
    *va_arg(params, int64_t *) = value.v.intValue;
    break;

  case 'f':
    if (value.type != wsky_Type_FLOAT)
      return 1;
    *va_arg(params, double *) = value.v.intValue;
    break;

  default:
    if (value.type != wsky_Type_OBJECT)
      return 1;
    return wsky_vaParseObject(value.v.objectValue, format, params);
  }

  return 0;
}

int wsky_vaParseValues(Value *values, const char *format,
                       va_list parameters) {
  while (*format) {
    if (wsky_vaParseValue(*values, *format, parameters))
      return 1;
    format++;
    values++;
  }
  return 0;
}

int wsky_parseValues(Value *values, const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  int r = wsky_vaParseValues(values, format, parameters);
  va_end(parameters);
  return r;
}
