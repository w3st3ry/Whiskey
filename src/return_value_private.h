#ifndef RETURN_VALUE_PRIVATE_H
# define RETURN_VALUE_PRIVATE_H

# include "whiskey.h"

typedef wsky_ReturnValue ReturnValue;

# define ReturnValue_TRUE       wsky_ReturnValue_TRUE
# define ReturnValue_FALSE      wsky_ReturnValue_FALSE
# define ReturnValue_NULL       wsky_ReturnValue_NULL
# define ReturnValue_ZERO       wsky_ReturnValue_ZERO

# define ReturnValue_fromBool           wsky_ReturnValue_fromBool
# define ReturnValue_fromInt            wsky_ReturnValue_fromInt
# define ReturnValue_fromFloat          wsky_ReturnValue_fromFloat
# define ReturnValue_fromValue          wsky_ReturnValue_fromValue
# define ReturnValue_fromObject         wsky_ReturnValue_fromObject
# define ReturnValue_fromException      wsky_ReturnValue_fromException

# define RETURN_BOOL            wsky_RETURN_BOOL
# define RETURN_NULL            wsky_RETURN_NULL
# define RETURN_TRUE            wsky_RETURN_TRUE
# define RETURN_FALSE           wsky_RETURN_FALSE
# define RETURN_INT             wsky_RETURN_INT
# define RETURN_FLOAT           wsky_RETURN_FLOAT
# define RETURN_VALUE           wsky_RETURN_VALUE
# define RETURN_OBJECT          wsky_RETURN_OBJECT
# define RETURN_C_STRING        wsky_RETURN_C_STRING

# define RAISE_EXCEPTION                wsky_RAISE_EXCEPTION
# define RAISE_NEW_EXCEPTION            wsky_RAISE_NEW_EXCEPTION
# define RAISE_NEW_PARAMETER_ERROR      wsky_RAISE_NEW_PARAMETER_ERROR
# define RAISE_NEW_INDEX_ERROR          wsky_RAISE_NEW_INDEX_ERROR
# define RAISE_NEW_TYPE_ERROR           wsky_RAISE_NEW_TYPE_ERROR
# define RAISE_NEW_ATTRIBUTE_ERROR      wsky_RAISE_NEW_ATTRIBUTE_ERROR
# define RAISE_NEW_NOT_IMPLEMENTED_ERROR wsky_RAISE_NEW_NOT_IMPLEMENTED_ERROR

#endif /* RETURN_VALUE_PRIVATE_H */
