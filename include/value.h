#ifndef VALUE_H_
# define VALUE_H_

#include "type.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct wsky_Class_s wsky_Class;
typedef struct wsky_String_s wsky_String;
struct wsky_Object_s;
#define wsky_Object struct wsky_Object_s

/**
 * @defgroup Value Value
 * @{
 */

/**
 * A Whiskey value.
 *
 * Integers, booleans and floats are not objects, and are not
 * garbage-collected.
 * This structure can hold any Wiskey value, whatever its type.
 */
typedef struct wsky_Value_s {

  /** The type of the value */
  wsky_Type type;

  /** An union of the differents types */
  union {
    /** If type == wsky_Type_BOOL */
    bool boolValue;

    /** If type == wsky_Type_INT */
    int64_t intValue;

    /** If type == wsky_Type_FLOAT */
    double floatValue;

    /** If type == wsky_Type_OBJECT */
    wsky_Object *objectValue;
  } v;
} wsky_Value;


extern const wsky_Value wsky_Value_TRUE;
extern const wsky_Value wsky_Value_FALSE;
extern const wsky_Value wsky_Value_NULL;
extern const wsky_Value wsky_Value_ZERO;

wsky_Value wsky_Value_fromBool(bool n);
wsky_Value wsky_Value_fromObject(wsky_Object *object);
wsky_Value wsky_Value_fromInt(int64_t n);
wsky_Value wsky_Value_fromFloat(double n);

bool wsky_Value_isNull(const wsky_Value value);

char *wsky_Value_toCString(const wsky_Value value);
wsky_String *wsky_Value_toString(const wsky_Value value);

const wsky_Class *wsky_Value_getClass(const wsky_Value value);
const char *wsky_Value_getClassName(const wsky_Value value);

/*
 * i: Integer
 * f: Float
 * s: String
 */
wsky_Value wsky_vaBuildValue(const char *format, va_list parameters);
wsky_Value wsky_buildValue(const char *format, ...);
int wsky_buildValues(wsky_Value *values, const char *format, ...);

/*
 * i: Integer
 * f: Float
 * s: Copy the null-terminated string to the character array whose
 * address you pass.
 * S: Store a pointer to a malloc'd string in the character pointer
 * variable whose address you pass. You must free the string.
 */
int wsky_vaParseValue(wsky_Value value, const char format,
                      va_list parameters);
int wsky_vaParseValues(wsky_Value *values, const char *format,
                       va_list parameters);
int wsky_parseValues(wsky_Value *values, const char *format, ...);

/**
 * @}
 */

#undef wsky_Object

#endif /* !VALUE_H_ */
