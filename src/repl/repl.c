#include "repl/repl.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "memory.h"
#include "objects/exception.h"
#include "objects/str.h"


typedef wsky_Value Value;
typedef wsky_ReturnValue ReturnValue;
typedef wsky_Scope Scope;


static void printCsiSgr(int n) {
  printf("\x1b[%dm", n);
}

static void printResetAttributes() {
  printCsiSgr(0);
}

static bool isChristmas() {
  time_t rawTime;
  time(&rawTime);
  struct tm *timeInfo;
  timeInfo = localtime(&rawTime);
  return timeInfo->tm_mon == 11 && timeInfo->tm_mday > 20;
}


static wsky_ASTNode *parse(const char *string,
                           bool debugMode) {

  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    wsky_SyntaxError_print(&lr.syntaxError, stderr);
    wsky_SyntaxError_free(&lr.syntaxError);
    return NULL;
  }
  if (debugMode) {
    printCsiSgr(90);
    printf("tokens:\n");
    wsky_TokenList_print(lr.tokens, stdout);
    printf("\n");
    printResetAttributes();
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
    printCsiSgr(90);
    printf("nodes:\n");
    if (pr.node) {
      wsky_ASTNode_print(pr.node, stdout);
    } else {
      printf("null");
    }
    printf("\n");
    printResetAttributes();
  }
  return pr.node;
}

static void print_exception(wsky_Exception *exception) {
  printf("%s\n", exception->message);
}

static int eval(const char *source, Scope *scope, bool debugMode) {
  wsky_ASTNode *node = parse(source, debugMode);
  if (!node)
    return 1;

  assert(node->type == wsky_ASTNodeType_SEQUENCE);

  ReturnValue rv = wsky_evalSequence((wsky_SequenceNode *)node, scope);
  wsky_ASTNode_delete(node);
  if (rv.exception) {
    print_exception(rv.exception);
    return 2;
  }

  rv = wsky_toString(rv.v);
  if (rv.exception) {
    print_exception(rv.exception);
    return 3;
  }

  wsky_String *string = (wsky_String *) rv.v.v.objectValue;
  printf("%s\n", string->string);
  return 0;
}

static char *readString(void) {
  char *string = wsky_safeMalloc(1);
  string[0] = '\0';

  unsigned length = 0;
  while (1) {
    int c = getchar();
    if (c == -1) {
      wsky_free(string);
      return NULL;
    }
    if (c == '\n')
      break;
    string = wsky_realloc(string, length + 2);
    if (!string)
      abort();
    string[length] = (char) c;
    length += 1;
    string[length] = '\0';
  }
  return string;
}


// TODO: add history
void wsky_repl(bool debugMode) {

  if (isChristmas()) {
    printf("Merry Christmas!\n");
  }

  wsky_start();

  Scope *scope = wsky_Scope_newRoot(wsky_Module_newMain());
  wsky_eval_pushScope(scope);

  for (;;) {
    printf(">>> ");
    char *string = readString();
    if (!string) {
      break;
    }
    eval(string, scope, debugMode);
    wsky_free(string);
  }

  wsky_eval_popScope();
  wsky_stop();
}
