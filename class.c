#include "class.h"

#include <stdlib.h>
#include <string.h>
#include "str.h"

void wsky_init(void) {
  wsky_String_initClass();
}

void wsky_free(void) {
  wsky_String_freeClass();
}
