#include "exception.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "value.h"
#include "gc.h"

wsky_Class wsky_Exception_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Exception",
  .constructor = &wsky_Exception_construct,
  .destructor = &wsky_Exception_destroy,
  .objectSize = sizeof(wsky_Exception),
};



wsky_Exception *wsky_Exception_new(const char *message,
                                   wsky_Exception *cause) {
  (void) cause;
  wsky_ReturnValue r;
  if (message) {
    wsky_Value v = wsky_buildValue("s", message);
    r = wsky_Object_new(&wsky_Exception_CLASS, 1, &v);
    wsky_Value_DECREF(v);
  }
  else {
    r = wsky_Object_new(&wsky_Exception_CLASS, 0, NULL);
  }
  if (r.exception)
    return NULL;
  return (wsky_Exception *) r.v.v.objectValue;
}

wsky_Exception *wsky_Exception_construct(wsky_Object *object,
                                         unsigned paramCount,
                                         wsky_Value *params) {
  if (paramCount > 1)
    abort();
  wsky_Exception *this = (wsky_Exception *) object;
  if (paramCount == 1) {
    if (wsky_parseValues(params, "S", &this->message))
      abort();
  } else {
    this->message = NULL;
  }
  return NULL;
}

void wsky_Exception_destroy(wsky_Object *object) {
  wsky_Exception *this = (wsky_Exception *) object;
  free(this->message);
}



void wsky_Exception_print(const wsky_Exception *this) {
  printf("Exception");
  if (this->message)
    printf("{message: %s}", this->message);
}
