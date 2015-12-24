#include "repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "gc.h"
#include "exception.h"


static wsky_ASTNode *parse(const char *string,
                           bool debugMode) {

  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    wsky_SyntaxError_print(&lr.syntaxError, stderr);
    wsky_SyntaxError_free(&lr.syntaxError);
    return NULL;
  }
  if (debugMode) {
    printf("tokens:\n");
    wsky_TokenList_print(lr.tokens, stdout);
    printf("\n");
  }

  wsky_ParserResult pr = wsky_parseLine(lr.tokens);
  wsky_TokenList_delete(lr.tokens);
  if (!pr.success) {
    printf("parser error:\n");
    wsky_SyntaxError_print(&pr.syntaxError, stderr);
    wsky_SyntaxError_free(&pr.syntaxError);
    return NULL;
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
  return pr.node;
}

static int eval(const char *source, wsky_Scope *scope, bool debugMode) {
  wsky_ASTNode *node = parse(source, debugMode);
  if (!node) {
    return 1;
  }

  wsky_ReturnValue rv = wsky_evalNode(node, scope);
  if (rv.exception) {
    printf("%s\n", rv.exception->message);
    return 2;
  }
  wsky_ASTNode_delete(node);

  char *string = wsky_Value_toCString(rv.v);
  printf("%s\n", string);
  wsky_FREE(string);
  return 0;
}

static char *readString(void) {
  char *string = wsky_MALLOC(1);
  string[0] = '\0';

  unsigned length = 0;
  while (1) {
    int c = getchar();
    if (c == -1) {
      wsky_FREE(string);
      return NULL;
    }
    if (c == '\n')
      break;
    string = wsky_REALLOC(string, length + 2);
    string[length] = (char) c;
    length += 1;
    string[length] = '\0';
  }
  return string;
}


// TODO: add history
void wsky_repl(bool debugMode) {
  wsky_start();

  wsky_Scope *scope = wsky_Scope_new(NULL, NULL);

  for (;;) {
    printf(">>> ");
    char *string = readString();
    if (!string) {
      break;
    }
    eval(string, scope, debugMode);
    wsky_FREE(string);
  }

  wsky_stop();
}
