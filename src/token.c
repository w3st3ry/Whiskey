#include <assert.h>
#include <string.h>
#include "whiskey_private.h"


Token wsky_Token_create(Position begin, Position end,
                        const char *string,
                        TokenType type) {
  Token t = {
    .begin = begin,
    .end = end,
    .string = wsky_strdup(string),
    .type = type,
  };
  if (type == wsky_TokenType_STRING)
    t.v.stringValue = NULL;
  return t;
}

void wsky_Token_free(Token *token) {
  switch (token->type) {

  case wsky_TokenType_STRING:
    assert(token->v.stringValue);
    wsky_free(token->v.stringValue);
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

  wsky_free(token->string);
}

bool wsky_Token_isLiteral(const Token *token) {
  TokenType type = token->type;
  if (type == wsky_TokenType_KEYWORD) {
    wsky_Keyword keyword = token->v.keyword;
    return keyword == wsky_Keyword_TRUE ||
      keyword == wsky_Keyword_FALSE ||
      keyword == wsky_Keyword_NULL;
  }
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
  return wsky_asprintf("{type: %s; string: %s}", type, token->string);
}

void wsky_Token_print(const Token *token, FILE *output) {
  char *s = wsky_Token_toString(token);
  fprintf(output, "%s", s);
  wsky_free(s);
}



TokenList *wsky_TokenList_new(Token *token,  TokenList *next) {
  TokenList *list = wsky_safeMalloc(sizeof(TokenList));
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
  wsky_free(list);
}

TokenList *wsky_TokenList_getLast(TokenList *list) {
  if (!list)
    return NULL;
  if (!list->next)
    return list;
  return wsky_TokenList_getLast(list->next);
}

void wsky_TokenList_deleteComments(TokenList **listPointer) {
  TokenList *list = *listPointer;
  if (!list)
    return;

  Token *token = &list->token;
  if (token->type == wsky_TokenType_COMMENT) {
    wsky_Token_free(token);
    *listPointer = (*listPointer)->next;
    wsky_TokenList_deleteComments(listPointer);
    wsky_free(list);
    return;
  }
  wsky_TokenList_deleteComments(&(*listPointer)->next);
}


char *wsky_TokenList_toString(const TokenList *list) {
  char *s = NULL;
  size_t length = 0;
  while (list) {
    char *tokenString = wsky_Token_toString(&list->token);
    s = wsky_realloc(s, length + strlen(tokenString) + 1);
    if (!s)
      abort();
    s[length] = '\0';
    strcat(s, tokenString);
    wsky_free(tokenString);
    length = strlen(s);
    list = list->next;
  }
  if (!s)
    s = wsky_strdup("");
  return s;
}

void wsky_TokenList_print(const TokenList *list, FILE *output) {
  if (!list)
    return;
  wsky_Token_print(&list->token, output);
  wsky_TokenList_print(list->next, output);
}
