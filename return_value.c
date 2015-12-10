#include "return_value.h"

#include <stdlib.h>
#include "exception.h"



const degat_ReturnValue degat_ReturnValue_TRUE = {
  .v = {
    .type = degat_Type_BOOL,
    .v = {
      .boolValue = true
    }
  },
  .exception = NULL
};
const degat_ReturnValue degat_ReturnValue_FALSE = {
  .v = {
    .type = degat_Type_BOOL,
    .v = {
      .boolValue = false
    }
  },
  .exception = NULL
};
const degat_ReturnValue degat_ReturnValue_NULL = {
  .v = {
    .type = degat_Type_OBJECT,
    .v = {
      .objectValue = NULL
    }
  },
  .exception = NULL
};
const degat_ReturnValue degat_ReturnValue_ZERO = {
  .v = {
    .type = degat_Type_INT,
    .v = {
      .intValue = 0
    }
  },
  .exception = NULL
};



degat_ReturnValue degat_ReturnValue_fromBool(bool n) {
  return n ? degat_ReturnValue_TRUE : degat_ReturnValue_FALSE;
}


degat_ReturnValue degat_ReturnValue_fromInt(int64_t n) {
  degat_ReturnValue r = {
    .exception = NULL,
    .v = degat_Value_fromInt(n)
  };
  return r;
}

degat_ReturnValue degat_ReturnValue_fromFloat(double n) {
  degat_ReturnValue r = {
    .exception = NULL,
    .v = degat_Value_fromFloat(n)
  };
  return r;
}

degat_ReturnValue degat_ReturnValue_fromObject(degat_Object *object) {
  degat_ReturnValue r = {
    .exception = NULL,
    .v = degat_Value_fromObject(object)
  };
  return r;
}

degat_ReturnValue degat_ReturnValue_fromException(degat_Exception *e) {
  degat_ReturnValue r = {
    .exception = e,
    .v = degat_Value_NULL
  };
  return r;
}



degat_ReturnValue degat_ReturnValue_newException(const char *message) {
  degat_RETURN_EXCEPTION(degat_Exception_new(message, NULL));
}
