#include "exception.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "value.h"
#include "gc.h"

degat_Class degat_Exception_CLASS = {
  .super = &degat_Object_CLASS,
  .name = "Exception",
  .constructor = &degat_Exception_construct,
  .destructor = &degat_Exception_destroy,
  .objectSize = sizeof(degat_Exception),
};



degat_Exception *degat_Exception_new(const char *message,
				     degat_Exception *cause) {
  (void) cause;
  degat_ReturnValue r;
  if (message)
    {
      degat_Value v = degat_buildValue("s", message);
      r = degat_Object_new(&degat_Exception_CLASS, 1, &v);
      degat_Value_DECREF(v);
    }
  else
    {
      r = degat_Object_new(&degat_Exception_CLASS, 0, NULL);
    }
  if (r.exception)
    return NULL;
  return (degat_Exception *) r.v.v.objectValue;
}

degat_Exception *degat_Exception_construct(degat_Object *object,
					   unsigned paramCount,
					   degat_Value *params) {
  if (paramCount > 1)
    abort();
  degat_Exception *this = (degat_Exception *) object;
  if (paramCount == 1) {
    if (degat_parseValues(params, "S", &this->message))
      abort();
  } else {
    this->message = NULL;
  }
  return NULL;
}

void degat_Exception_destroy(degat_Object *object) {
  degat_Exception *this = (degat_Exception *) object;
  free(this->message);
}



void degat_Exception_print(const degat_Exception *this) {
  printf("Exception");
  if (this->message)
    printf("{message: %s}", this->message);
}
