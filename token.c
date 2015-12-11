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

void wsky_Token_print(FILE *output) {
  fprintf(output, "Token");
}
