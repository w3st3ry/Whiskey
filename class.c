#include "class.h"

#include <stdlib.h>
#include <string.h>
#include "str.h"

void degat_init(void) {
  degat_String_initClass();
}

void degat_free(void) {
  degat_String_freeClass();
}
