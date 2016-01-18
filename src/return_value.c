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



ReturnValue wsky_ReturnValue_newException(const char *message) {
  wsky_RETURN_EXCEPTION(wsky_Exception_new(message, NULL));
}
