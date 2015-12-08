#include <stdio.h>
#include "str.h"
#include "class.h"
#include "gc.h"

int main(int argc, char **argv)
{
  degat_init();
  degat_String *s = degat_String_new("Hello World!");
  printf("%s\n", s->string);
  degat_ReturnValue r = degat_Object_callMethod0((degat_Object *) s,
						 "getLength");
  printf("string length: %d\n", (int) r.v.v.intValue);
  degat_Object_DECREF(s);
  degat_free();
  return (0);
}
