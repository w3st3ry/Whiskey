#include "class.h"

#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "function.h"

void wsky_init(void) {
  wsky_String_initClass();
  wsky_Function_initClass();
}

void wsky_free(void) {
  wsky_String_freeClass();
  wsky_Function_freeClass();
}
