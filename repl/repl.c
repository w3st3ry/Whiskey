#include "repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"


static int eval(const char *string, bool debugMode) {
  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    wsky_SyntaxError_print(&lr.syntaxError, stderr);
    wsky_SyntaxError_free(&lr.syntaxError);
    return 1;
  }
  if (debugMode) {
    printf("tokens:\n");
    wsky_TokenList_print(lr.tokens, stdout);
    printf("\n");
  }

  wsky_ParserResult pr = wsky_parse(lr.tokens);
  if (!pr.success) {
    printf("parser error:\n");
    wsky_SyntaxError_print(&pr.syntaxError, stderr);
    wsky_SyntaxError_free(&pr.syntaxError);
    goto freeTokens;
  }
  if (debugMode) {
    printf("Nodes:\n");
    if (pr.node) {
      wsky_ASTNode_print(pr.node, stdout);
    } else {
      printf("null");
    }
    printf("\n");
  }
  wsky_ASTNode_delete(pr.node);

 freeTokens:
  wsky_TokenList_delete(lr.tokens);

  return 0;
}


static char *readString(void) {
  char *string = malloc(1);
  string[0] = '\0';

  unsigned length = 0;
  while (1) {
    int c = getchar();
    if (c == -1) {
      free(string);
      return NULL;
    }
    if (c == '\n')
      break;
    string = realloc(string, length + 2);
    string[length] = (char) c;
    length += 1;
    string[length] = '\0';
  }
  return string;
}


// TODO: add history
void wsky_repl(bool debugMode) {
  wsky_init();

  for (;;) {
    printf(">>> ");
    char *string = readString();
    if (!string) {
      break;
    }
    eval(string, debugMode);
    free(string);
  }

  wsky_free();
}
