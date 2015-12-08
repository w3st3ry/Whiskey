#include "value.h"

#include <stdlib.h>



const degat_Value degat_Value_NULL = {
  .type = degat_Type_OBJECT,
  .v = {
    .objectValue = NULL
  }
};
const degat_Value degat_Value_ZERO = {
  .type = degat_Type_INT,
  .v = {
    .intValue = 0
  }
};
const degat_Value degat_Value_TRUE = {
  .type = degat_Type_BOOL,
  .v = {
    .boolValue = true
  }
};
const degat_Value degat_Value_FALSE = {
  .type = degat_Type_BOOL,
  .v = {
    .boolValue = false
  }
};

degat_Value degat_Value_fromBool(bool n) {
  return n ? degat_Value_TRUE : degat_Value_FALSE;
}

degat_Value degat_Value_fromObject(degat_Object *object) {
  degat_Value v = {
    .type = degat_Type_OBJECT,
    .v = {
      .objectValue = object
    }
  };
  return v;
}

degat_Value degat_Value_fromInt(int64_t n) {
  degat_Value v = {
    .type = degat_Type_INT,
    .v = {
      .intValue = n
    }
  };
  return v;
}

degat_Value degat_Value_fromFloat(double n) {
  degat_Value v = {
    .type = degat_Type_FLOAT,
    .v = {
      .floatValue = n
    }
  };
  return v;
}

bool degat_Value_isNull(const degat_Value value) {
  return value.type == degat_Type_OBJECT &&
    !value.v.objectValue;
}
