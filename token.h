#ifndef TOKEN_H_
# define TOKEN_H_

# include "position.h"


struct degat_Token_s;
typedef struct degat_Token_s degat_Token;

struct degat_Token_s
{
  degat_Position begin;
  degat_Position end;
  const char *string;
};


degat_Token degat_Token_create(degat_Position begin,
			       degat_Position end,
			       const char *string);
void degat_Token_free(degat_Token *token);
void degat_Token_print(FILE *output);


#endif /* !TOKEN_H_ */
