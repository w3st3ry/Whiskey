#ifndef TOKEN_H_
# define TOKEN_H_

# include "position.h"


struct wsky_Token_s;
typedef struct wsky_Token_s wsky_Token;

/**
 * A token for the lexer
 */
struct wsky_Token_s {
  wsky_Position begin;
  wsky_Position end;

  /**
   * Read-only
   */
  char *string;
};


wsky_Token wsky_Token_create(wsky_Position begin,
			       wsky_Position end,
			       const char *string);
void wsky_Token_free(wsky_Token *token);

void wsky_Token_print(const wsky_Token *token, FILE *output);



struct wsky_TokenList_s;
typedef struct wsky_TokenList_s wsky_TokenList;

/**
 * A linked list of tokens
 */
struct wsky_TokenList_s {
  wsky_Token token;
  wsky_TokenList *next;
};

/**
 * @param token A token
 * @param next The next element or `NULL`.
 */
wsky_TokenList *wsky_TokenList_new(wsky_Token token,  wsky_TokenList *next);

void wsky_TokenList_add(wsky_TokenList **list_pointer, wsky_Token token);

/**
 * Frees the list and the tokens.
 */
void wsky_TokenList_delete(wsky_TokenList *list);

/**
 * Returns the last element of the list.
 */
wsky_TokenList *wsky_TokenList_getLast(wsky_TokenList *list);

void wsky_TokenList_print(const wsky_TokenList *list, FILE *output);

#endif /* !TOKEN_H_ */
