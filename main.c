#include <stdio.h>
#include "lexer.h"
#include "parser.h"

static int eval(const char *string) {
  wsky_init();

  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    wsky_SyntaxError_print(&lr.syntaxError, stderr);
    wsky_SyntaxError_free(&lr.syntaxError);
    goto free;
  }
  printf("tokens:\n");
  wsky_TokenList_print(lr.tokens, stdout);
  printf("\n\n");

  wsky_ParserResult pr = wsky_parse(lr.tokens);
  if (!pr.success) {
    printf("parser error:\n");
    wsky_SyntaxError_print(&pr.syntaxError, stderr);
    wsky_SyntaxError_free(&pr.syntaxError);
    goto freeTokens;
  }
  printf("Nodes:\n");
  if (pr.node) {
    wsky_ASTNode_print(pr.node, stdout);
    wsky_ASTNode_delete(pr.node);
  } else {
    printf("null");
  }
  printf("\n");

 freeTokens:
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
