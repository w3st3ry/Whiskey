#include "objects/exception.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "value.h"
#include "gc.h"
#include "objects/class.h"



typedef wsky_Object Object;
typedef wsky_Exception Exception;
typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;



static ReturnValue construct(Object *object,
                             unsigned paramCount, const Value *params);
static ReturnValue destroy(Object *object);

static ReturnValue raise(Exception *exception);



#define GET_NAME(function, name)                                \
  {#name, 0, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC,    \
      (wsky_Method0)&function}

#define GET(name) GET_NAME(name, name)

static wsky_MethodDef methods[] = {
  GET(raise),
  {0, 0, 0, 0},
};

const wsky_ClassDef wsky_Exception_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Exception",
  .final = false,
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

static ReturnValue construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  assert(paramCount <= 1);
  Exception *self = (Exception *) object;
  if (paramCount == 1)
    assert(!wsky_parseValues(params, "S", &self->message));
  else
    self->message = NULL;
  wsky_RETURN_NULL;
}

static ReturnValue destroy(wsky_Object *object) {
  Exception *self = (Exception *) object;
  wsky_free(self->message);
  wsky_RETURN_NULL;
}


static ReturnValue raise(Exception *exception) {
  wsky_RETURN_EXCEPTION(exception);
}


void wsky_Exception_print(const Exception *self) {
  printf("%s", self->class->name);
  if (self->message)
    printf(": %s", self->message);
  printf("\n");
}
