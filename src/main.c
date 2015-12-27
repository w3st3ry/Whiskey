#include <stdio.h>
#include "repl/repl.h"


int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  printf("Whiskey\n");

  wsky_repl(true);
  return 0;
}
