#include "token.h"

#include <stdlib.h>
#include <string.h>


wsky_Token wsky_Token_create(wsky_Position begin,
			       wsky_Position end,
			       const char *string) {
  wsky_Token t = {
    .begin = begin,
    .end = end,
    .string = strdup(string),
  };
  return t;
}

void wsky_Token_free(wsky_Token *token) {
  free(token->string);
}

void wsky_Token_print(const wsky_Token *token, FILE *output) {
  (void) token;
  fprintf(output, "Token");
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
