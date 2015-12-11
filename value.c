#include "value.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"



const wsky_Value wsky_Value_NULL = {
  .type = wsky_Type_OBJECT,
  .v = {
    .objectValue = NULL
  }
};
const wsky_Value wsky_Value_ZERO = {
  .type = wsky_Type_INT,
  .v = {
    .intValue = 0
  }
};
const wsky_Value wsky_Value_TRUE = {
  .type = wsky_Type_BOOL,
  .v = {
    .boolValue = true
  }
};
const wsky_Value wsky_Value_FALSE = {
  .type = wsky_Type_BOOL,
  .v = {
    .boolValue = false
  }
};

wsky_Value wsky_Value_fromBool(bool n) {
  return n ? wsky_Value_TRUE : wsky_Value_FALSE;
}

wsky_Value wsky_Value_fromObject(wsky_Object *object) {
  wsky_Value v = {
    .type = wsky_Type_OBJECT,
    .v = {
      .objectValue = object
    }
  };
  return v;
}

wsky_Value wsky_Value_fromInt(int64_t n) {
  wsky_Value v = {
    .type = wsky_Type_INT,
    .v = {
      .intValue = n
    }
  };
  return v;
}

wsky_Value wsky_Value_fromFloat(double n) {
  wsky_Value v = {
    .type = wsky_Type_FLOAT,
    .v = {
      .floatValue = n
    }
  };
  return v;
}

bool wsky_Value_isNull(const wsky_Value value) {
  return value.type == wsky_Type_OBJECT &&
    !value.v.objectValue;
}



wsky_Value wsky_vaBuildValue(const char *format, va_list parameters) {
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

wsky_Value wsky_buildValue(const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  wsky_Value v = wsky_vaBuildValue(format, parameters);
  va_end(parameters);
  return v;
}



static int wsky_vaParseObject(wsky_Object *o,
			       const char format,
			       va_list params) {
  switch (format) {
  case 's': {
    if (o->class != &wsky_String_CLASS)
      return 1;
    char *dest = va_arg(params, char*);
    wsky_String *src = (wsky_String *)o;
    if (src)
      strcpy(dest, src->string);
    else
      abort();
    break;
  }

  case 'S': {
    if (o->class != &wsky_String_CLASS)
      return 1;
    char **dest = va_arg(params, char**);
    wsky_String *src = (wsky_String *)o;
    if (src)
      *dest = strdup(src->string);
    else
      *dest = NULL;
    break;
  }

  default:
    return 1;
  }

  return 0;
}

int wsky_vaParseValue(wsky_Value value,
		       const char format,
		       va_list params) {
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

int wsky_vaParseValues(wsky_Value *values, const char *format,
			va_list parameters) {
  while (*format) {
    if (wsky_vaParseValue(*values, *format, parameters))
      return 1;
    format++;
    values++;
  }
  return 0;
}

int wsky_parseValues(wsky_Value *values, const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  int r = wsky_vaParseValues(values, format, parameters);
  va_end(parameters);
  return r;
}
