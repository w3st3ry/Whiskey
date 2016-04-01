#ifndef RETURN_VALUE_H_
# define RETURN_VALUE_H_

# include "value.h"
# include <stdlib.h>

struct wsky_Exception_s;
typedef struct wsky_Exception_s wsky_Exception;

/**
 * This function is for exception-management purposes.
 */
typedef struct wsky_Result_s {

  /** The value if the exception is `NULL` */
  wsky_Value v;

  /**
   * The exception or `NULL`.
   */
  wsky_Exception *exception;

} wsky_Result;


/**
 * Converts a value to a new Whiskey string.
 * Returns a wsky_String or raises an exception.
 */
wsky_Result wsky_toString(wsky_Value value);


/** A predefined return value for `true` */
extern const wsky_Result wsky_Result_TRUE;

/** A predefined return value for `false` */
extern const wsky_Result wsky_Result_FALSE;

/** A predefined return value for `null` */
extern const wsky_Result wsky_Result_NULL;

/** A predefined return value for `0` */
extern const wsky_Result wsky_Result_ZERO;

static inline wsky_Result wsky_Result_fromBool(bool n) {
  return n ? wsky_Result_TRUE : wsky_Result_FALSE;
}

static inline wsky_Result wsky_Result_fromInt(wsky_int n) {
  wsky_Result r = {
    .exception = NULL,
    .v = wsky_Value_fromInt(n)
  };
  return r;
}

static inline wsky_Result wsky_Result_fromFloat(wsky_float n) {
  wsky_Result r = {
    .exception = NULL,
    .v = wsky_Value_fromFloat(n)
  };
  return r;
}

static inline wsky_Result wsky_Result_fromValue(wsky_Value v) {
  wsky_Result r = {
    .exception = NULL,
    .v = v,
  };
  return r;
}

static inline wsky_Result wsky_Result_fromObject(wsky_Object *o) {
  wsky_Result r = {
    .exception = NULL,
    .v = wsky_Value_fromObject(o)
  };
  return r;
}

static
inline wsky_Result wsky_Result_fromException(wsky_Exception *e) {
  wsky_Result r = {
    .exception = e,
    .v = wsky_Value_NULL
  };
  return r;
}


wsky_Result wsky_Result_newException(const char *message);


# define wsky_RETURN_BOOL(n_)   return wsky_Result_fromBool(n_)
# define wsky_RETURN_NULL       return wsky_Result_NULL
# define wsky_RETURN_TRUE       return wsky_Result_TRUE
# define wsky_RETURN_FALSE      return wsky_Result_FALSE
# define wsky_RETURN_INT(n_)    return wsky_Result_fromInt(n_)
# define wsky_RETURN_FLOAT(n_)  return wsky_Result_fromFloat(n_)
# define wsky_RETURN_VALUE(n)   return wsky_Result_fromValue(n)
# define wsky_RETURN_OBJECT(n_) return wsky_Result_fromObject(n_)
# define wsky_RETURN_C_STRING(s)                                        \
  return wsky_Result_fromObject((wsky_Object *) wsky_String_new(s))

# define wsky_RAISE_EXCEPTION(e)                \
  return wsky_Result_fromException(e)

# define wsky_RAISE_NEW_EXCEPTION(msg)          \
  return wsky_Result_newException(msg)

# define wsky_RAISE_NEW_PARAMETER_ERROR(message)                        \
  wsky_RAISE_EXCEPTION((wsky_Exception *)wsky_ParameterError_new(message))

# define wsky_RAISE_NEW_INDEX_ERROR()           \
  wsky_RAISE_NEW_EXCEPTION("Index error")

# define wsky_RAISE_NEW_TYPE_ERROR(message)                             \
  wsky_RAISE_EXCEPTION((wsky_Exception *)wsky_TypeError_new(message))

# define wsky_RAISE_NEW_VALUE_ERROR(message)                            \
  wsky_RAISE_EXCEPTION((wsky_Exception *)wsky_ValueError_new(message))

# define wsky_RAISE_NEW_ATTRIBUTE_ERROR(message)                        \
  wsky_RAISE_EXCEPTION((wsky_Exception *)wsky_AttributeError_new(message))

# define wsky_RAISE_NEW_NOT_IMPLEMENTED_ERROR(message)                  \
  wsky_RAISE_EXCEPTION((wsky_Exception *)wsky_NotImplementedError_new(message))

#endif /* !RETURN_VALUE_H_ */
