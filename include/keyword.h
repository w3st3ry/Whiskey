#ifndef KEYWORD_H_
# define KEYWORD_H_

#include <stdbool.h>

typedef enum {
  wsky_Keyword_AND,
  wsky_Keyword_CLASS,
  wsky_Keyword_ELSE,
  wsky_Keyword_FALSE,
  wsky_Keyword_FOR,
  wsky_Keyword_IF,
  wsky_Keyword_IN,
  wsky_Keyword_NOT,
  wsky_Keyword_NULL,
  wsky_Keyword_OR,
  wsky_Keyword_THIS,
  wsky_Keyword_TRUE,
  wsky_Keyword_VAR,
  wsky_Keyword_WHILE,
} wsky_Keyword;

/*
 * Writes the recognized keyword at the given address.
 * Return true on error
 */
bool wsky_Keyword_parse(const char *string, wsky_Keyword *keywordPointer);

#endif /* !KEYWORD_H_ */
