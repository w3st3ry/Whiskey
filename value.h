#ifndef VALUE_H_
# define VALUE_H_

#include "type.h"
#include <stdint.h>
#include <stdbool.h>

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

#endif /* !VALUE_H_ */
