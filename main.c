#include <stdio.h>
#include "repl.h"
#include <gc.h>



int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  printf("Whiskey\n");

  GC_INIT();

  wsky_repl(true);
  return 0;
}
