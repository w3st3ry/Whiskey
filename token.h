#ifndef TOKEN_H_
# define TOKEN_H_

# include "position.h"


struct wsky_Token_s;
typedef struct wsky_Token_s wsky_Token;

struct wsky_Token_s
{
  wsky_Position begin;
  wsky_Position end;
  const char *string;
};


wsky_Token wsky_Token_create(wsky_Position begin,
			       wsky_Position end,
			       const char *string);
void wsky_Token_free(wsky_Token *token);
void wsky_Token_print(FILE *output);


#endif /* !TOKEN_H_ */
