#ifndef TOKEN_H_
# define TOKEN_H_

# include <stdint.h>
# include "position.h"
# include "operator.h"
# include "keyword.h"

/**
 * The type of a wsky_Token
 */
typedef enum {
  /** For templates only */
  wsky_TokenType_HTML,
  wsky_TokenType_WSKY_PRINT,
  wsky_TokenType_WSKY_STMTS,

  wsky_TokenType_INT,
  wsky_TokenType_FLOAT,
  wsky_TokenType_STRING,

  wsky_TokenType_IDENTIFIER,

  wsky_TokenType_KEYWORD,

  wsky_TokenType_OPERATOR,

  wsky_TokenType_COMMENT,
} wsky_TokenType;

struct wsky_Token_s;
typedef struct wsky_Token_s wsky_Token;

struct wsky_TokenList_s;
typedef struct wsky_TokenList_s wsky_TokenList;



/**
 * A token for the lexer
 */
struct wsky_Token_s {
  wsky_Position begin;
  wsky_Position end;

  char *string;

  wsky_TokenType type;

  union {

    /** For STRING type only */
    char *stringValue;

    double floatValue;
    int64_t intValue;

    /** For WSKY_PRINT and WSKY_STMTS types only */
    wsky_TokenList *children;

    /** For OPERATOR type only */
    wsky_Operator operator;

    /** For KEYWORD type only */
    wsky_Keyword keyword;
  } v;
};


wsky_Token wsky_Token_create(wsky_Position begin,
                             wsky_Position end,
                             const char *string,
                             wsky_TokenType type);
void wsky_Token_free(wsky_Token *token);

bool wsky_Token_isLiteral(const wsky_Token *token);

/**
 * Returns a malloc'd string
 */
char *wsky_Token_toString(const wsky_Token *token);
void wsky_Token_print(const wsky_Token *token, FILE *output);



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
wsky_TokenList *wsky_TokenList_new(wsky_Token *token,  wsky_TokenList *next);

void wsky_TokenList_add(wsky_TokenList **listPointer,
                        wsky_TokenList *new);

void wsky_TokenList_addToken(wsky_TokenList **listPointer,
                             wsky_Token *token);

/**
 * Frees the list and the tokens.
 */
void wsky_TokenList_delete(wsky_TokenList *list);

/**
 * Returns the last element of the list.
 */
wsky_TokenList *wsky_TokenList_getLast(wsky_TokenList *list);

/**
 * Returns a malloc'd string
 */
char *wsky_TokenList_toString(const wsky_TokenList *list);

void wsky_TokenList_print(const wsky_TokenList *list, FILE *output);

#endif /* !TOKEN_H_ */
