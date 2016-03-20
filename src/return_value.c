#include "whiskey_private.h"


const ReturnValue ReturnValue_TRUE = {
  .v = {
    .type = Type_BOOL,
    .v = {
      .boolValue = true
    }
  },
  .exception = NULL
};

const ReturnValue ReturnValue_FALSE = {
  .v = {
    .type = Type_BOOL,
    .v = {
      .boolValue = false
    }
  },
  .exception = NULL
};

const ReturnValue ReturnValue_NULL = {
  .v = {
    .type = Type_OBJECT,
    .v = {
      .objectValue = NULL
    }
  },
  .exception = NULL
};

const ReturnValue ReturnValue_ZERO = {
  .v = {
    .type = Type_INT,
    .v = {
      .intValue = 0
    }
  },
  .exception = NULL
};



ReturnValue wsky_ReturnValue_newException(const char *message) {
  RAISE_EXCEPTION(wsky_Exception_new(message, NULL));
}
