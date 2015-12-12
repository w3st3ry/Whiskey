#include <stdio.h>
#include "lexer.h"

static int eval(const char *string) {
  wsky_init();

  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    wsky_SyntaxError_print(&lr.syntaxError, stderr);
    wsky_SyntaxError_free(&lr.syntaxError);
    goto free;
  }
  wsky_TokenList_print(lr.tokens, stdout);
  printf("\n");
  wsky_TokenList_delete(lr.tokens);

 free:
  wsky_free();
  return 0;
}

int main(int argc, char **argv)
{
  if (argc <= 1)
    return 1;
  return eval(argv[1]);;
}
