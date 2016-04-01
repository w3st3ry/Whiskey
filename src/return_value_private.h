#ifndef RETURN_VALUE_PRIVATE_H
# define RETURN_VALUE_PRIVATE_H

# include "whiskey.h"

typedef wsky_Result Result;

# define Result_TRUE       wsky_Result_TRUE
# define Result_FALSE      wsky_Result_FALSE
# define Result_NULL       wsky_Result_NULL
# define Result_ZERO       wsky_Result_ZERO

# define Result_fromBool           wsky_Result_fromBool
# define Result_fromInt            wsky_Result_fromInt
# define Result_fromFloat          wsky_Result_fromFloat
# define Result_fromValue          wsky_Result_fromValue
# define Result_fromObject         wsky_Result_fromObject
# define Result_fromException      wsky_Result_fromException

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
# define RAISE_NEW_ATTRIBUTE_ERROR      wsky_RAISE_NEW_ATTRIBUTE_ERROR
# define RAISE_NEW_EXCEPTION            wsky_RAISE_NEW_EXCEPTION
# define RAISE_NEW_INDEX_ERROR          wsky_RAISE_NEW_INDEX_ERROR
# define RAISE_NEW_NOT_IMPLEMENTED_ERROR wsky_RAISE_NEW_NOT_IMPLEMENTED_ERROR
# define RAISE_NEW_PARAMETER_ERROR      wsky_RAISE_NEW_PARAMETER_ERROR
# define RAISE_NEW_TYPE_ERROR           wsky_RAISE_NEW_TYPE_ERROR
# define RAISE_NEW_VALUE_ERROR          wsky_RAISE_NEW_VALUE_ERROR

#endif /* RETURN_VALUE_PRIVATE_H */
