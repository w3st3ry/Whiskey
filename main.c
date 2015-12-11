#include <stdio.h>
#include "str.h"
#include "class.h"
#include "gc.h"

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  wsky_init();

  wsky_String *s = wsky_String_new("Hello World!");
  printf("%s\n", s->string);
  wsky_ReturnValue r = wsky_Object_callMethod0((wsky_Object *) s,
						 "getLength");
  printf("string length: %d\n", (int) r.v.v.intValue);
  wsky_DECREF(s);

  wsky_free();
  return 0;
}
