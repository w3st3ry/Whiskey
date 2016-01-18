#ifndef VALUE_H_
# define VALUE_H_

#include "type.h"
#include "whiskey.h"
#include <stdbool.h>
#include <stdarg.h>

typedef struct wsky_Class_s wsky_Class;
typedef struct wsky_String_s wsky_String;
typedef struct wsky_Object_s wsky_Object;

/**
 * @defgroup Value Value
 * @{
 */

/**
 * A Whiskey value.
 *
 * Integers, booleans and floats are not objects, and are not
 * garbage-collected.
 * This structure can hold any Whiskey value, whatever its type.
 */
typedef struct wsky_Value_s {

  /** The type of the value */
  wsky_Type type;

  /** An union of the differents types */
  union {
    /** If type == wsky_Type_BOOL */
    bool boolValue;

    /** If type == wsky_Type_INT */
    wsky_int intValue;

    /** If type == wsky_Type_FLOAT */
    wsky_float floatValue;

    /** If type == wsky_Type_OBJECT */
    wsky_Object *objectValue;
  } v;
} wsky_Value;


/** A predefined value for `true` */
extern const wsky_Value wsky_Value_TRUE;

/** A predefined return value for `false` */
extern const wsky_Value wsky_Value_FALSE;

/** A predefined return value for `null` */
extern const wsky_Value wsky_Value_NULL;

/** A predefined return value for `zero` */
extern const wsky_Value wsky_Value_ZERO;

/** Creates a new value from a boolean */
wsky_Value wsky_Value_fromBool(bool n);

/** Creates a new value from a wsky_Object */
wsky_Value wsky_Value_fromObject(wsky_Object *object);

/** Creates a new value from an integer */
wsky_Value wsky_Value_fromInt(wsky_int n);

/** Creates a new value from a float */
wsky_Value wsky_Value_fromFloat(wsky_float n);

/**
 * Return `true` if the type of the given value is OBJECT and its
 * member objectValue is NULL
 */
bool wsky_isNull(const wsky_Value value);

/**
 * Return the class of the given value.
 */
wsky_Class *wsky_getClass(const wsky_Value value);

/**
 * Returns the class name of the given value.
 */
const char *wsky_getClassName(const wsky_Value value);



/**
 * Like wsky_buildValue(), but with a va_list.
 */
wsky_Value wsky_vaBuildValue(const char *format, va_list parameters);

/**
 * Like wsky_buildValues(), but with a single value.
 */
wsky_Value wsky_buildValue(const char *format, ...);

/**
 * Builds Whiskey values from C values and a format string,
 * a bit like printf().
 *
 * The format string can contain the characters:
 *   - i: Integer
 *   - f: Float
 *   - s: String
 */
int wsky_buildValues(wsky_Value *values, const char *format, ...);



/**
 * Like wsky_parseValues(), but with a va_list.
 */
int wsky_vaParseValues(wsky_Value *values, const char *format,
                       va_list parameters);

/**
 * Converts C values to a Whiskey values with a format string,
 * a bit like scanf().
 *
 * The format string can contain the characters:
 *   - i: Integer
 *   - f: Float
 *   - s: Copy the null-terminated string to the character array whose
 *   address you pass.
 *   - S: Store a pointer to a malloc'd string in the character pointer
 *   variable whose address you pass. You must free the string.
 */
int wsky_parseValues(wsky_Value *values, const char *format, ...);

/**
 * @}
 */

#endif /* !VALUE_H_ */
