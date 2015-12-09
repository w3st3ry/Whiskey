#include "value.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"



const degat_Value degat_Value_NULL = {
  .type = degat_Type_OBJECT,
  .v = {
    .objectValue = NULL
  }
};
const degat_Value degat_Value_ZERO = {
  .type = degat_Type_INT,
  .v = {
    .intValue = 0
  }
};
const degat_Value degat_Value_TRUE = {
  .type = degat_Type_BOOL,
  .v = {
    .boolValue = true
  }
};
const degat_Value degat_Value_FALSE = {
  .type = degat_Type_BOOL,
  .v = {
    .boolValue = false
  }
};

degat_Value degat_Value_fromBool(bool n) {
  return n ? degat_Value_TRUE : degat_Value_FALSE;
}

degat_Value degat_Value_fromObject(degat_Object *object) {
  degat_Value v = {
    .type = degat_Type_OBJECT,
    .v = {
      .objectValue = object
    }
  };
  return v;
}

degat_Value degat_Value_fromInt(int64_t n) {
  degat_Value v = {
    .type = degat_Type_INT,
    .v = {
      .intValue = n
    }
  };
  return v;
}

degat_Value degat_Value_fromFloat(double n) {
  degat_Value v = {
    .type = degat_Type_FLOAT,
    .v = {
      .floatValue = n
    }
  };
  return v;
}

bool degat_Value_isNull(const degat_Value value) {
  return value.type == degat_Type_OBJECT &&
    !value.v.objectValue;
}



degat_Value degat_vaBuildValue(const char *format, va_list parameters) {
  while (*format) {
    switch (*format) {
    case 'i':
      return degat_Value_fromInt(va_arg(parameters, int64_t));
    case 'f':
      return degat_Value_fromFloat(va_arg(parameters, double));
    case 's': {
      degat_String *s = degat_String_new(va_arg(parameters, char *));
      return degat_Value_fromObject((degat_Object *) s);
    }
    default:
      fprintf(stderr, "degat_Value_vaBuild(): Invalid format\n");
      abort();
    }
  }
  return degat_Value_NULL;
}

degat_Value degat_buildValue(const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  degat_Value v = degat_vaBuildValue(format, parameters);
  va_end(parameters);
  return v;
}



static int degat_vaParseObject(degat_Object *o,
			       const char format,
			       va_list params) {
  switch (format) {
  case 's': {
    if (o->class != &degat_String_CLASS)
      return 1;
    char *dest = va_arg(params, char*);
    degat_String *src = (degat_String *)o;
    strcpy(dest, src->string);
    break;
  }

  case 'S': {
    if (o->class != &degat_String_CLASS)
      return 1;
    char **dest = va_arg(params, char**);
    degat_String *src = (degat_String *)o;
    *dest = strdup(src->string);
    break;
  }

  default:
    return 1;
  }

  return 0;
}

int degat_vaParseValue(degat_Value value,
		       const char format,
		       va_list params) {
  switch (format) {
  case 'i':
    if (value.type != degat_Type_INT)
      return 1;
    *va_arg(params, int64_t *) = value.v.intValue;
    break;

  case 'f':
    if (value.type != degat_Type_FLOAT)
      return 1;
    *va_arg(params, double *) = value.v.intValue;
    break;

  default:
    if (value.type != degat_Type_OBJECT)
      return 1;
    return degat_vaParseObject(value.v.objectValue, format, params);
  }

  return 0;
}

int degat_vaParseValues(degat_Value *values, const char *format,
			va_list parameters) {
  while (*format) {
    if (degat_vaParseValue(*values, *format, parameters))
      return 1;
    format++;
    values++;
  }
  return (0);
}

int degat_parseValues(degat_Value *values, const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  int r = degat_vaParseValues(values, format, parameters);
  va_end(parameters);
  return r;
}
