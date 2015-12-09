#ifndef VALUE_H_
# define VALUE_H_

#include "type.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

struct degat_Object_s;
typedef struct degat_Object_s degat_Object;

struct degat_Value_s;
typedef struct degat_Value_s degat_Value;

struct degat_Value_s {
  degat_Type type;
  union {
    bool boolValue;
    int64_t intValue;
    double floatValue;
    degat_Object *objectValue;
  } v;
};


extern const degat_Value degat_Value_TRUE;
extern const degat_Value degat_Value_FALSE;
extern const degat_Value degat_Value_NULL;
extern const degat_Value degat_Value_ZERO;

degat_Value degat_Value_fromBool(bool n);
degat_Value degat_Value_fromObject(degat_Object *object);
degat_Value degat_Value_fromInt(int64_t n);
degat_Value degat_Value_fromFloat(double n);

bool degat_Value_isNull(const degat_Value value);

degat_Value degat_vaBuildValue(const char *format, va_list parameters);
degat_Value degat_buildValue(const char *format, ...);
int degat_buildValues(degat_Value *values, const char *format, ...);

int degat_vaParseValue(degat_Value value, const char format,
		       va_list parameters);
int degat_vaParseValues(degat_Value *values, const char *format,
			va_list parameters);
int degat_parseValues(degat_Value *values, const char *format, ...);

#endif /* !VALUE_H_ */
