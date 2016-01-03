#include "objects/exception.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "value.h"
#include "gc.h"



typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;



static ReturnValue construct(wsky_Object *object,
                             unsigned paramCount,
                             wsky_Value *params);
static ReturnValue destroy(wsky_Object *object);



static wsky_MethodDef methods[] = {
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_Exception_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Exception",
  .constructor = &construct,
  .destructor = &destroy,
  .objectSize = sizeof(Exception),
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

wsky_Class *wsky_Exception_CLASS;



/* TODO: Implement and test the cause */
Exception *wsky_Exception_new(const char *message,
                              Exception *cause) {
  (void) cause;
  wsky_ReturnValue r;
  if (message) {
    wsky_Value v = wsky_buildValue("s", message);
    r = wsky_Object_new(wsky_Exception_CLASS, 1, &v);
  }
  else {
    r = wsky_Object_new(wsky_Exception_CLASS, 0, NULL);
  }
  if (r.exception)
    abort();
  return (Exception *) r.v.v.objectValue;
}

static ReturnValue construct(wsky_Object *object,
                             unsigned paramCount,
                             wsky_Value *params) {
  assert(paramCount <= 1);
  Exception *self = (Exception *) object;
  if (paramCount == 1) {
    assert(!wsky_parseValues(params, "S", &self->message));
  } else {
    self->message = NULL;
  }
  wsky_RETURN_NULL;
}

static ReturnValue destroy(wsky_Object *object) {
  Exception *self = (Exception *) object;
  wsky_FREE(self->message);
  wsky_RETURN_NULL;
}



void wsky_Exception_print(const Exception *self) {
  printf("<Exception");
  if (self->message)
    printf(" message: %s", self->message);
  printf(">");
}
