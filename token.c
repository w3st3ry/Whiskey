#include "token.h"

#include <stdlib.h>
#include <string.h>


degat_Token degat_Token_create(degat_Position begin,
			       degat_Position end,
			       const char *string) {
  degat_Token t = {
    .begin = begin,
    .end = end,
    .string = strdup(string),
  };
  return t;
}

void degat_Token_free(degat_Token *token) {
  free((char *)token->string);
}

void degat_Token_print(FILE *output) {
  fprintf(output, "Token");
}
