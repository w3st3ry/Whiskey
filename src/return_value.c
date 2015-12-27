#include "return_value.h"

#include <stdlib.h>
#include "objects/exception.h"


typedef wsky_ReturnValue ReturnValue;
typedef wsky_Value Value;


const ReturnValue wsky_ReturnValue_TRUE = {
  .v = {
    .type = wsky_Type_BOOL,
    .v = {
      .boolValue = true
    }
  },
  .exception = NULL
};
const ReturnValue wsky_ReturnValue_FALSE = {
  .v = {
    .type = wsky_Type_BOOL,
    .v = {
      .boolValue = false
    }
  },
  .exception = NULL
};
const ReturnValue wsky_ReturnValue_NULL = {
  .v = {
    .type = wsky_Type_OBJECT,
    .v = {
      .objectValue = NULL
    }
  },
  .exception = NULL
};
const ReturnValue wsky_ReturnValue_ZERO = {
  .v = {
    .type = wsky_Type_INT,
    .v = {
      .intValue = 0
    }
  },
  .exception = NULL
};



ReturnValue wsky_ReturnValue_fromBool(bool n) {
  return n ? wsky_ReturnValue_TRUE : wsky_ReturnValue_FALSE;
}


ReturnValue wsky_ReturnValue_fromInt(int64_t n) {
  ReturnValue r = {
    .exception = NULL,
    .v = wsky_Value_fromInt(n)
  };
  return r;
}

ReturnValue wsky_ReturnValue_fromFloat(double n) {
  ReturnValue r = {
    .exception = NULL,
    .v = wsky_Value_fromFloat(n)
  };
  return r;
}

ReturnValue wsky_ReturnValue_fromValue(Value v) {
  ReturnValue r = {
    .exception = NULL,
    .v = v,
  };
  return r;
}

ReturnValue wsky_ReturnValue_fromObject(wsky_Object *object) {
  ReturnValue r = {
    .exception = NULL,
    .v = wsky_Value_fromObject(object)
  };
  return r;
}

ReturnValue wsky_ReturnValue_fromException(wsky_Exception *e) {
  ReturnValue r = {
    .exception = e,
    .v = wsky_Value_NULL
  };
  return r;
}



ReturnValue wsky_ReturnValue_newException(const char *message) {
  wsky_RETURN_EXCEPTION(wsky_Exception_new(message, NULL));
}
