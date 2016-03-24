#include "whiskey_private.h"
#include <string.h>

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  printf("Whiskey\n");

  bool debugMode = false;
  if (argc >= 2 && strcmp(argv[1], "--debug") == 0)
    debugMode = true;
  wsky_repl(debugMode);
  return 0;
}
