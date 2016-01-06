#ifndef RETURN_VALUE_H_
# define RETURN_VALUE_H_

# include "value.h"

struct wsky_Exception_s;
typedef struct wsky_Exception_s wsky_Exception;

/**
 * This function is for exception-management purposes.
 */
typedef struct wsky_ReturnValue_s {

  /** The value if the exception is `NULL` */
  wsky_Value v;

  /**
   * The exception or `NULL`.
   */
  wsky_Exception *exception;

} wsky_ReturnValue;


/** A predefined return value for `true` */
extern const wsky_ReturnValue wsky_ReturnValue_TRUE;

/** A predefined return value for `false` */
extern const wsky_ReturnValue wsky_ReturnValue_FALSE;

/** A predefined return value for `null` */
extern const wsky_ReturnValue wsky_ReturnValue_NULL;

/** A predefined return value for `0` */
extern const wsky_ReturnValue wsky_ReturnValue_ZERO;

wsky_ReturnValue wsky_ReturnValue_fromBool(bool n);
wsky_ReturnValue wsky_ReturnValue_fromInt(wsky_int n);
wsky_ReturnValue wsky_ReturnValue_fromFloat(wsky_float n);
wsky_ReturnValue wsky_ReturnValue_fromValue(wsky_Value n);
wsky_ReturnValue wsky_ReturnValue_fromObject(wsky_Object *n);
wsky_ReturnValue wsky_ReturnValue_fromException(wsky_Exception *e);

wsky_ReturnValue wsky_ReturnValue_newException(const char *message);

# define wsky_RETURN_BOOL(n_) return wsky_ReturnValue_fromBool(n_)
# define wsky_RETURN_NULL return wsky_ReturnValue_NULL
# define wsky_RETURN_TRUE return wsky_ReturnValue_TRUE
# define wsky_RETURN_FALSE return wsky_ReturnValue_FALSE
# define wsky_RETURN_INT(n_) return wsky_ReturnValue_fromInt(n_)
# define wsky_RETURN_FLOAT(n_) return wsky_ReturnValue_fromFloat(n_)
# define wsky_RETURN_VALUE(n) return wsky_ReturnValue_fromValue(n)
# define wsky_RETURN_OBJECT(n_) return wsky_ReturnValue_fromObject(n_)
# define wsky_RETURN_CSTRING(s)                                         \
  return wsky_ReturnValue_fromObject((wsky_Object *) wsky_String_new(s))

# define wsky_RETURN_EXCEPTION(n_)                              \
  return wsky_ReturnValue_fromException((wsky_Exception *) n_)

# define wsky_RETURN_NEW_EXCEPTION(msg)         \
  return wsky_ReturnValue_newException(msg)

# define wsky_RETURN_NEW_PARAMETER_ERROR(message)               \
  wsky_RETURN_EXCEPTION(wsky_ParameterError_new(message))

# define wsky_RETURN_NEW_INDEX_ERROR()          \
  wsky_RETURN_NEW_EXCEPTION("Index error")

# define wsky_RETURN_NEW_TYPE_ERROR(message)            \
  wsky_RETURN_EXCEPTION(wsky_TypeError_new(message))

# define wsky_RETURN_NEW_ATTRIBUTE_ERROR(message)               \
  wsky_RETURN_EXCEPTION(wsky_AttributeError_new(message))

#endif /* !RETURN_VALUE_H_ */
