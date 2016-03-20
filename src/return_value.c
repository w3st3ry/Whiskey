#include "return_value_private.h"


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
  RAISE_EXCEPTION(wsky_Exception_new(message, NULL));
}
