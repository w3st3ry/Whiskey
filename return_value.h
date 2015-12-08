#ifndef RETURN_VALUE_H_
# define RETURN_VALUE_H_

#include "value.h"

struct degat_Exception_s;
typedef struct degat_Exception_s degat_Exception;

struct degat_ReturnValue_s;
typedef struct degat_ReturnValue_s degat_ReturnValue;

struct degat_ReturnValue_s {
  degat_Value v;
  degat_Exception *exception;
};


extern const degat_ReturnValue degat_ReturnValue_TRUE;
extern const degat_ReturnValue degat_ReturnValue_FALSE;
extern const degat_ReturnValue degat_ReturnValue_NULL;
extern const degat_ReturnValue degat_ReturnValue_ZERO;

degat_ReturnValue degat_ReturnValue_fromBool(bool n);
degat_ReturnValue degat_ReturnValue_fromInt(int64_t n);
degat_ReturnValue degat_ReturnValue_fromFloat(double n);
degat_ReturnValue degat_ReturnValue_fromObject(degat_Object *n);
degat_ReturnValue degat_ReturnValue_fromException(degat_Exception *exception);

#define degat_RETURN_BOOL(n_) return degat_ReturnValue_fromBool(n_)
#define degat_RETURN_TRUE return degat_ReturnValue_TRUE
#define degat_RETURN_FALSE return degat_ReturnValue_FALSE
#define degat_RETURN_INT(n_) return degat_ReturnValue_fromInt(n_)
#define degat_RETURN_FLOAT(n_) return degat_ReturnValue_fromFloat(n_)
#define degat_RETURN_OBJECT(n_) return degat_ReturnValue_fromObject(n_)
#define degat_RETURN_EXCEPTION(n_) return degat_ReturnValue_fromException(n_)

#endif /* !RETURN_VALUE_H_ */
