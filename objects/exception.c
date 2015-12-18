#include "exception.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "value.h"
#include "gc.h"



typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;



static Exception *construct(wsky_Object *object,
                            unsigned paramCount,
                            wsky_Value *params);
static void destroy(wsky_Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0},
};

wsky_Class wsky_Exception_CLASS = {
  .super = &wsky_Object_CLASS,
  .name = "Exception",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Exception),
  .methodDefs = methods,
};



Exception *wsky_Exception_new(const char *message,
                              Exception *cause) {
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
  return (Exception *) r.v.v.objectValue;
}

static Exception *construct(wsky_Object *object,
                            unsigned paramCount,
                            wsky_Value *params) {
  if (paramCount > 1)
    abort();
  Exception *this = (Exception *) object;
  if (paramCount == 1) {
    if (wsky_parseValues(params, "S", &this->message))
      abort();
  } else {
    this->message = NULL;
  }
  return NULL;
}

static void destroy(wsky_Object *object) {
  Exception *this = (Exception *) object;
  free(this->message);
}



void wsky_Exception_print(const Exception *this) {
  printf("<Exception");
  if (this->message)
    printf(" message: %s", this->message);
  printf(">");
}
