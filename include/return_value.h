#ifndef RETURN_VALUE_H_
# define RETURN_VALUE_H_

# include "value.h"

struct wsky_Exception_s;
typedef struct wsky_Exception_s wsky_Exception;

struct wsky_ReturnValue_s;
typedef struct wsky_ReturnValue_s wsky_ReturnValue;

/**
 * This function is for exception-management purposes.
 */
struct wsky_ReturnValue_s {
  wsky_Value v;

  /**
   * The exception or `NULL`.
   */
  wsky_Exception *exception;
};


extern const wsky_ReturnValue wsky_ReturnValue_TRUE;
extern const wsky_ReturnValue wsky_ReturnValue_FALSE;
extern const wsky_ReturnValue wsky_ReturnValue_NULL;
extern const wsky_ReturnValue wsky_ReturnValue_ZERO;

wsky_ReturnValue wsky_ReturnValue_fromBool(bool n);
wsky_ReturnValue wsky_ReturnValue_fromInt(int64_t n);
wsky_ReturnValue wsky_ReturnValue_fromFloat(double n);
wsky_ReturnValue wsky_ReturnValue_fromValue(wsky_Value n);
wsky_ReturnValue wsky_ReturnValue_fromObject(wsky_Object *n);
wsky_ReturnValue wsky_ReturnValue_fromException(wsky_Exception *e);

wsky_ReturnValue wsky_ReturnValue_newException(const char *message);

# define wsky_RETURN_BOOL(n_) return wsky_ReturnValue_fromBool(n_)
# define wsky_RETURN_TRUE return wsky_ReturnValue_TRUE
# define wsky_RETURN_FALSE return wsky_ReturnValue_FALSE
# define wsky_RETURN_INT(n_) return wsky_ReturnValue_fromInt(n_)
# define wsky_RETURN_FLOAT(n_) return wsky_ReturnValue_fromFloat(n_)
# define wsky_RETURN_VALUE(n) return wsky_ReturnValue_fromValue(n)
# define wsky_RETURN_OBJECT(n_) return wsky_ReturnValue_fromObject(n_)
# define wsky_RETURN_CSTRING(s)                                         \
  return wsky_ReturnValue_fromObject((wsky_Object *) wsky_String_new(s))

# define wsky_RETURN_EXCEPTION(n_) return wsky_ReturnValue_fromException(n_)

# define wsky_RETURN_NEW_EXCEPTION(msg)         \
  return wsky_ReturnValue_newException(msg)

# define wsky_RETURN_NEW_PARAMETER_ERROR()      \
  wsky_RETURN_NEW_EXCEPTION("Parameter error")

# define wsky_RETURN_NEW_SYNTAX_ERROR()         \
  wsky_RETURN_NEW_EXCEPTION("Syntax error")

# define wsky_RETURN_NEW_INDEX_ERROR()          \
  wsky_RETURN_NEW_EXCEPTION("Index error")

# define wsky_RETURN_NEW_TYPE_ERROR()           \
  wsky_RETURN_NEW_EXCEPTION("Type error")



#endif /* !RETURN_VALUE_H_ */
