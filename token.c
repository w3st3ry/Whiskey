#include "token.h"

#include <stdlib.h>
#include <string.h>
#include "wsky_gc.h"


typedef wsky_Token Token;
typedef wsky_TokenList TokenList;
typedef wsky_TokenType TokenType;
typedef wsky_Position Position;

Token wsky_Token_create(Position begin, Position end,
                        const char *string,
                        TokenType type) {
  Token t = {
    .begin = begin,
    .end = end,
    .string = wsky_STRDUP(string),
    .type = type,
  };
  if (type == wsky_TokenType_STRING)
    t.v.stringValue = NULL;
  return t;
}

void wsky_Token_free(Token *token) {
  switch (token->type) {

  case wsky_TokenType_STRING:
    if (!token->v.stringValue)
      abort();
    wsky_FREE(token->v.stringValue);
    break;

  case wsky_TokenType_WSKY_STMTS:
  case wsky_TokenType_WSKY_PRINT:
    wsky_TokenList_delete(token->v.children);
    break;

  case wsky_TokenType_HTML:
  case wsky_TokenType_INT: case wsky_TokenType_FLOAT:
  case wsky_TokenType_OPERATOR:
  case wsky_TokenType_IDENTIFIER:
  case wsky_TokenType_KEYWORD:
  case wsky_TokenType_COMMENT:
    break;
  }

  wsky_FREE(token->string);
}

bool wsky_Token_isLiteral(const Token *token) {
  TokenType type = token->type;
  return type == wsky_TokenType_INT ||
    type == wsky_TokenType_FLOAT ||
    type == wsky_TokenType_STRING;
}

static const char *wsky_TokenType_toString(const Token *token) {

#define CASE(type) case wsky_TokenType_ ## type: return #type

  switch (token->type) {
    CASE(HTML);
    CASE(WSKY_PRINT); CASE(WSKY_STMTS);
    CASE(INT); CASE(FLOAT); CASE(STRING);
    CASE(IDENTIFIER);
    CASE(KEYWORD);
    CASE(OPERATOR);
    CASE(COMMENT);
  }

#undef CASE

  return NULL;
}

char *wsky_Token_toString(const Token *token) {
  const char *type = wsky_TokenType_toString(token);
  char *s = wsky_MALLOC(strlen(token->string) + strlen(type) + 30);
  sprintf(s, "{type: %s; string: %s}", type, token->string);
  return s;
}

void wsky_Token_print(const Token *token, FILE *output) {
  char *s = wsky_Token_toString(token);
  fprintf(output, "%s", s);
  wsky_FREE(s);
}



TokenList *wsky_TokenList_new(Token *token,  TokenList *next) {
  TokenList *list = wsky_MALLOC(sizeof(TokenList));
  if (!list) {
    return NULL;
  }
  list->token = *token;
  list->next = next;
  return list;
}

void wsky_TokenList_add(wsky_TokenList **listPointer,
                        wsky_TokenList *new) {
  if (!*listPointer) {
    *listPointer = new;
    return;
  }
  TokenList *last = wsky_TokenList_getLast(*listPointer);
  last->next = new;
}

void wsky_TokenList_addToken(TokenList **listPointer, Token *token) {
  TokenList *new = wsky_TokenList_new(token, NULL);
  wsky_TokenList_add(listPointer, new);
}

void wsky_TokenList_delete(TokenList *list) {
  if (!list)
    return;
  wsky_TokenList_delete(list->next);
  wsky_Token_free(&list->token);
  wsky_FREE(list);
}

TokenList *wsky_TokenList_getLast(TokenList *list) {
  if (!list)
    return NULL;
  if (!list->next)
    return list;
  return wsky_TokenList_getLast(list->next);
}


char *wsky_TokenList_toString(const TokenList *list) {
  char *s = NULL;
  size_t length = 0;
  while (list) {
    char *tokenString = wsky_Token_toString(&list->token);
    s = wsky_REALLOC(s, length + strlen(tokenString) + 1);
    s[length] = '\0';
    strcat(s, tokenString);
    wsky_FREE(tokenString);
    length = strlen(s);
    list = list->next;
  }
  if (!s)
    s = wsky_STRDUP("");
  return s;
}

void wsky_TokenList_print(const TokenList *list, FILE *output) {
  if (!list)
    return;
  wsky_Token_print(&list->token, output);
  wsky_TokenList_print(list->next, output);
}
