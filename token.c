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
    CASE(INT); CASE(FLOAT); CASE(STRING);
    CASE(IDENTIFIER);
    CASE(OPERATOR);
    CASE(COMMENT);
  }
#undef CASE
}

char *wsky_Token_toString(const wsky_Token *token) {
  const char *type = wsky_TokenType_toString(token);
  char *s = malloc(strlen(token->string) + strlen(type) + 30);
  sprintf(s, "{type: %s; string: %s}", type, token->string);
  return s;
}

void wsky_Token_print(const wsky_Token *token, FILE *output) {
  char *s = wsky_Token_toString(token);
  fprintf(output, "%s", s);
  free(s);
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


char *wsky_TokenList_toString(const wsky_TokenList *list) {
  char *s = NULL;
  size_t length = 0;
  while (list) {
    char *tokenString = wsky_Token_toString(&list->token);
    s = realloc(s, length + strlen(tokenString) + 1);
    s[length] = '\0';
    strcat(s, tokenString);
    free(tokenString);
    length = strlen(s);
    list = list->next;
  }
  if (!s)
    s = strdup("");
  return s;
}

void wsky_TokenList_print(const wsky_TokenList *list, FILE *output) {
  if (!list)
    return;
  wsky_Token_print(&list->token, output);
  wsky_TokenList_print(list->next, output);
}
