#ifndef L_POSITION_H_
# define L_POSITION_H_

# include <stdio.h>
# include "objects/program_file.h"



struct wsky_Position_s;
typedef struct wsky_Position_s wsky_Position;

struct wsky_Position_s {

  /** The file or `NULL` */
  wsky_ProgramFile *file;

  int index;
  int line;
  int column;
};

bool wsky_Position_equals(const wsky_Position *a, const wsky_Position *b);

void wsky_Position_print(const wsky_Position *self, FILE *file);

/**
 * Returns a new malloc'd string
 */
char *wsky_Position_toString(const wsky_Position *self);



#endif /* !L_POSITION_H_ */
