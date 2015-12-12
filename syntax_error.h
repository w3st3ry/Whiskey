#ifndef SYNTAX_ERROR_H_
# define SYNTAX_ERROR_H_


# include "position.h"

typedef struct wsky_SyntaxError_s wsky_SyntaxError;

struct wsky_SyntaxError_s {
  wsky_Position position;
  char *message;
};


wsky_SyntaxError wsky_SyntaxError_create(const char *message,
					 wsky_Position position);
wsky_SyntaxError *wsky_SyntaxError_new(const char *message,
				       wsky_Position position);

void wsky_SyntaxError_free(wsky_SyntaxError *this);
void wsky_SyntaxError_delete(wsky_SyntaxError *this);

char *wsky_SyntaxError_toString(const wsky_SyntaxError *this);
void wsky_SyntaxError_print(const wsky_SyntaxError *this, FILE *output);


#endif /* !SYNTAX_ERROR_H_ */
