#include <assert.h>
#include <string.h>
#include <time.h>

/*
 * Don't include whiskey_private.h here, because some names
 * conflicts with readline.h
 */
#include "whiskey.h"

#ifdef HAVE_READLINE
# include <readline/readline.h>
# include <readline/history.h>
#endif


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


#ifdef HAVE_READLINE

static char *wsky_readLine(const char *prompt) {
  char *line = readline(prompt);

  /* If the line has any text in it, save it on the history. */
  if (line && *line)
    add_history(line);

  if (!line)
    putchar('\n');

  return line;
}

#else /* HAVE_READLINE */

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

static char *wsky_readLine(const char *prompt) {
  printf("%s", prompt);
  char *s = readString();

  if (!s)
    putchar('\n');

  return s;
}

#endif /* HAVE_READLINE */


static void printSyntaxError(const wsky_SyntaxError *error, bool debugMode) {
  if (!error->expectedSomething) {
    wsky_SyntaxError_print(error, stderr);
    return;
  }

  if (!debugMode)
    return;

  printCsiSgr(90);
  wsky_SyntaxError_print(error, stderr);
  printResetAttributes();
}

/**
 * *expectedSomething is set to true when a syntax error
 * due to an unfinished expression occurs.
 */
static wsky_TokenList *lexToTokenList(const char *string, bool debugMode,
                                      bool *expectedSomething) {
  wsky_LexerResult lr = wsky_lexFromString(string);
  if (!lr.success) {
    *expectedSomething = lr.syntaxError.expectedSomething;
    printSyntaxError(&lr.syntaxError, debugMode);
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
  return lr.tokens;
}

/**
 * *expectedSomething is set to true when a syntax error
 * due to an unfinished expression occurs.
 */
static wsky_ASTNode *parse(const char *string, bool debugMode,
                           bool *expectedSomething) {
  wsky_TokenList *tokens = lexToTokenList(string, debugMode,
                                          expectedSomething);
  if (!tokens)
    return NULL;

  wsky_ParserResult pr = wsky_parseLine(tokens);
  wsky_TokenList_delete(tokens);
  if (!pr.success) {
    *expectedSomething = pr.syntaxError.expectedSomething;
    printSyntaxError(&pr.syntaxError, debugMode);
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

static wsky_ASTNode *readASTNode(bool debugMode, bool *eof) {
  *eof = false;
  char *source = wsky_readLine(">>> ");
  if (!source) {
    *eof = true;
    return NULL;
  }

  bool expectedSomething = false;

  while (true) {
    wsky_ASTNode *node = parse(source, debugMode, &expectedSomething);
    if (node || !expectedSomething) {
      free(source);
      return node;
    }

    char *nextLine = wsky_readLine("... ");
    if (!nextLine) {
      free(source);
      *eof = true;
      return NULL;
    }

    char *new = wsky_asprintf("%s\n%s", source, nextLine);
    free(source);
    free(nextLine);
    if (!new)
      return NULL;
    source = new;
  };
}

static void print_exception(wsky_Exception *exception) {
  printf("%s\n", exception->message);
}

static int evalNode(wsky_ASTNode *node, Scope *scope) {

  assert(node->type == wsky_ASTNodeType_SEQUENCE);

  ReturnValue rv = wsky_evalSequence((wsky_SequenceNode *)node, scope);
  wsky_ASTNode_delete(node);
  if (rv.exception) {
    print_exception(rv.exception);
    return 1;
  }

  rv = wsky_toString(rv.v);
  if (rv.exception) {
    print_exception(rv.exception);
    return 2;
  }

  wsky_String *string = (wsky_String *) rv.v.v.objectValue;
  printf("%s\n", string->string);
  return 0;
}

static int readAndEval(wsky_Scope *scope, bool debugMode, bool *eof) {
  wsky_ASTNode *node = readASTNode(debugMode, eof);
  if (!node)
    return -1;
  return evalNode(node, scope);
}


void wsky_repl(bool debugMode) {
  if (isChristmas()) {
    printf("Merry Christmas!\n");
  }

  wsky_start();

  Scope *scope = wsky_Scope_newRoot(wsky_Module_newMain());
  wsky_eval_pushScope(scope);

  for (;;) {
    bool eof = false;
    readAndEval(scope, debugMode, &eof);
    if (eof)
      break;
  }

  wsky_eval_popScope();
  wsky_stop();
}
