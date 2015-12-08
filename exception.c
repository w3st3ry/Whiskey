#include "exception.h"

#include <stdlib.h>
#include <string.h>
#include "value.h"

const degat_Class degat_Exception_CLASS = {
  .super = &degat_Object_CLASS,
  .name = "Exception",
  .objectSize = sizeof(degat_Exception),
};



degat_Exception *degat_Exception_new(const char *message,
				     degat_Exception *cause) {
  if (!message)
    abort();
  degat_Object *o = NULL;
  degat_Value dMessage = degat_Value_NULL;
  if (degat_Exception_construct(o, 1, &dMessage))
    abort();
  return ((degat_Exception *) o);
}

degat_Exception *degat_Exception_construct(degat_Object *object,
					   unsigned paramCount,
					   degat_Value *params) {
  degat_Exception *this = (degat_Exception *) object;
  this->message = strdup("yolo");
  return (NULL);
}

void degat_Exception_destroy(degat_Object *object) {
  degat_Exception *this = (degat_Exception *) object;
  free(this->message);
}
