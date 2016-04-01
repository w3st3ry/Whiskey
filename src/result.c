#include "whiskey_private.h"


const Result Result_TRUE = {
  .v = {
    .type = Type_BOOL,
    .v = {
      .boolValue = true
    }
  },
  .exception = NULL
};

const Result Result_FALSE = {
  .v = {
    .type = Type_BOOL,
    .v = {
      .boolValue = false
    }
  },
  .exception = NULL
};

const Result Result_NULL = {
  .v = {
    .type = Type_OBJECT,
    .v = {
      .objectValue = NULL
    }
  },
  .exception = NULL
};

const Result Result_ZERO = {
  .v = {
    .type = Type_INT,
    .v = {
      .intValue = 0
    }
  },
  .exception = NULL
};



Result wsky_Result_newException(const char *message) {
  RAISE_EXCEPTION(wsky_Exception_new(message, NULL));
}
