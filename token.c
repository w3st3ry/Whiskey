#include "token.h"

#include <stdlib.h>
#include <string.h>


wsky_Token wsky_Token_create(wsky_Position begin,
			     wsky_Position end,
			     const char *string,
			     wsky_TokenType type) {
  wsky_Token t = {
    .begin = begin,
    .end = end,
    .string = strdup(string),
    .type = type,
  };
  if (type == wsky_TokenType_STRING)
    t.v.stringValue = NULL;
  return t;
}

void wsky_Token_free(wsky_Token *token) {
  if (token->type == wsky_TokenType_STRING)
    {
      if (!token->v.stringValue)
	abort();
      free(token->v.stringValue);
    }
  free(token->string);
}

static const char *wsky_TokenType_toString(const wsky_Token *token) {
#define CASE(type) case wsky_TokenType_ ## type: return #type
  switch (token->type) {
    CASE(HTML);
    CASE(INT); CASE(FLOAT); CASE(STRING);
    CASE(IDENTIFIER);
    CASE(OPERATOR);
    CASE(COMMENT);
  }
#undef CASE
}

void wsky_Token_print(const wsky_Token *token, FILE *output) {
  const char *type = wsky_TokenType_toString(token);
  if (!type)
    abort();
  fprintf(output, "Token {type: %s; string: %s}",
	  type, token->string);
}



wsky_TokenList *wsky_TokenList_new(wsky_Token token,  wsky_TokenList *next) {
  wsky_TokenList *list = malloc(sizeof(wsky_TokenList));
  if (!list) {
    return NULL;
  }
  list->token = token;
  list->next = next;
  return list;
}

void wsky_TokenList_add(wsky_TokenList **list_pointer, wsky_Token token) {
  wsky_TokenList *new = wsky_TokenList_new(token, NULL);
  if (!*list_pointer) {
    *list_pointer = new;
    return;
  }
  wsky_TokenList *last = wsky_TokenList_getLast(*list_pointer);
  last->next = new;
}

void wsky_TokenList_delete(wsky_TokenList *list) {
  if (!list)
    return;
  wsky_TokenList_delete(list->next);
  wsky_Token_free(&list->token);
  free(list);
}

wsky_TokenList *wsky_TokenList_getLast(wsky_TokenList *list) {
  if (!list)
    return NULL;
  if (!list->next)
    return list;
  return wsky_TokenList_getLast(list->next);
}

void wsky_TokenList_print(const wsky_TokenList *list, FILE *output) {
  if (!list)
    return;
  wsky_Token_print(&list->token, output);
  wsky_TokenList_print(list->next, output);
}
