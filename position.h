#ifndef L_POSITION_H_
# define L_POSITION_H_

# include <stdio.h>
# include "object.h"


struct degat_Position_s;
typedef struct degat_Position_s degat_Position;

struct degat_Position_s
{
  degat_Object *file;
  int index;
  int line;
  int column;
};

void degat_Position_print(const degat_Object *this, FILE *file);
degat_Value *degat_Position_toString(const degat_Object *this, FILE *file);

#endif /* !L_POSITION_H_ */
