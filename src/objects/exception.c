#include <assert.h>
#include <string.h>
#include "../whiskey_private.h"


static Result construct(Object *object,
                             unsigned paramCount, const Value *params);
static Result destroy(Object *object);

static Result raise(Exception *exception);

static Result getMessage(Exception *exception);



#define GET_NAME(function, name)                                \
  {#name, 0, wsky_MethodFlags_GET | wsky_MethodFlags_PUBLIC,    \
      (wsky_Method0)&function}

#define GET(name) GET_NAME(name, name)

static MethodDef methods[] = {
  GET(raise),
  GET_NAME(getMessage, message),
  {0, 0, 0, 0},
};

const ClassDef wsky_Exception_CLASS_DEF = {
  .super = &wsky_Object_CLASS_DEF,
  .name = "Exception",
  .final = false,
  .constructor = &construct,
  .privateConstructor = false,
  .destructor = &destroy,
  .methodDefs = methods,
  .gcAcceptFunction = NULL,
};

Class *wsky_Exception_CLASS;



/* TODO: Implement and test the cause */
Exception *wsky_Exception_new(const char *message,
                              Exception *cause) {
  (void) cause;
  Result r;
  if (message) {
    Value v = wsky_buildValue("s", message);
    r = wsky_Object_new(wsky_Exception_CLASS, 1, &v);
  } else {
    r = wsky_Object_new(wsky_Exception_CLASS, 0, NULL);
  }
  if (r.exception)
    abort();
  return (Exception *) r.v.v.objectValue;
}

static Result construct(Object *object,
                             unsigned paramCount,
                             const Value *params) {
  assert(paramCount <= 1);
  Exception *self = (Exception *) object;
  if (paramCount == 1)
    assert(!wsky_parseValues(params, "S", &self->message));
  else
    self->message = NULL;
  RETURN_NULL;
}

static Result destroy(Object *object) {
  Exception *self = (Exception *) object;
  wsky_free(self->message);
  RETURN_NULL;
}


static Result raise(Exception *exception) {
  RAISE_EXCEPTION(exception);
}

static Result getMessage(Exception *exception) {
  RETURN_C_STRING(exception->message);
}

void wsky_Exception_print(const Exception *self) {
  printf("%s", self->class->name);
  if (self->message)
    printf(": %s", self->message);
  printf("\n");
}
