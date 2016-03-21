#include "whiskey_private.h"


int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  printf("Whiskey\n");

  wsky_repl(true);
  return 0;
}
